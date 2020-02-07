#version 420

// Vertex Shader Input
in vec4 fVertWorldLocation;
in vec4 fNormal;
in vec4 fUVx2;

// Object
uniform vec4 diffuseColour;
uniform vec4 specularColour;
uniform vec4 eyeLocation;

// Used to draw debug (or unlit) objects
uniform bool isUniform;
uniform bool isSkybox;

// Texture
uniform sampler2D textSamp00;	// albedo
uniform sampler2D textSamp01;	// normal
uniform sampler2D textSamp02;	// position
uniform sampler2D textSamp03;	// bloom
uniform sampler2D textSamp04;	// unlit


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

const float GOLDEN_ANGLE = 2.39996323; 
const float MAX_BLUR_SIZE = 20.0; 
const float RAD_SCALE = 5.0; // Smaller = nicer blur, larger = faster

float getBlurSize(float depth, float focusPoint, float focusScale)
{
	float coc = clamp((1.0 / focusPoint - 1.0 / depth)*focusScale, -1.0, 1.0);
	return abs(coc) * MAX_BLUR_SIZE;
}

vec3 depthOfField(vec2 texCoord, float focusPoint, float focusScale)
{
	vec2 uPixelSize = vec2(1.0 / iResolution.x, 1.0 / iResolution.y);
	float centerDepth = distance(texture(textSamp02, texCoord).rgb, eyeLocation.rgb);
	float centerSize = getBlurSize(centerDepth, focusPoint, focusScale);
	vec3 color = texture(textSamp00, texCoord).rgb;
	float tot = 1.0;
	float radius = RAD_SCALE;
	for (float ang = 0.0; radius<MAX_BLUR_SIZE; ang += GOLDEN_ANGLE)
	{
		vec2 tc = texCoord + vec2(cos(ang), sin(ang)) * uPixelSize * radius;
		vec3 sampleColor = texture(textSamp00, tc).rgb;
		float sampleDepth = distance(texture(textSamp02, tc).rgb, eyeLocation.rgb);
		float sampleSize = getBlurSize(sampleDepth, focusPoint, focusScale);
		if (sampleDepth > centerDepth)
			sampleSize = clamp(sampleSize, 0.0, centerSize*2.0);
		float m = smoothstep(radius-0.5, radius+0.5, sampleSize);
		color += mix(color/tot, sampleColor, m);
		tot += 1.0;   radius += RAD_SCALE/radius;
	}
	return color /= tot;
}


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

	float brightness = dot(colour.rgb, vec3(0.2126, 0.6152, 0.522) * 0.7);
	//float brightness = dot(colour.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness <= 1.0)
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

	return BrightColor;
}

const int num_circle_samples = 10;

vec4 circular_blur(sampler2D tex, float offset)
{
	vec4 colour = vec4(0.0);
	float contribution = 0.7 / (num_circle_samples);
	float angle_step = 360.0 / num_circle_samples;
	for (int i = 0; i < num_circle_samples; ++i)
	{
		vec2 uv = fUVx2.xy;
		float angle = angle_step * i;
		uv.x += sin(angle) * offset;
		uv.y += cos(angle) * offset;
		colour.rgb += texture(tex, uv).rgb * contribution;
	}
	colour.rgb += texture(tex, fUVx2.xy).rgb * 0.3;
	colour.a = 1.0;
	return colour;
}

const float focal_distance = 5.0;
const float focus_length = 5.0;
vec4 DOF(sampler2D tex)
{
	vec4 colour = texture(tex, fUVx2.st);

	float z = distance(texture(textSamp02, fUVx2.st).rgb, eyeLocation.rgb);
//	float n = focal_distance - focus_length;
//	float f = focal_distance + focus_length;
//	float zfocus = focal_distance;
//	float d = (f / (f - n)) - ((n * f) / (f - n)) * (1.0 / z);
//
//	float s = 1.0;
//	float coc = abs(s * (1.0 - (zfocus / n)) + (s * zfocus * ((f-n)/(n*f))) * d);
//	colour = circular_blur(tex, coc / 10000.0);

	float distance_from_focus = abs(focal_distance - z);
	float range = focus_length;
	float ratio = min((distance_from_focus / range) - 1.0, 0.0);
	
	colour = circular_blur(tex, clamp(abs(distance_from_focus * ratio) / 500.0, 0.0, 0.01));

	return colour;
}

void main()  
{
	vec2 uv = fUVx2.st;
	unlitColour = vec4(0.0);

	if (isFinalPass)
	{
		//pixelColour.rgb = texture(textSamp00, uv).rgb;
		pixelColour.rgb = depthOfField(uv, 10.0, 5.0);
		//pixelColour = circular_blur(textSamp00);
		//pixelColour.rgb = DOF(textSamp00).rgb;
		pixelColour.a = 1.0;

		vec3 bloom = circular_blur(textSamp03, 0.008).rgb * 1.0;
		pixelColour += vec4(bloom, 0.0);

		pixelColour = clamp(pixelColour, 0.0, 1.0);

		float exposure = 2.0;
		float gamma = 0.6;

		// tone mapping
		vec3 result = vec3(1.0) - exp(-pixelColour.rgb * exposure);
		// also gamma correct while we're at it       
		result = pow(result, vec3(1.0 / gamma));

		pixelColour.rgb = result;
		//pixelColour.rgb = bloom;

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