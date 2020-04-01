#version 420

in vec4 fVertWorldLocation;
uniform vec3 fColour;

layout (location = 0) out vec4 pixelColour;

void main()
{
	pixelColour = vec4(fColour, 1.0);
}