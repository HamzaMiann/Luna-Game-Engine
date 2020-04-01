#version 420

uniform mat4 matModel;					// Model or World 
uniform mat4 matView; 					// View or camera
uniform mat4 matProj;					// Projection transform

in vec4 vPosition;				// Was vec3
in vec4 vNormal;				// Vertex Normal
in vec4 vUVx2;					// 2 x Texture coord
in vec4 vTangent;				// For bump mapping
in vec4 vBiNormal;				// For bump mapping
in vec4 vBoneID;			// For skinned mesh (FBX)
in vec4 vBoneWeight;		// For skinned mesh (FBX)

out vec4 fVertWorldLocation;

void main()
{
    vec4 vertPosition = vPosition;

    mat4 matMVP = matProj * matView * matModel;

	gl_Position = matMVP * vec4(vertPosition.xyz, 1.0);
	
	fVertWorldLocation = matModel * vec4(vertPosition.xyz, 1.0);
}


