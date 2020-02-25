#version 420

// Vertex Shader Input
in vec4 fVertWorldLocation;
in vec4 fNormal;
in vec4 fUVx2;

// Object
uniform vec4 diffuseColour;
uniform vec4 specularColour;
uniform vec4 eyeLocation;
uniform vec4 eyeTarget;

// Used to draw debug (or unlit) objects
uniform bool isUniform;
uniform bool isSkybox;

uniform bool bloomEnabled;
uniform bool DOFEnabled;
uniform bool volumetricEnabled;

// Texture
uniform sampler2D textSamp00;	// albedo
uniform sampler2D textSamp01;	// normal
uniform sampler2D textSamp02;	// position
uniform sampler2D textSamp03;	// bloom
uniform sampler2D textSamp04;	// unlit

uniform vec2 lightPositionOnScreen;


// Globals
in float fiTime;
in float fisWater;
uniform vec2 iResolution;

uniform bool isFinalPass;



layout (location = 0) out vec4 pixelColour;			// RGB A   (0 to 1) 
layout (location = 1) out vec4 normalColour;	
layout (location = 2) out vec4 positionColour;
layout (location = 3) out vec4 bloomColour;			// BLOOM   (0 to 1) 
layout (location = 4) out vec4 unlitColour;			// BLOOM   (0 to 1) 

// Fragment shader
struct sLight
{
	vec4 position;			
	vec4 diffuse;	
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

// Lights
const int LIGHT_BUFFER = 24;
uniform int NUMBEROFLIGHTS;
uniform sLight theLights[LIGHT_BUFFER];

const int num_samples = 13;

vec4 gaussian_blur(sampler2D tex)
{

	vec4 rgba = vec4(0.0);
	float offset = 0.005;

	vec3 offsets[num_samples] = {
		vec3(0., 0., 0.2),		// CENTER

		vec3(offset, 0., 0.1),	// EDGES
		vec3(0., offset, 0.1),
		vec3(-offset, 0., 0.1),
		vec3(0., -offset, 0.1),

		vec3(offset, offset, 0.05),	// IMMEDIATE CORNERS
		vec3(-offset, offset, 0.05),
		vec3(-offset, offset, 0.05),
		vec3(-offset, -offset, 0.05),

		vec3(offset * 2.0, 0., 0.05),	// FAR EDGES
		vec3(0., offset * 2.0, 0.05),
		vec3(-offset * 2.0, 0., 0.05),
		vec3(0., -offset * 2.0, 0.05),
	};

	//if (length(texture(tex, fUVx2.st + offsets[0].xy).rgb) > 0.0001) return vec4(0.0, 0.0, 0.0, 1.0);

	for (int i = 0; i < num_samples; ++i)
	{
		rgba += texture( tex, fUVx2.st + offsets[i].xy ).rgba * offsets[i].z;
	}

	rgba.a = 1.0;

	return rgba;
}

vec4 NoEffect()
{
	vec4 tex = texture( textSamp00, fUVx2.st ).rgba;
	tex.a = 1.0;
	return tex;
}

vec4 calcualteLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular );


vec4 BloomCutoff(vec4 colour)
{
	vec4 BrightColor = vec4(colour.rgb, 1.0);

	float brightness = dot(colour.rgb, vec3(0.2126, 0.6152, 0.522));
	brightness = dot(colour.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness <= 1.0)
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

	return BrightColor;
}



///
// Computes blur for a given texture and pixel offset
// using a circular blur technique
///
vec4 circular_blur(sampler2D tex, float offset)
{
	const int num_circle_samples = 10;
	const float AOE_strength = 0.7;
	// get the size of each pixel on the screen
	vec2 pixelSize = vec2(1.0 / iResolution.x, 1.0 / iResolution.y);
	vec4 colour = vec4(0.0);
	float contribution = AOE_strength / (num_circle_samples);
	float angle_step = 360.0 / num_circle_samples;
	for (int i = 0; i < num_circle_samples; ++i)
	{
		float angle = radians(angle_step * i);
		vec2 uv = fUVx2.xy + vec2(cos(angle), sin(angle)) * pixelSize * offset;
		colour.rgb += texture(tex, uv).rgb * contribution;
	}
	colour.rgb += texture(tex, fUVx2.xy).rgb * (1. - AOE_strength);
	colour.a = 1.0;
	return colour;
}

///
// Computes DOF for a given texture
///
vec4 DOF(sampler2D mainTexture, sampler2D positionTexture)
{
	const float near_focus = 10.0;
	const float focus_length = 60.0;
	const float blur_scale = 5.0;

	vec4 colour = vec4(0., 0., 0., 1.);

	float z = distance(texture(positionTexture, fUVx2.st).rgb, eyeLocation.rgb);

	float distance_from_focus = abs(near_focus - z);
	float ratio = 0.0;
	if (z < near_focus)
		//ratio = clamp(1.0 / (z / 10.), 0.0, 40.0);
		ratio = smoothstep(0., 1., 1. - (z / near_focus));
	else
		ratio = clamp((distance_from_focus / focus_length) - 1.0, 0.0, 1.0);

	colour = circular_blur(mainTexture, blur_scale * ratio);

	return colour;
}

vec4 CalculateVolumetricLightScattering(sampler2D tex)
{
	// direction of the camera
	vec3 viewDir = normalize(eyeTarget.rgb - eyeLocation.rgb);

	// direction of the light
	vec3 lightDir = normalize(theLights[0].position.rgb - eyeLocation.rgb);

	// ratio decreases when the camera is looking away from the light
	float exposureRatio = smoothstep(0.0, 1.0, clamp(dot(viewDir, lightDir) - 0.2, 0.1, 1.0));

	vec2 uv = fUVx2.st;
	vec2 origin = uv;
	vec4 colour = vec4(texture(tex, uv).rgb, 1);

	// clamp the light position so that the delta is not too high
	vec2 lightPos = lightPositionOnScreen.xy;
	if (distance(lightPositionOnScreen.xy, vec2(0.0)) > 20)
	{
		lightPos = normalize(lightPositionOnScreen.xy) * 20.0;
	}

	float density = 0.97;
	float weight = 0.5;
	float exposure = 0.1 * exposureRatio;
	float decay = 0.9;
	int NUM_SAMPLES = 100;

	vec2 deltaTextCoord = vec2( uv - lightPos.xy ) * 0.02;
    deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density;
    float illuminationDecay = 1.0;

    for(int i=0; i < NUM_SAMPLES && distance(origin, uv) < distance(origin, lightPos)  ; i++)
    {
            uv -= deltaTextCoord;
            vec4 samp = texture2D(tex, uv);
            samp *= illuminationDecay * weight;
            colour += samp;
            illuminationDecay *= decay;
    }

	float dustAmount = length(colour.rgb);
    colour *= exposure;
	uv = vec2(fUVx2.x + (fiTime / (15.0)), fUVx2.y + (fiTime / -20.0));
	colour.rgb += (texture(textSamp04, uv).rgb / 2.0) * exposure * dustAmount;

	return colour;
}

void main()  
{
	vec2 uv = fUVx2.st;
	unlitColour = vec4(0.0);

	if (isFinalPass)
	{
		pixelColour.rgb = texture(textSamp00, uv).rgb;
		
		if (DOFEnabled)
		{
			pixelColour.rgb = DOF(textSamp00, textSamp01).rgb;
		}
		
		if (bloomEnabled)
		{
			vec3 bloom = circular_blur(textSamp03, 10.0).rgb * 1.0;
			pixelColour += vec4(bloom, 0.0);

			pixelColour = clamp(pixelColour, 0.0, 1.0);

			float exposure = 2.0;
			float gamma = 0.6;

			// tone mapping
			vec3 result = vec3(1.0) - exp(-pixelColour.rgb * exposure);

			// gamma correction       
			result = pow(result, vec3(1.0 / gamma));

			pixelColour.rgb = result;
		}

		if (volumetricEnabled)
		{
			pixelColour.rgb += CalculateVolumetricLightScattering(textSamp02).rgb;
		}

		pixelColour.a = 1.0;

		return;
	}

	vec3 col = NoEffect().rgb;
	vec3 normal = texture( textSamp01, uv ).rgb - 1.;
	vec3 pos = texture( textSamp02, uv ).rgb;

	pixelColour = calcualteLightContrib(col, normal, pos, vec4(0.));
	if (texture(textSamp04, uv).r > 0.0)
	{
		pixelColour.rgb = col;
	}
	bloomColour = BloomCutoff(pixelColour);

	positionColour.rgb = pos;
	positionColour.a = 1.;

	normalColour = vec4(distance(pos, eyeLocation.xyz)) / 1000.0;
	normalColour.r *= theLights[0].diffuse.r;
	normalColour.g *= theLights[0].diffuse.g;
	normalColour.b *= theLights[0].diffuse.b;

	return;
	
} // end main


vec4 calcualteLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, 
                            vec3 vertexWorldPos, vec4 vertexSpecular )
{
	vec3 norm = normalize(vertexNormal);
	
	vec4 finalObjectColour = vec4( 0.0, 0.0, 0.0, 1.0 );
	
	for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
	{	
		// ********************************************************
		// is light "on"
		if ( theLights[index].param2.x == 0.0f )
		{	// it's off
			continue;
		}
		
		// Cast to an int (note with c'tor)
		int intLightType = int(theLights[index].param1.x);
		
		// We will do the directional light here... 
		// (BEFORE the attenuation, since sunlight has no attenuation, really)
		if ( intLightType == DIRECTIONAL_LIGHT_TYPE )		// = 2
		{
			// This is supposed to simulate sunlight. 
			// SO: 
			// -- There's ONLY direction, no position
			// -- Almost always, there's only 1 of these in a scene
			// Cheapest light to calculate. 

			vec3 lightContrib = theLights[index].diffuse.rgb;
			
			// Get the dot product of the light and normalize
			float dotProduct = dot( -theLights[index].direction.xyz,  
									   normalize(norm.xyz) );	// -1 to 1

			dotProduct = max( 0.0f, dotProduct );		// 0 to 1
		
			lightContrib *= dotProduct;		
			
			finalObjectColour.rgb += (vertexMaterialColour.rgb * theLights[index].diffuse.rgb * lightContrib); 
									 //+ (materialSpecular.rgb * lightSpecularContrib.rgb);
			// NOTE: There isn't any attenuation, like with sunlight.
			// (This is part of the reason directional lights are fast to calculate)


			//return finalObjectColour;	
			continue;
		}
		
		// Assume it's a point light 
		// intLightType = 0
		
		// Contribution for this light
		vec3 vLightToVertex = theLights[index].position.xyz - vertexWorldPos.xyz;
		float distanceToLight = length(vLightToVertex);	
		vec3 lightVector = normalize(vLightToVertex);
		float dotProduct = dot(lightVector, vertexNormal.xyz);	 
		
		dotProduct = max( 0.0f, dotProduct );	
		
		vec3 lightDiffuseContrib = dotProduct * theLights[index].diffuse.rgb;
			

		// Specular 
		vec3 lightSpecularContrib = vec3(0.0f);
			
		vec3 reflectVector = reflect( -lightVector, normalize(norm.xyz) );

		// Get eye or view vector
		// The location of the vertex in the world to your eye
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

		// To simplify, we are NOT using the light specular value, just the object’s.
		float objectSpecularPower = vertexSpecular.w; 
		
//		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
//			                   * vertexSpecular.rgb;	//* theLights[lightIndex].Specular.rgb
		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
			                   * theLights[index].specular.rgb;
							   
		// Attenuation
		float attenuation = 1.0f / 
				( theLights[index].atten.x + 										
				  theLights[index].atten.y * distanceToLight +						
				  theLights[index].atten.z * distanceToLight*distanceToLight );  	
				  
		// total light contribution is Diffuse + Specular
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;
		
		
		// But is it a spot light
		if ( intLightType == SPOT_LIGHT_TYPE )		// = 1
		{	
		

			// Yes, it's a spotlight
			// Calcualate light vector (light to vertex, in world)
			vec3 vertexToLight = vertexWorldPos.xyz - theLights[index].position.xyz;

			vertexToLight = normalize(vertexToLight);

			float currentLightRayAngle
					= dot( vertexToLight.xyz, theLights[index].direction.xyz );
					
			currentLightRayAngle = max(0.0f, currentLightRayAngle);

			//vec4 param1;	
			// x = lightType, y = inner angle, z = outer angle, w = TBD

			// Is this inside the cone? 
			float outerConeAngleCos = cos(radians(theLights[index].param1.z));
			float innerConeAngleCos = cos(radians(theLights[index].param1.y));
							
			// Is it completely outside of the spot?
			if ( currentLightRayAngle < outerConeAngleCos )
			{
				// Nope. so it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if ( currentLightRayAngle < innerConeAngleCos )
			{
				// Angle is between the inner and outer cone
				// (this is called the penumbra of the spot light, by the way)
				// 
				// This blends the brightness from full brightness, near the inner cone
				//	to black, near the outter cone
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / 
									  (innerConeAngleCos - outerConeAngleCos);
									  
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}
						
		}// if ( intLightType == 1 )
		
		
					
		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb)
								  + (vertexSpecular.rgb  * lightSpecularContrib.rgb );

	}//for(intindex=0...
	
	finalObjectColour.a = 1.0f;
	
	return finalObjectColour;
}