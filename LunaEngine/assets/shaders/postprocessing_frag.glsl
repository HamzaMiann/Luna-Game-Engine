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
uniform mat4 shadowMVP;

// Used to draw debug (or unlit) objects
uniform bool isUniform;
uniform bool isSkybox;

uniform bool bloomEnabled;
uniform bool DOFEnabled;
uniform bool volumetricEnabled;
uniform bool cloudsEnabled;
uniform bool cloudShadowsEnabled;
uniform bool vignetteEnabled;
uniform bool lensDirtEnabled;
uniform bool shadowsEnabled;

// Texture
uniform sampler2D textSamp00;	// albedo
uniform sampler2D textSamp01;	// normal
uniform sampler2D textSamp02;	// position
uniform sampler2D textSamp03;	// bloom
uniform sampler2D textSamp04;	// unlit
uniform sampler2D textSamp05;	// REFLECTIVE

uniform sampler3D worleyTexture;
uniform sampler2D perlinTexture;
uniform sampler2D lensTexture;
uniform sampler2D UITexture;
uniform sampler2D GraveTexture;
uniform sampler2DShadow shadowTexture;
//uniform sampler2D shadowTexture;

uniform samplerCube skyBox;

uniform vec2 lightPositionOnScreen;
uniform float cloudDensityFactor;
uniform float cloudDensityCutoff;
uniform float cloudLightScattering;

uniform float mixValue;
uniform float bloomScale;

// Globals
in float fiTime;
in float fisWater;
uniform vec2 iResolution;

uniform bool isFinalPass;
uniform float blendRatio;



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
const int DIRECTIONAL_POSITIONED_LIGHT = 3;

// Lights
const int LIGHT_BUFFER = 24;
uniform int NUMBEROFLIGHTS;
uniform sLight theLights[LIGHT_BUFFER];

const int num_samples = 13;

float rand(vec2 st) { 
	return fract(sin(dot(st.xy,
                 vec2(12.9898,78.233)))
                 * 43758.5453123);
}

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
	vec4 BrightColour = vec4(0.2126, 0.5152, 0.522, 1.0);

	colour.r = (colour.r + BrightColour.r) * colour.r;
	colour.g = (colour.r + BrightColour.g) * colour.g;
	colour.b = (colour.r + BrightColour.b) * colour.b;
	colour.rgb = clamp(colour.rgb, 0., 1.) * bloomScale;

	return colour;

//	float brightness = dot(colour.rgb, vec3(0.2126, 0.6152, 0.522));
//	brightness = dot(colour.rgb, vec3(0.2126, 0.7152, 0.0722));
//    if(brightness <= 1.0)
//        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
//
//	return BrightColor;
}



///
// Computes blur for a given texture and pixel offset
// using a circular blur technique
///
vec4 circular_blur(sampler2D tex, float offset)
{
	const int num_circle_samples = 15;
	const float AOE_strength = 0.9;
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
	const float near_focus = 5.0;
	const float focus_length = 70.0;
	const float far_blur_scale = 2.0;
	const float near_blur_scale = 10.0;

	vec4 colour = vec4(0., 0., 0., 1.);

	float z = distance(texture(positionTexture, fUVx2.st).rgb, eyeLocation.rgb);

	float distance_from_focus = abs(near_focus - z);
	float ratio = 0.0;
	if (z < near_focus)
	{
		//ratio = clamp(1.0 / (z / 10.), 0.0, 40.0);
		ratio = smoothstep(0., 1., 1. - (z / near_focus));
		colour = circular_blur(mainTexture, near_blur_scale * ratio);
	}
	else
	{
		ratio = clamp((distance_from_focus / focus_length) - 1.0, 0.0, 1.0);
		colour = circular_blur(mainTexture, far_blur_scale * ratio);
	}


	return colour;
}

vec4 CalculateVolumetricLightScattering(sampler2D tex)
{
	// direction of the camera
	vec3 viewDir = normalize(eyeTarget.rgb - eyeLocation.rgb);

	// direction of the light
	vec3 lightDir = normalize(theLights[0].position.rgb - eyeLocation.rgb);

	// ratio decreases when the camera is looking away from the light
	float exposureRatio = smoothstep(0.1, 1.0, clamp(dot(viewDir, lightDir) - 0.2, 0.3, 1.0));

	vec2 uv = fUVx2.st;
	vec2 origin = uv;
	vec4 colour = vec4(texture(tex, uv).rgb, 1);

	// get the UV position of the light
	vec2 lightPos = (lightPositionOnScreen.xy + 1.) / 2.;

	float density = 0.97;
	float weight = 0.5;
	float exposure = 0.3 * exposureRatio;
	float decay = 0.98;
	int NUM_SAMPLES = 100;

	vec2 deltaTextCoord = vec2( uv - lightPos.xy );// * 0.02;
    deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density;
    float illuminationDecay = 1.0;

    for(int i=0; i < NUM_SAMPLES  ; i++)
    {
            uv -= deltaTextCoord;
            vec4 samp = texture2D(tex, uv);
            samp *= illuminationDecay * weight;
            colour += samp;
            illuminationDecay *= decay;
    }

	float dustAmount = length(colour.rgb);
    colour *= exposure;
	uv = origin - vec2(lightPos.x + (fiTime / (30.0)), lightPos.y + (fiTime / -40.0));
	colour *= 1. - texture(textSamp04, uv).r;
	//colour.rgb += (texture(textSamp04, uv - lightPositionOnScreen.xy / 400.).rgb / 2.0) * exposure * dustAmount;

	return colour;
}

struct Ray
{
	vec3 ro;
	vec3 rd;
};

struct Plane
{
	vec3 n;
	vec3 p;
	float d;
};

float intersect(Ray ray, Plane p)
{
	if (dot(ray.rd, p.n) >= 0.0)
	{
		return 0.0;
	}

	float t = (-dot(ray.ro, p.n) + p.d) / dot(ray.rd, p.n);
	return t;
}

Plane GetPlane1()
{
	Plane p;
	p.n = vec3(0, -1, 0);
	p.p = vec3(1, 100, 0);
	p.d = -100;
	return p;
}

Plane GetPlane2()
{
	Plane p;
	p.n = vec3(0, -1, 0);
	p.p = vec3(1, 120, 0);
	p.d = -120;
	return p;
}

float GetDensityAt(vec3 position)
{
	float density = 0.;
	vec3 col = textureLod(worleyTexture, position, 0.).rgb;
	density = col.r;
	density = mix(density, col.g, 0.3);
	density = mix(density, col.b, 0.2);
	density = max(0., density - cloudDensityCutoff);
	return density;
}

float GetDensityPerlinAt(vec3 position)
{
	float density = 0.;
	vec3 col = mix(texture(worleyTexture, position).rgb, texture(perlinTexture, position.xy + position.z).rgb, 0.5);
	density = col.r;
	density = mix(density, col.g, 0.3);
	density = mix(density, col.b, 0.2);
	density = max(0., density - cloudDensityCutoff);
	return density;
}

const float timeFactor = 2.0;
vec4 RayTracePlane(Ray ray, vec4 previousColour)
{
	vec4 col = previousColour.rgba;
	vec2 uv = fUVx2.st;

	float t = intersect(ray, GetPlane1());
	if (t > 0.0 && (t < distance(texture( textSamp02, uv ).xyz, ray.ro)))
	{
		vec3 P = (ray.ro + ray.rd * t);

		ray.ro = P;

		float t2 = intersect(ray, GetPlane2());
		vec3 P2 = (ray.ro + ray.rd * t2);

		const int LIGHT_SAMPLES = 5;
		const int NUM_DENSITY_SAMPLES = 20;
		vec3 origin = P;
		vec3 marchStep = (P2 - P) / NUM_DENSITY_SAMPLES;

		float density = 0.;
		float lightDensity = 0.;

		for (int i = 0; i < NUM_DENSITY_SAMPLES; ++i)
		{
			vec3 uv3 = (origin + marchStep * i).xzy;
			ray.ro = uv3.xzy;
			ray.rd = normalize(theLights[0].position.xyz - eyeLocation.xyz);

			float t3 = intersect(ray, GetPlane2());
			if (t3 > 0.)
			{
				if (t3 > 20.) t3 = 20.;
				vec3 mStep = ((ray.ro + ray.rd * t3) - ray.ro) / LIGHT_SAMPLES;
				for (int n = 0; n < LIGHT_SAMPLES; ++n)
				{
					vec3 uv4 = (ray.ro + mStep * n).xzy;
					uv4.xy /= 400.;
					uv4.xy += fiTime / (40. * timeFactor);
					uv4.z += fiTime / (100.  * timeFactor);
					lightDensity += GetDensityAt(uv4);
				}
			}

			uv3.xy /= 400.;
			uv3.xy += fiTime / (40. * timeFactor);
			uv3.z += fiTime / (100.  * timeFactor);
			density += GetDensityAt(uv3);
			//density += fbm3D(uv3).r / float(NUM_DENSITY_SAMPLES);
		}

		float ratio = exp(-density / cloudDensityFactor);//(t / (3. * 20.));
		float lightRatio = exp(-lightDensity / cloudDensityFactor) * cloudLightScattering;
		col.rgb *= ratio;
		col.rgb += (1. - ratio) * (lightRatio * theLights[0].diffuse.rgb);
	}

	return col;
}

float RayTraceShadows(Ray ray)
{
	vec4 col = pixelColour.rgba;
	vec2 uv = fUVx2.st;
	float ratio = 1.;

	float t = intersect(ray, GetPlane1());

	if (t > 0.0)
	{
		vec3 P = (ray.ro + ray.rd * t);

		ray.ro = P;

		float t2 = intersect(ray, GetPlane2());
		vec3 P2 = (ray.ro + ray.rd * t2);

		const int NUM_DENSITY_SAMPLES = 10;
		vec3 origin = P;
		vec3 marchStep = (P2 - P) / NUM_DENSITY_SAMPLES;

		float density = 0.;

		for (int i = 0; i < NUM_DENSITY_SAMPLES; ++i)
		{
			vec3 uv3 = (origin + marchStep * i).xzy;
			uv3.xy /= 400.;
			uv3.xy += fiTime / (40. * timeFactor);
			uv3.z += fiTime / (100. * timeFactor);
			density += GetDensityAt(uv3);
		}

		ratio = exp(-density / cloudDensityFactor);
		//vec3 colour =  (ratio) * col.rgb;
		//col.rgb = mix(pixelColour.rgb, colour, 0.5);
	}

	//return col;
	return ratio;
}

uniform bool switchColour;

void main()
{
	vec2 uv = fUVx2.st;
	unlitColour = vec4(0.0);
	pixelColour.a = 1.;

	if (isFinalPass)
	{
		//pixelColour.rgb = texture(textSamp00, uv).rgb;
		
		float smoothmix = smoothstep(0., 1., mixValue);


		if (switchColour && mixValue > 0.) {
			pixelColour.rgb = circular_blur(textSamp00, 50.0 * (1.0 - smoothmix)).rgb * (smoothmix + 0.2);
		}
		else {
			pixelColour.rgb = texture(textSamp00, uv).rgb;
		}

		// DOF effect
		if (DOFEnabled)
		{
			pixelColour.rgb = mix(pixelColour.rgb, DOF(textSamp00, textSamp01).rgb, smoothmix);
		}
		
		// bloom effect
		if (bloomEnabled)
		{
			vec3 bloom = circular_blur(textSamp03, 10.0).rgb * 1.0;
			pixelColour += vec4(bloom, 0.0);

			pixelColour = clamp(pixelColour, 0.0, 1.0);

			float exposure = 2.0;
			float gamma = 0.7;

			// tone mapping
			vec3 result = vec3(1.0) - exp(-pixelColour.rgb * exposure);

			// gamma correction       
			result = pow(result, vec3(1.0 / gamma));

			pixelColour.rgb = result;
		}

		// volumetric light scattering effect
		if (volumetricEnabled)
		{
			vec3 scatter = CalculateVolumetricLightScattering(textSamp02).rgb;
			pixelColour.rgb += scatter;
			// lens dirt
			if (lensDirtEnabled) {
				vec3 dirt = vec3(texture(lensTexture, uv).r);
				float dist = exp(-distance((lightPositionOnScreen + 1.) / 2., uv) * 7.);
				pixelColour.rgb += dirt * dist * length(scatter * 10.);
			}
		}


		// vignette effect
		if (vignetteEnabled)
		{
			float d = smoothstep(0., 1., distance(vec2(0.5) * 0.8, fUVx2.st * 0.8));
			pixelColour.rgb = mix(pixelColour.rgb, vec3(0.), d * 1.2);
		}

		float disp = smoothstep(0., 1., 1. - distance(vec2(0.5), fUVx2.st) * 10. * (1. - blendRatio));
		vec2 UVX = uv + rand(uv + mod(fiTime,1.0)) / 80.;
		vec3 graves =  texture(GraveTexture, UVX).rgb;
		pixelColour.rgb = mix(pixelColour.rgb, texture(textSamp02, uv).rgb + graves, disp * blendRatio);
		//pixelColour.rgb = mix(pixelColour.rgb, vec3(texture(worleyTexture, (eyeLocation.xyz / 10.) + vec3(uv, 1.0)).rgb), disp * blendRatio);


		if (true) {
			// cinematic black bars
			if (abs(uv.y - 0.5) > 0.4)
			{
				pixelColour.rgb = vec3(0.);
			}
		}

		vec3 ui = texture(UITexture, uv).rgb;
		if (length(ui) > 0.1)
		{
			pixelColour.rgb = ui;
		}

		pixelColour.a = 1.0;

		return;
	}
	

	vec3 col = NoEffect().rgb;
	vec3 normal = texture( textSamp01, uv ).rgb - 1.;
	vec3 pos = texture( textSamp02, uv ).rgb;
	vec2 specularBuffer = texture(textSamp03, uv).xy;
	vec4 spec = vec4(specularBuffer.x);
	spec.w = specularBuffer.y * 1000.;

	//float d = distance(eyeLocation.xyz, pos);

	vec2 lpos = (lightPositionOnScreen.xy + 1.) / 2.;

	// calculate lighting
	pixelColour = calcualteLightContrib(col, normal, pos, spec);
	
	// calculate reflections
	if (texture(textSamp05, uv).r > 0.0)
	{
		vec3 direction = normalize(pos - eyeLocation.xyz);
		pixelColour.rgb = texture(skyBox, reflect(direction, vec3(0.0, 1.0, 0.0))).rgb;
	}

	unlitColour.xyz = texture(textSamp03, uv).xyz;
	unlitColour.a = 1.0;

	// remove lighting if the pixel is part of the skybox
	if (texture(textSamp04, uv).r > 0.0)
	{
		pixelColour.rgb = col;
	}

	
	// clouds effect
	if (cloudsEnabled) {
		Ray ray;
		ray.ro = eyeLocation.xyz;
		ray.rd = normalize(texture(textSamp02, uv).xyz - ray.ro);
		pixelColour = RayTracePlane(ray, pixelColour);
	}
	

	pixelColour.a = 1.0;

	// calculate bloom from given lighting
	bloomColour = BloomCutoff(pixelColour);	// BLOOM TEXTURE

	// set the position buffer for next pass
	positionColour.rgb = pos;				// POSITION TEXTURE, just copy it to the next buffer
	positionColour.a = 1.;

	

	// volumetric lighting buffer calculation
	//normalColour = vec4(distance(pos, eyeLocation.xyz)) / 1000.0;	// CUSTOM COLOURED DEPTH BUFFER FOR VOLUMETRIC LIGHTING
	//normalColour.rgb = clamp(normalColour.rgb, vec3(0.), vec3(1.));
	//normalColour.rgb = vec3(distance(pos, eyeLocation.xyz)) / 1000.0;
	normalColour.r *= theLights[0].diffuse.r;
	normalColour.g *= theLights[0].diffuse.g;
	normalColour.b *= theLights[0].diffuse.b;
	normalColour.rgba = vec4(0., 0. ,0., 1.);

	if (texture(textSamp04, uv).r > 0.0)
	{
		if (distance(uv * vec2(16., 9.), lpos  * vec2(16., 9.)) < 0.2)
		{
			normalColour.rgb = theLights[0].diffuse.rgb;
		}
	}

	// add clouds to the volumetric lighting buffer
	if (cloudsEnabled) {
		Ray ray;
		ray.ro = eyeLocation.xyz;
		ray.rd = normalize(texture(textSamp02, uv).xyz - ray.ro);
		normalColour = RayTracePlane(ray, normalColour);
	}
	
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

		if ( intLightType == DIRECTIONAL_POSITIONED_LIGHT )
		{
			vec3 lightContrib = theLights[index].diffuse.rgb;
			vec3 dir = normalize(eyeLocation.xyz - theLights[index].position.xyz);
			// Get the dot product of the light and normalize
			float dotProduct = dot( -dir,  
									   normalize(norm.xyz) );	// -1 to 1

			dotProduct = max( 0.0f, dotProduct );		// 0 to 1
		
			lightContrib *= dotProduct;

			// cloud shadows
			float cloudShadowFactor = 1.;
			if (cloudsEnabled && cloudShadowsEnabled) {
				Ray ray;
				ray.ro = texture(textSamp02, fUVx2.st).xyz;
				if (distance(ray.ro, eyeLocation.xyz) < 800.)
				{
					vec3 lDir = normalize(eyeLocation.xyz - theLights[index].position.xyz);
					ray.rd = -lDir;
					cloudShadowFactor = RayTraceShadows(ray);
				}
			}

			float shadowFactor = 1.;
			if (shadowsEnabled) {
				vec4 glposition = (shadowMVP * vec4(vertexWorldPos, 1.0)) * 0.5 + 0.5;
				if (glposition.z > 1.)
				{
					glposition.z = 1.;
				}
				glposition.xy = clamp(glposition.xy, 0., 1.);
				float bias = 0.0005;
				shadowFactor = shadowFactor * min(texture( shadowTexture, vec3(glposition.xy, glposition.z - bias)) + 0.5, 1.0);
			}

			shadowFactor *= cloudShadowFactor;

			// Specular 
			vec3 lvector = normalize(theLights[index].position.xyz - vertexWorldPos.xyz);
			vec3 specContrib = vec3(0.0f);
			
			vec3 reflectVector = reflect( -lvector, normalize(norm.xyz) );

			// Get eye or view vector
			// The location of the vertex in the world to your eye
			vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

			// To simplify, we are NOT using the light specular value, just the object’s.
			float objectSpecularPower = vertexSpecular.w; 
		
			specContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
								   * theLights[index].specular.rgb;
			
			finalObjectColour.rgb += (vertexMaterialColour.rgb * theLights[index].diffuse.rgb * lightContrib) 
			* theLights[index].atten.r * shadowFactor + vertexSpecular.rgb * specContrib * shadowFactor; 
			continue;
		}
		
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