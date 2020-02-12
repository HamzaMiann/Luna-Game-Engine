#version 420

// Vertex Shader Input
in vec4 fVertWorldLocation;
in vec4 fNormal;
in vec4 fUVx2;
in float fiTime;

// Texture
uniform sampler2D textSamp00;	// albedo
uniform sampler2D textSamp01;	// normal
uniform sampler2D textSamp02;	// position
uniform sampler2D textSamp03;	// bloom
uniform sampler2D textSamp04;	// unlit

// Globals
uniform vec2 iResolution;

// OUTPUT
layout (location = 0) out vec4 pixelColour;			// RGB A   (0 to 1)



vec4 gaussian_blur(sampler2D tex)
{
	const int num_samples = 13;

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


	for (int i = 0; i < num_samples; ++i)
	{
		rgba += texture( tex, fUVx2.st + offsets[i].xy ).rgba * offsets[i].z;
	}

	rgba.a = 1.0;

	return rgba;
}


////
// Computes blur for a given texture and pixel offset
// using a circular blur technique
////
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
		float angle = angle_step * i;
		vec2 uv = fUVx2.xy + vec2(cos(angle), sin(angle)) * pixelSize * offset;
		colour.rgb += texture(tex, uv).rgb * contribution;
	}
	colour.rgb += texture(tex, fUVx2.xy).rgb * (1. - AOE_strength);
	colour.a = 1.0;
	return colour;
}



void main()  
{
	vec2 uv = fUVx2.st;

	pixelColour = circular_blur(textSamp00, 10.0);

	return;
	
} // end main