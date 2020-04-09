#version 420

//uniform mat4 MVP;
uniform mat4 matModel;					// Model or World 
uniform mat4 matModelInverTrans;		// Model or World 
uniform mat4 matView; 					// View or camera
uniform mat4 matProj;					// Projection transform
uniform float iTime;
uniform bool isWater;

in vec4 vPosition;				// Was vec3
in vec4 vNormal;				// Vertex Normal
in vec4 vUVx2;					// 2 x Texture coord
in vec4 vTangent;				// For bump mapping
in vec4 vBiNormal;				// For bump mapping
in vec4 vBoneID;			// For skinned mesh (FBX)
in vec4 vBoneWeight;		// For skinned mesh (FBX)

const int MAXNUMBEROFBONES = 100;
uniform mat4 matBonesArray[MAXNUMBEROFBONES];
uniform bool isSkinnedMesh;

out vec4 fVertWorldLocation;
out vec4 fNormal;
out vec4 fTangent;
out vec4 fBiTangent;
out vec4 fUVx2;
out float fiTime;
out float depth;

void main()
{

	if (isSkinnedMesh)
	{
        mat4 BoneTransform = matBonesArray[ int(vBoneID[0]) ] * vBoneWeight[0];
			 BoneTransform += matBonesArray[ int(vBoneID[1]) ] * vBoneWeight[1];
			 BoneTransform += matBonesArray[ int(vBoneID[2]) ] * vBoneWeight[2];
			 BoneTransform += matBonesArray[ int(vBoneID[3]) ] * vBoneWeight[3];

        // Apply the bone transform to the vertex:
		vec4 vertOriginal = vec4(vPosition.xyz, 1.0);

		vec4 vertAfterBoneTransform = BoneTransform * vertOriginal;
				
		mat4 matMVP = matProj * matView * matModel;
		// Transform the updated vertex location (from the bone)
		//  and transform with model view projection matrix (as usual)
		gl_Position = matMVP * vertAfterBoneTransform;
		
		
		// Then we do the normals, etc.
		fVertWorldLocation = matModel * vertAfterBoneTransform;	
		
		// Updated "world" or "model" transform 
		mat4 matModelAndBone = matModel * BoneTransform;
		
		//vec3 theNormal = normalize(vNormal.xyz);
		//fNormal = (matModelInverTrans * BoneTransform) * vec4(theNormal, 1.0);
		//fNormal.xyz = normalize(fNormal.xyz);
//		fNormal = (matModelInverTrans) * vNormal;
//		fNormal.xyz = normalize(fNormal.xyz);
		mat4 inversetranspose = inverse(transpose(matModelAndBone));
		vec3 theNormal = normalize(vNormal.xyz);
		fNormal = inversetranspose * vec4(theNormal, 1.0);
		fNormal.xyz = normalize(fNormal.xyz); 

		fTangent = inversetranspose * vTangent;
		fBiTangent = inversetranspose * vBiNormal;
		
		fUVx2 = vUVx2;

		depth = gl_Position.z;

		return;
	}

	
    vec4 vertPosition = vPosition;

    mat4 matMVP = matProj * matView * matModel;

	gl_Position = matMVP * vec4(vertPosition.xyz, 1.0);
	
	fVertWorldLocation = matModel * vec4(vertPosition.xyz, 1.0);

	//mat4 matInv = inverse(transpose(matModel));

	fNormal = matModelInverTrans * vNormal;
	fTangent = matModelInverTrans * vTangent;
	fBiTangent = matModelInverTrans * vBiNormal;

	fUVx2 = vUVx2;

	fiTime = iTime;

	depth = gl_Position.z;
}
