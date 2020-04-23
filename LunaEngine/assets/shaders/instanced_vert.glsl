#version 420

in layout (location = 0) vec4 vPosition;				// Was vec3
in layout (location = 1) vec4 vNormal;				// Vertex Normal
in layout (location = 2) vec4 vUVx2;					// 2 x Texture coord
in layout (location = 3) vec4 vTangent;				// For bump mapping
in layout (location = 4) vec4 vBiNormal;				// For bump mapping
in layout (location = 5) vec4 vBoneID;			// For skinned mesh (FBX)
in layout (location = 6) vec4 vBoneWeight;		// For skinned mesh (FBX)
in layout (location = 7) mat4 vMatModel;

uniform mat4 matView; 					// View or camera
uniform mat4 matProj;					// Projection transform
uniform float iTime;

out vec4 fVertWorldLocation;
out vec4 fNormal;
out vec4 fTangent;
out vec4 fBiTangent;
out vec4 fUVx2;
out float fiTime;
out float depth;

void main()
{
    mat4 model = vMatModel;
    vec4 vertPosition = vPosition;

    mat4 matMVP = matProj * matView * model;

	gl_Position = matMVP * vec4(vertPosition.xyz, 1.0);
	
	fVertWorldLocation = model * vec4(vertPosition.xyz, 1.0);

    // Reset scale for normal inverse transpose rotation
    model[0].x = 1.;
    model[1].y = 1.;
    model[2].z = 1.;
    model[3].w = 1.;

	mat4 matInv = inverse(transpose(model));

	fNormal = matInv * vNormal;
	fTangent = matInv * vTangent;
	fBiTangent = matInv * vBiNormal;

	fUVx2 = vUVx2;

	fiTime = iTime;

	depth = gl_Position.z;
}