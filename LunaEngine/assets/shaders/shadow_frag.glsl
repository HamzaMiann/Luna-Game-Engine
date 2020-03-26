#version 420

// Vertex Shader Input
in vec4 fVertWorldLocation;

layout (location = 0) out vec4 pixelColour;			// RGB A   (0 to 1) 


void main()  
{
	
	pixelColour.rgb = vec3(fVertWorldLocation.z);
	pixelColour.a = 1.;

}


