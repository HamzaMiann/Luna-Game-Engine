#include "cModelLoader.h"

#include <iostream>			// cin cout
#include <fstream>		    // ifstream ofstream
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <Texture/cBasicTextureManager.h>

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

// Takes the filename to load
// Returns by ref the mesh
bool cModelLoader::LoadPlyModel(
	std::string filename,
	std::string friendlyName,
	cMesh &theMesh)				// Note the "&"
{

	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(filename,
											 aiProcess_Triangulate |
											 aiProcess_OptimizeMeshes |
											 aiProcess_OptimizeGraph |
											 aiProcess_JoinIdenticalVertices |
											 aiProcess_GenNormals |
											 aiProcess_CalcTangentSpace );

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
			tempMeshTriangle.first = glm::vec3(first.x, first.y, first.z);
			tempMeshTriangle.second = glm::vec3(second.x, second.y, second.z);
			tempMeshTriangle.third = glm::vec3(third.x, third.y, third.z);
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

		if (mesh.HasBones())
		{
			scene->mRootNode->mTransformation;
			theMesh.isAnimated = true;
			for (unsigned int i = 0; i < mesh.mNumBones; ++i)
			{
				aiBone* bone = mesh.mBones[i];
				for (unsigned int n = 0; n < bone->mNumWeights && n < 4u; ++n)
				{
					aiVertexWeight weight = bone->mWeights[n];
					theMesh.vecVertices[weight.mVertexId].SetID(n, weight.mWeight);
					//theMesh.vecVertices[weight.mVertexId].boneID[n] = i;
					//theMesh.vecVertices[weight.mVertexId].boneWeights[n] = weight.mWeight;
				}
			}

		}

		if (scene->HasAnimations())
		{
			for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
			{
				theMesh.animations.push_back(cAnimation());
				cAnimation& anim = theMesh.animations[theMesh.animations.size() - 1];
				aiAnimation* animation = scene->mAnimations[i];
				anim.name = animation->mName.C_Str();
				anim.duration = animation->mDuration;
				anim.ticksPerSecond = animation->mTicksPerSecond;
				for (unsigned int n = 0; n < animation->mNumChannels; ++n)
				{
					aiNodeAnim* node = animation->mChannels[n];
					anim.channels.push_back(cAnimation::sAnimationNode());
					auto& animnode = anim.channels[anim.channels.size() - 1];
					animnode.name = node->mNodeName.C_Str();
					for (unsigned int x = 0; x < node->mNumPositionKeys; ++x)
					{
						glm::vec3 pos;
						glm::quat rot;
						glm::vec3 scale;

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

						glm::mat4 mat(1.f);
						mat *= glm::translate(mat, pos);
						mat *= glm::mat4(rot);

						animnode.rotatedPositions.push_back(mat);
						animnode.scalings.push_back(scale);
					}
				}
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
	//	tempMeshTriangle.first = glm::vec3(first.x, first.y, first.z);
	//	tempMeshTriangle.second = glm::vec3(second.x, second.y, second.z);
	//	tempMeshTriangle.third = glm::vec3(third.x, third.y, third.z);
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
