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
in float fisWater;
uniform vec2 iResolution;



layout (location = 0) out vec4 pixelColour;			// RGB A   (0 to 1) 
layout (location = 1) out vec4 normalColour;		// Depth (0 to 1)
layout (location = 2) out vec4 positionColour;		// Depth (0 to 1)
layout (location = 3) out vec4 bloomColour;			// Depth (0 to 1)
layout (location = 4) out vec4 unlitColour;			// Depth (0 to 1)

layout (location = 5) out vec4 depthColour;			// Depth (0 to 1)


void main()  
{
	unlitColour = vec4(0.0);
	bloomColour = vec4(0.0);
	positionColour = vec4(fVertWorldLocation.xyz, 1.0);

	vec3 tex0_RGB = texture( textSamp00, fUVx2.st * tex_tiling.x ).rgb;
	vec3 tex1_RGB = texture( textSamp01, fUVx2.st * tex_tiling.y ).rgb;
	vec3 tex2_RGB = texture( textSamp02, fUVx2.st * tex_tiling.z ).rgb;
	vec3 tex3_RGB = texture( textSamp03, fUVx2.st * tex_tiling.w ).rgb;
		
	
	vec3 albedo = tex0_RGB;
	vec3 tangent_normal = (tex1_RGB * 2.0) - 1.0;				// normal texture value
	vec3 bitangent_normal = cross(tangent_normal, fNormal.xyz);
	float specular = tex2_RGB.r / 10.0;

	vec3 worldSpaceNormal = vec3(tangent_normal.r * tangent_normal.xyz + tangent_normal.b * fNormal.xyz + tangent_normal.g * bitangent_normal.xyz);
	//worldSpaceNormal = fNormal.xyz;
	normalColour = vec4(normalize(worldSpaceNormal), 1.0);

	vec3 direction = normalize(fVertWorldLocation.xyz - eyeLocation.xyz);
	vec3 reflectiveColour = texture(skyBox, reflect(direction, fNormal.xyz)).rgb;

	pixelColour = vec4(albedo, 1.0);
	pixelColour.rgb = mix(pixelColour.rgb, reflectiveColour.rgb, specular);

	return;
	
} // end main