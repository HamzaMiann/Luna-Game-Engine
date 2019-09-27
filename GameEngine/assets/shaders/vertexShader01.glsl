#version 420

//uniform mat4 MVP;
uniform mat4 matModel;		// Model or World 
uniform mat4 matView; 		// View or camera
uniform mat4 matProj;		// Projection transform

in vec3 vColour;
in vec3 vPosition;
in vec4 vNormal;

out vec3 color;
out vec4 vertWorld;			// Location of the vertex in the world
out vec4 vVertNormal;

void main()
{
    vec3 vertPosition = vPosition;
	
    mat4 matMVP = matProj * matView * matModel;
	
	gl_Position = matMVP * vec4(vertPosition, 1.0);
	
	// Vertex location in "world space"
	vertWorld = matModel * vec4(vertPosition, 1.0);		
	
    color = vColour;
	vVertNormal = vNormal;
}
