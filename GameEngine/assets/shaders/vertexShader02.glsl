#version 420

//uniform mat4 MVP;
uniform mat4 matModel;					// Model or World 
uniform mat4 matModelInverTrans;		// Model or World 
uniform mat4 matView; 					// View or camera
uniform mat4 matProj;					// Projection transform
uniform float iTime;

in vec4 vColour;				// Was vec3
in vec4 vPosition;				// Was vec3
in vec4 vNormal;				// Vertex Normal
in vec4 vUVx2;					// 2 x Texture coord

//out vec3 color;
//out vec4 vertWorld;			// Location of the vertex in the world
out vec4 fColour;	
out vec4 fVertWorldLocation;
out vec4 fNormal;
out vec4 fUVx2;
out float fiTime;

float rand(float s, float r) { return mod(mod(s, r + iTime) * 112341, 1); }

void main()
{
    vec4 vertPosition = vPosition;
	
    mat4 matMVP = matProj * matView * matModel;
	
	gl_Position = matMVP * vec4(vertPosition.xyz, 1.0);
	gl_Position.y += 0.01 / sin(iTime * 50.f);
	
	// Vertex location in "world space"
	fVertWorldLocation = matModel * vec4(vertPosition.xyz, 1.0);
	
	//mat4 matInv = inverse(transpose(matModel));

    fColour = vColour;
	fNormal = matModelInverTrans * vNormal;

	fUVx2 = vUVx2;

	fiTime = iTime;
}
