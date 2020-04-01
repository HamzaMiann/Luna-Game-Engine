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
out vec4 fUVx2;
out float fiTime;
out float fisWater;
out float depth;

float rand(float s, float r) { return mod(mod(s, r + iTime) * 112341, 1); }
float random (in vec2 st);
float noise (in vec2 st);

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

		vec3 theNormal = normalize(vNormal.xyz);
		fNormal = inverse(transpose(matModelAndBone)) * vec4(theNormal, 1.0);
		fNormal.xyz = normalize(fNormal.xyz); 
		
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

	fUVx2 = vUVx2;

	fiTime = iTime;

	depth = gl_Position.z;
}



float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}