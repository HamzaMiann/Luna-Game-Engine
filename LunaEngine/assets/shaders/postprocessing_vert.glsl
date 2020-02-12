#version 420

//uniform mat4 MVP;
uniform mat4 matModel;					// Model or World 
uniform mat4 matModelInverTrans;		// Model or World 
uniform mat4 matView; 					// View or camera
uniform mat4 matProj;					// Projection transform
uniform float iTime;

in vec4 vPosition;				// Was vec3
in vec4 vNormal;				// Vertex Normal
in vec4 vUVx2;					// 2 x Texture coord
in vec4 vTangent;				// For bump mapping
in vec4 vBiNormal;				// For bump mapping
in vec4 vBoneID;				// For skinned mesh (FBX)
in vec4 vBoneWeight;			// For skinned mesh (FBX)

//out vec3 color;
//out vec4 vertWorld;			// Location of the vertex in the world
out vec4 fVertWorldLocation;
out vec4 fNormal;
out vec4 fUVx2;
out float fiTime;

void main()
{
	gl_Position = (matProj * matModel) * vPosition;
    vec4 vertPosition = vPosition;
	
	// Vertex location in "world space"

	fVertWorldLocation = matModel * vec4(vertPosition.xyz, 1.0);

	//mat4 matInv = inverse(transpose(matModel));

	fNormal = matModelInverTrans * vNormal;
	fUVx2 = vUVx2;
	fiTime = iTime;

}