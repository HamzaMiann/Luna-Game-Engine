#version 420

// Vertex Shader Input
in vec4 fVertWorldLocation;
in vec4 fNormal;
in vec4 fTangent;
in vec4 fBiTangent;
in vec4 fUVx2;

// Object
uniform vec4 diffuseColour;
uniform vec4 specularColour;
uniform vec4 eyeLocation;

// Used to draw debug (or unlit) objects
uniform bool isUniform;
uniform bool isSkybox;
uniform bool isPBR;

uniform float reflectivity;	// 0 - 1
uniform float refractivity; // 0 - 1

// Texture
uniform sampler2D textSamp00;
uniform sampler2D textSamp01;
uniform sampler2D textSamp02;
uniform sampler2D textSamp03;
uniform vec4 tex_0_3_ratio;		// x = 0, y = 1, z = 2, w = 3
uniform vec4 tex_tiling;
uniform samplerCube skyBox;

uniform sampler2D blendMap;
uniform bool useBlendMap;
uniform float blendTiling;

uniform bool isShadowMap;

// Globals
in float fiTime;
in float depth;
uniform vec2 iResolution;



layout (location = 0) out vec4 pixelColour;			// RGB A   (0 to 1) 
layout (location = 1) out vec4 normalColour;			// Depth (0 to 1)
layout (location = 2) out vec4 positionColour;			// Depth (0 to 1)
layout (location = 3) out vec4 bloomColour;			// Depth (0 to 1)
layout (location = 4) out vec4 unlitColour;			// Depth (0 to 1)

layout (location = 5) out vec4 depthColour;			// Depth (0 to 1)


float getFogFactor(float d);
float rand(float s, float r) { return mod(mod(s, r + fiTime) * 112341, 1); }

float GetFogValue() { return getFogFactor(distance(eyeLocation, fVertWorldLocation)); }
vec4 GetRandValue() { return vec4(rand(gl_FragCoord.x, fVertWorldLocation.x), rand(gl_FragCoord.y, fVertWorldLocation.y), rand(gl_FragCoord.z, fVertWorldLocation.z), 1); }
vec4 GetSineColValue() { return vec4(sin(gl_FragCoord.x/iResolution.x), sin(gl_FragCoord.y/iResolution.y), cos(gl_FragCoord.z), 1); }
vec4 GetTimeColValue(vec2 uv) { return vec4(0.5 + 0.5*cos(fiTime+uv.xyx+vec3(0,2,4)), 1.0); }

float random (in vec2 st);
float noise (in vec2 st);


void main()  
{
	
	unlitColour = vec4(0.0);
	bloomColour = vec4(0.0);
	depthColour = vec4(vec3(distance(fVertWorldLocation.xyz, eyeLocation.xyz) / 1000.0), 1.0);
	positionColour = vec4(fVertWorldLocation.xyz, 1.0);

	if (isUniform)
	{
		pixelColour.rgb = diffuseColour.rgb;
		pixelColour.a = 1.0;
		unlitColour += 1.0;

		return;
	}

	if ( isSkybox )
	{
		vec3 normal = fNormal.xyz;
		vec3 skyColour = texture( skyBox, normal ).rgb;
		pixelColour.rgb = skyColour.rgb;
		pixelColour.a = 1.0;				// NOT transparent
		
		//pixelColour.rgb *= 1.0;		// Make it a little brighter
		unlitColour += 1.0;

		return;
	}

	if (texture( textSamp00, fUVx2.st * tex_tiling.x ).a < 1.0)
	{
		discard;
	}

	if (isShadowMap)
	{
		pixelColour.rgb = vec3(depth);
		pixelColour.a = 1.0;
		return;
	}

	vec3 tex0_RGB = texture( textSamp00, fUVx2.st * tex_tiling.x ).rgb;
	vec3 tex1_RGB = texture( textSamp01, fUVx2.st * tex_tiling.y ).rgb;
	vec3 tex2_RGB = texture( textSamp02, fUVx2.st * tex_tiling.z ).rgb;
	vec3 tex3_RGB = texture( textSamp03, fUVx2.st * tex_tiling.w ).rgb;
		
	vec3 texRGB =   ( tex_0_3_ratio.x * tex0_RGB )
				  + ( tex_0_3_ratio.y * tex1_RGB )
				  + ( tex_0_3_ratio.z * tex2_RGB )
				  + ( tex_0_3_ratio.w * tex3_RGB );

	vec3 direction = normalize(fVertWorldLocation.xyz - eyeLocation.xyz);

	vec3 reflectiveColour = texture(skyBox, reflect(direction, fNormal.xyz)).rgb;
	vec3 refractiveColour = texture(skyBox, refract(direction, fNormal.xyz, 1.0 / 1.52)).rgb;

	if (useBlendMap) {
		float ratio = smoothstep(0., 1., texture(blendMap, fUVx2.st * blendTiling).r);
		texRGB = (tex_0_3_ratio.x * ratio * tex0_RGB) + (tex_0_3_ratio.y * (1. - ratio) * tex1_RGB).rgb;
	}

	bloomColour.x = specularColour.r;
	bloomColour.y = min(specularColour.w, 1000.0) / 1000.;
	bloomColour.a = 1.0;

	if (isPBR)
	{
		mat3 TBN = mat3(
			normalize(fTangent.xyz),
			normalize(fBiTangent.xyz),
			normalize(fNormal.xyz)
		);
		vec3 worldSpaceNormal = (tex1_RGB * 2.) - 1.;
		worldSpaceNormal = fNormal.xyz;
		worldSpaceNormal = TBN * worldSpaceNormal;
		normalColour = vec4(normalize(worldSpaceNormal) + 1.0, 1.0);

		pixelColour = vec4(tex0_RGB.rgb, diffuseColour.a);
	}
	else
	{
		normalColour = vec4(normalize(fNormal.xyz) + 1.0, 1.0);

		pixelColour = vec4(texRGB, diffuseColour.a);

		pixelColour.rgb = mix(pixelColour.rgb, reflectiveColour.rgb, reflectivity);
		pixelColour.rgb = mix(pixelColour.rgb, refractiveColour.rgb, refractivity);

		pixelColour.a = 1.0;
	}

	return;

	//pixelColour = mix(pixelColour, vec4(0.0,0.0,0.0,0.0), GetFogValue());
	//pixelColour = mix(pixelColour, vec4(0.0,0.0,0.0,0.0), GetRandValue());
	//pixelColour = mix(pixelColour, vec4(0.0,0.0,0.0,0.0), GetSineColValue());
	//pixelColour = mix(pixelColour, vec4(0.0,0.0,0.0,0.0), GetTimeColValue(gl_FragCoord.xy/iResolution.xy));
	//pixelColour = mix(pixelColour, vec4(0.0,0.0,0.0,0.0), noise(fVertWorldLocation.xy));


	
} // end main




float getFogFactor(float d)
{
    const float FogMax = 80.0;
    const float FogMin = 30.0;

    if (d>=FogMax) return 1;
    if (d<=FogMin) return 0;

    return 1 - (FogMax - d) / (FogMax - FogMin);
}

float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}