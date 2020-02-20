#include "cModelLoader.h"

#include <iostream>			// cin cout
#include <fstream>		    // ifstream ofstream
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <Texture/cBasicTextureManager.h>
#include <Animation/cAnimationManager.h>
#include <Mesh/cVAOManager.h>


cModelLoader::cModelLoader()			// constructor
{
	//std::cout << "A cModelLoader() is created!" << std::endl;
	return;
}

cModelLoader::~cModelLoader()			// destructor
{
	//std::cout << "A cModelLoader() is destroyed. How sad." << std::endl;
	return;
}

mat4 AIMatrixToGLMMatrix(aiMatrix4x4& mat)
{
	return mat4(mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3,
		mat.a4, mat.b4, mat.c4, mat.d4);
}

void SetHeirarchy(sHeirarchy& node, cAnimation::sBone& parent, std::vector<cAnimation::sBone*>& bones)
{
	unsigned int n = 0;
	for (; n < bones.size(); ++n)
	{
		if (bones[n]->name == node.name)
		{
			bones[n]->nodeTransform = node.transform;
			bones[n]->boneOffset = node.boneOffset;
			bones[n]->id = node.id;
			parent.children.push_back(bones[n]);
			break;
		}
	}

	for (unsigned int i = 0; i < node.children.size(); ++i)
	{
		auto child = node.children[i];
		SetHeirarchy(*child, *bones[n], bones);
	}
}

void PopulateChildren(sHeirarchy& node, std::vector<sBoneInfo>& bones)
{
	for (unsigned int i = 0; i < bones.size(); ++i)
	{
		if (bones[i].parentBoneName == node.name)
		{
			sHeirarchy* child = new sHeirarchy;
			child->name = bones[i].BoneName;
			child->id = bones[i].id;
			child->transform = bones[i].BoneOffset;
			node.children.push_back(child);
		}
	}

	for (unsigned int i = 0; i < node.children.size(); ++i)
	{
		PopulateChildren(*node.children[i], bones);
	}
}


// Takes the filename to load
// Returns by ref the mesh
bool cModelLoader::LoadModel(
	std::string filename,
	std::string friendlyName,
	cMesh &theMesh)				// Note the "&"
{

	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.

	unsigned int flags = aiProcess_Triangulate |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace;

	if (filename.substr(filename.find_last_of(".") + 1) == "fbx")
	{
		flags |= aiProcess_PopulateArmatureData;
	}

	const aiScene* scene = importer.ReadFile(filename, flags);

	if (!scene)
	{
		std::cout << "Error while loading model... " << importer.GetErrorString() << std::endl;
		return false;
	}

	if (scene->HasMeshes())
	{
		aiMesh& mesh = *scene->mMeshes[0];
		theMesh.name = friendlyName;

		for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
		{
			sMeshVertex tempVertex;

			aiVector3D vec = mesh.mVertices[i];
			tempVertex.x = vec.x;
			tempVertex.y = vec.y;
			tempVertex.z = vec.z;

			aiVector3D normal = mesh.mNormals[i];
			tempVertex.nx = normal.x;
			tempVertex.ny = normal.y;
			tempVertex.nz = normal.z;

			aiVector3D uv = mesh.mTextureCoords[0][i];
			tempVertex.u = uv.x;
			tempVertex.v = uv.y;

			theMesh.vecVertices.push_back(tempVertex);
		}

		for (unsigned int i = 0; i < mesh.mNumFaces; ++i)
		{
			sTriangle tempTriangle;
			sMeshTriangle tempMeshTriangle;

			aiFace face = mesh.mFaces[i];

			tempTriangle.vert_index_1 = face.mIndices[0];
			tempTriangle.vert_index_2 = face.mIndices[1];
			tempTriangle.vert_index_3 = face.mIndices[2];

			sMeshVertex first = theMesh.vecVertices[tempTriangle.vert_index_1];
			sMeshVertex second = theMesh.vecVertices[tempTriangle.vert_index_2];
			sMeshVertex third = theMesh.vecVertices[tempTriangle.vert_index_3];
			tempMeshTriangle.first = vec3(first.x, first.y, first.z);
			tempMeshTriangle.second = vec3(second.x, second.y, second.z);
			tempMeshTriangle.third = vec3(third.x, third.y, third.z);
			tempMeshTriangle.normal.x = (first.nx + second.nx + third.nx) / 3.f;
			tempMeshTriangle.normal.y = (first.ny + second.ny + third.ny) / 3.f;
			tempMeshTriangle.normal.z = (first.nz + second.nz + third.nz) / 3.f;

			tempMeshTriangle.m1 = (tempMeshTriangle.first + tempMeshTriangle.second) / 2.f;
			tempMeshTriangle.m2 = (tempMeshTriangle.first + tempMeshTriangle.third) / 2.f;
			tempMeshTriangle.m3 = (tempMeshTriangle.second + tempMeshTriangle.second) / 2.f;
			tempMeshTriangle.m1 = (tempMeshTriangle.m1 + tempMeshTriangle.m2 + tempMeshTriangle.m3) / 3.f;

			theMesh.vecTriangles.push_back(tempTriangle);

			theMesh.vecMeshTriangles.push_back(tempMeshTriangle);
		}


		if (scene->HasTextures())
		{
			for (unsigned int i = 0; i < scene->mNumTextures; ++i)
			{
				aiTexture& tex = *scene->mTextures[i];
				std::string filename = tex.mFilename.C_Str();
				filename = filename.substr(filename.find_last_of('/') + 1);
				size_t size = sizeof(tex.pcData) / sizeof(aiTexel);
				std::vector<unsigned char> data;
				for (unsigned int n = 0; n < size; ++n)
				{
					data.push_back(tex.pcData[n].r);
					data.push_back(tex.pcData[n].g);
					data.push_back(tex.pcData[n].b);
					data.push_back(tex.pcData[n].a);
				}
				cBasicTextureManager::Instance()->Create2DTexture(filename, true, data, tex.mWidth, tex.mHeight);
				std::cout << "ASSIMP TEXTURE LOADED: " << filename << std::endl;
			}
		}

		std::vector<sBoneInfo> bones;
		std::map<std::string, unsigned int> ids;
		
		sHeirarchy* root = new sHeirarchy;

		if (mesh.HasBones())
		{
			scene->mRootNode->mTransformation;
			theMesh.isAnimated = true;
			theMesh.numBones = mesh.mNumBones;
			for (unsigned int boneIndex = 0; boneIndex < mesh.mNumBones; ++boneIndex)
			{
				aiBone* bone = mesh.mBones[boneIndex];
				std::string name = bone->mName.C_Str();
				unsigned int bidx = 0;

				
				std::map<std::string, unsigned int>::iterator it = ids.find(name);

				if (it == ids.end())
				{
					bidx = boneIndex;
					ids[bone->mName.C_Str()] = bidx;
					sBoneInfo bi;
					bi.BoneName = bone->mName.C_Str();
					bi.id = bidx;
					bi.BoneOffset = AIMatrixToGLMMatrix(bone->mOffsetMatrix);
					bi.BoneTransform = AIMatrixToGLMMatrix(bone->mNode->mTransformation);

					if (std::string(bone->mNode->mParent->mName.C_Str())
						== std::string(scene->mRootNode->mName.C_Str()))
					{
						bi.parentBoneName = "";
						root->name = bi.BoneName;
						root->id = bi.id;
						root->transform = bi.BoneTransform;
						root->boneOffset = bi.BoneOffset;
						root->globalInverse = glm::inverse(AIMatrixToGLMMatrix(scene->mRootNode->mTransformation));
					}
					else
					{
						bi.parentBoneName = bone->mNode->mParent->mName.C_Str();
					}

					bones.push_back(bi);
				}
				else
				{
					bidx = ids[bone->mName.C_Str()];
				}

				for (unsigned int weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
				{
					aiVertexWeight weight = bone->mWeights[weightIndex];
					bool found = false;
					for (unsigned int i = 0; i < NUMBEROFBONES; ++i)
					{
						if (theMesh.vecVertices[weight.mVertexId].boneWeights[i] == 0.f)
						{
							found = true;
							theMesh.vecVertices[weight.mVertexId].boneID[i] = (float)bidx;
							theMesh.vecVertices[weight.mVertexId].boneWeights[i] = weight.mWeight;
							break;
						}
					}
					//theMesh.vecVertices[weight.mVertexId].SetID(bidx, weight.mWeight);
				}
			}

			PopulateChildren(*root, bones);

			theMesh.root = root;
			theMesh.boneIDs = ids;

		}

		if (scene->HasAnimations())
		{
			for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
			{
				aiAnimation* animation = scene->mAnimations[i];

				cAnimation* anim = new cAnimation;
				anim->name = friendlyName + std::to_string(i);
				anim->duration = animation->mDuration;
				anim->ticksPerSecond = animation->mTicksPerSecond;

				anim->numBones = bones.size();
				anim->bones = ids;

				std::vector<cAnimation::sBone*> bones;

				for (unsigned int n = 0; n < animation->mNumChannels; ++n)
				{
					aiNodeAnim* node = animation->mChannels[n];
					cAnimation::sBone* bone = new cAnimation::sBone;
					bone->name = node->mNodeName.C_Str();

					for (unsigned int x = 0; x < node->mNumPositionKeys; ++x)
					{
						vec3 pos;
						quat rot;
						vec3 scale;

						bone->mTimes.push_back(node->mPositionKeys[x].mTime);

						pos.x = node->mPositionKeys[x].mValue.x;
						pos.y = node->mPositionKeys[x].mValue.y;
						pos.z = node->mPositionKeys[x].mValue.z;

						rot.x = node->mRotationKeys[x].mValue.x;
						rot.y = node->mRotationKeys[x].mValue.y;
						rot.z = node->mRotationKeys[x].mValue.z;
						rot.w = node->mRotationKeys[x].mValue.w;

						scale.x = node->mScalingKeys[x].mValue.x;
						scale.y = node->mScalingKeys[x].mValue.y;
						scale.z = node->mScalingKeys[x].mValue.z;

						bone->positions.push_back(pos);
						bone->rotations.push_back(rot);
						bone->scalings.push_back(scale);
					}

					bone->id = ids[bone->name];
					bones.push_back(bone);
				}


				for (unsigned int n = 0; n < bones.size(); ++n)
				{
					if (bones[n]->name == root->name)
					{
						anim->root = *bones[n];
						anim->root.nodeTransform = root->transform;
						anim->root.boneOffset = root->boneOffset;
						anim->GlobalInverse = root->globalInverse;
					}
				}

				for (unsigned int n = 0; n < root->children.size(); ++n)
				{
					auto node = root->children[n];
					SetHeirarchy(*node, anim->root, bones);
				}

				cAnimationManager::Instance().animations[anim->name] = anim;
				std::cout << "Mesh with animation loaded: " << anim->name << std::endl;
			}

		}

		return true;

	}

	return false;






	// OLD LOADER
	//----------------------------------------


	//std::string AABB_file = "assets/models/Terrain_XYZ_n_uv.ply";
	////std::string AABB_file = "assets/models/Halo_Ring2_XYZ_N_UV.ply";

	//std::ifstream theFile( filename.c_str() );
	//if ( ! theFile.is_open() )
	//{	
	//	// On no! Where's the file??? 
	//	return false;
	//}

	//// Scan the file until I get to "vertex", and stop
	//std::string temp;
	////theFile >> temp;
	//while ( theFile >> temp ) 
	//{
	//	// Have it hit the word "vertex"?
	//	if ( temp == "vertex" )
	//	{
	//		break;		// Exit the while loop
	//	}
	//}// while ( theFile >> temp ) 
	//
	//unsigned int numberOfVertices;
	//theFile >> numberOfVertices;



	//while (theFile >> temp)
	//{
	//	// Have it hit the word "face"?
	//	if (temp == "face")
	//	{
	//		break;		// Exit the while loop
	//	}
	//}// while ( theFile >> temp ) 

	//unsigned int numberOfTriangles;
	//theFile >> numberOfTriangles;

	//while (theFile >> temp)
	//{
	//	// Have it hit the word "end_header"?
	//	if (temp == "end_header")
	//	{
	//		break;		// Exit the while loop
	//	}
	//}// while ( theFile >> temp ) 



	//// Read all the vertices
	//for (unsigned int index = 0; index != numberOfVertices; index++)
	//{
	//	// -0.0312216 0.126304 0.00514924
	//	sPlyVertexXYZ tempVertex;

	//	theFile >> tempVertex.x >> tempVertex.y >> tempVertex.z;
	//	theFile >> tempVertex.nx >> tempVertex.ny >> tempVertex.nz;
	//	theFile >> tempVertex.u >> tempVertex.v;

	//	if (filename == AABB_file)
	//	{
	//		if (tempVertex.x < theMesh.min.x) theMesh.min.x = tempVertex.x;
	//		if (tempVertex.y < theMesh.min.y) theMesh.min.y = tempVertex.y;
	//		if (tempVertex.z < theMesh.min.z) theMesh.min.z = tempVertex.z;

	//		if (tempVertex.x > theMesh.max.x) theMesh.max.x = tempVertex.x;
	//		if (tempVertex.y > theMesh.max.y) theMesh.max.y = tempVertex.y;
	//		if (tempVertex.z > theMesh.max.z) theMesh.max.z = tempVertex.z;
	//	}

	//	// Add this temp vertex to the vector of vertices
	//	// (cMesh &theMesh)
	//	theMesh.vecVertices.push_back( tempVertex );
	//}

	//for (unsigned int index = 0; index != numberOfTriangles; index++)
	//{
	//	// 3 166 210 265 
	//	int discardThis;
	//	sPlyTriangle tempTriangle;

	//	theFile >> discardThis 
	//		>> tempTriangle.vert_index_1 
	//		>> tempTriangle.vert_index_2
	//		>> tempTriangle.vert_index_3;

	//	// Add this triangle
	//	theMesh.vecTriangles.push_back(tempTriangle);

	//	// calculate and cache mesh triangle
	//	sMeshTriangle tempMeshTriangle;
	//	sPlyVertexXYZ first = theMesh.vecVertices[tempTriangle.vert_index_1];
	//	sPlyVertexXYZ second = theMesh.vecVertices[tempTriangle.vert_index_2];
	//	sPlyVertexXYZ third = theMesh.vecVertices[tempTriangle.vert_index_3];
	//	tempMeshTriangle.first = vec3(first.x, first.y, first.z);
	//	tempMeshTriangle.second = vec3(second.x, second.y, second.z);
	//	tempMeshTriangle.third = vec3(third.x, third.y, third.z);
	//	tempMeshTriangle.normal.x = (first.nx + second.nx + third.nx) / 3.f;
	//	tempMeshTriangle.normal.y = (first.ny + second.ny + third.ny) / 3.f;
	//	tempMeshTriangle.normal.z = (first.nz + second.nz + third.nz) / 3.f;

	//	tempMeshTriangle.m1 = (tempMeshTriangle.first + tempMeshTriangle.second) / 2.f;
	//	tempMeshTriangle.m2 = (tempMeshTriangle.first + tempMeshTriangle.third) / 2.f;
	//	tempMeshTriangle.m3 = (tempMeshTriangle.second + tempMeshTriangle.second) / 2.f;
	//	tempMeshTriangle.m1 = (tempMeshTriangle.m1 + tempMeshTriangle.m2 + tempMeshTriangle.m3) / 3.f;

	//	theMesh.vecMeshTriangles.push_back(tempMeshTriangle);
	//}

	//if (filename == AABB_file)
	//{
	//	if (theMesh.min.x < min.x) min.x = theMesh.min.x;
	//	if (theMesh.min.y < min.y) min.y = theMesh.min.y;
	//	if (theMesh.min.z < min.z) min.z = theMesh.min.z;

	//	if (theMesh.max.x > max.x) max.x = theMesh.max.x;
	//	if (theMesh.max.y > max.y) max.y = theMesh.max.y;
	//	if (theMesh.max.z > max.z) max.z = theMesh.max.z;
	//}

	//return true;
}

bool cModelLoader::LoadAnimation(std::string filename, std::string friendlyName, std::string meshName)
{
	cMesh* theMesh = cVAOManager::Instance().FindMeshByModelName(meshName);
	if (!theMesh) return false;

	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.

	unsigned int flags = aiProcess_Triangulate |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_PopulateArmatureData;

	const aiScene* scene = importer.ReadFile(filename, flags);

	if (!scene)
	{
		std::cout << "Error while loading model... " << importer.GetErrorString() << std::endl;
		return false;
	}

	std::map<std::string, unsigned int> ids = theMesh->boneIDs;
	sHeirarchy* root = theMesh->root;

	if (scene->HasAnimations())
	{
		aiAnimation* animation = scene->mAnimations[0];

		cAnimation* anim = new cAnimation;
		anim->name = friendlyName;
		anim->duration = animation->mDuration;
		anim->ticksPerSecond = animation->mTicksPerSecond;

		anim->numBones = theMesh->numBones;
		anim->bones = ids;

		std::vector<cAnimation::sBone*> bones;

		for (unsigned int n = 0; n < animation->mNumChannels; ++n)
		{
			aiNodeAnim* node = animation->mChannels[n];
			cAnimation::sBone* bone = new cAnimation::sBone;
			bone->name = node->mNodeName.C_Str();

			for (unsigned int x = 0; x < node->mNumPositionKeys; ++x)
			{
				vec3 pos;
				quat rot;
				vec3 scale;

				bone->mTimes.push_back(node->mPositionKeys[x].mTime);

				pos.x = node->mPositionKeys[x].mValue.x;
				pos.y = node->mPositionKeys[x].mValue.y;
				pos.z = node->mPositionKeys[x].mValue.z;

				rot.x = node->mRotationKeys[x].mValue.x;
				rot.y = node->mRotationKeys[x].mValue.y;
				rot.z = node->mRotationKeys[x].mValue.z;
				rot.w = node->mRotationKeys[x].mValue.w;

				scale.x = node->mScalingKeys[x].mValue.x;
				scale.y = node->mScalingKeys[x].mValue.y;
				scale.z = node->mScalingKeys[x].mValue.z;

				bone->positions.push_back(pos);
				bone->rotations.push_back(rot);
				bone->scalings.push_back(scale);
			}

			bone->id = ids[bone->name];
			bones.push_back(bone);
		}


		for (unsigned int n = 0; n < bones.size(); ++n)
		{
			if (bones[n]->name == root->name)
			{
				anim->root = *bones[n];
				anim->root.nodeTransform = root->transform;
				anim->root.boneOffset = root->boneOffset;
				anim->GlobalInverse = root->globalInverse;
			}
		}

		for (unsigned int n = 0; n < root->children.size(); ++n)
		{
			auto node = root->children[n];
			SetHeirarchy(*node, anim->root, bones);
		}

		cAnimationManager::Instance().animations[anim->name] = anim;

	}


	return true;
}
