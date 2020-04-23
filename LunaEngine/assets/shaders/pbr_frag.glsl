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
uniform bool isTomb;

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


// Globals
in float fiTime;
uniform vec2 iResolution;



layout (location = 0) out vec4 pixelColour;			// RGB A   (0 to 1) 
layout (location = 1) out vec4 normalColour;		// Depth (0 to 1)
layout (location = 2) out vec4 positionColour;		// Depth (0 to 1)
layout (location = 3) out vec4 bloomColour;			// Depth (0 to 1)
layout (location = 4) out vec4 unlitColour;			// Depth (0 to 1)

const float height_scale = 0.1;

vec2 GetTextureCoords(vec2 texCoords, vec3 viewDir)
{
	return texCoords;
}

void main()  
{
	

	unlitColour = vec4(0.0);
	positionColour = vec4(fVertWorldLocation.xyz, 1.0);

	vec2 uv = GetTextureCoords(fUVx2.st * tex_tiling.w, normalize(eyeLocation.xyz - fVertWorldLocation.xyz));

	vec3 albedo = texture( textSamp00, uv).rgb;
	vec3 normalMap = texture( textSamp01, uv).rgb;
	vec3 specularMap = 1. - texture( textSamp02, uv).rgb;
		
	mat3 TBN = mat3(
		normalize(fTangent.xyz),
		normalize(fBiTangent.xyz),
		normalize(fNormal.xyz)
	);

	vec3 worldSpaceNormal = (normalMap * 2.) - 1.;
	worldSpaceNormal = TBN * worldSpaceNormal;
	normalColour = vec4(normalize(worldSpaceNormal) + 1.0, 1.0);

	vec3 direction = normalize(fVertWorldLocation.xyz - eyeLocation.xyz);
	vec3 reflectiveColour = texture(skyBox, reflect(direction, fNormal.xyz)).rgb;

	pixelColour = vec4(albedo, 1.0);

	bloomColour.x = specularColour.r * specularMap.r;
	bloomColour.y = min(specularColour.w, 1000.) / 1000.;
	bloomColour.a = 1.0;

	if (isTomb)
	{
		pixelColour.rgb = vec3(1.0, 0.0, 0.0);
		return;
	}
	
} // end main