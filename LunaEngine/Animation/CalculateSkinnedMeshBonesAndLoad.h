//****************************************************************************************
//    ___  _    _                      _  __  __           _     
//   / __|| |__(_) _ _   _ _   ___  __| ||  \/  | ___  ___| |_   
//   \__ \| / /| || ' \ | ' \ / -_)/ _` || |\/| |/ -_)(_-<| ' \  
//   |___/|_\_\|_||_||_||_||_|\___|\__,_||_|  |_|\___|/__/|_||_| 
//                           

#include <Mesh/cVAOManager.h>
#include <cGameObject.h>

void CalculateSkinnedMeshBonesAndLoad( sMeshDrawInfo &theMesh, cGameObject* pTheGO,
									   unsigned int UniformLoc_numBonesUsed, 
									   unsigned int UniformLoc_bonesArray )
{

	std::string animationToPlay = "";
	float curFrameTime = 0.0;


	// See what animation should be playing... 
	cAnimationState* pAniState = pTheGO->pAniState;

	// Are there any animations in the queue of animations
	if ( ! pAniState->vecAnimationQueue.empty() )
	{
		// Play the "1st" animation in the queue 
		animationToPlay = pAniState->vecAnimationQueue[0].name;
		curFrameTime = pAniState->vecAnimationQueue[0].currentTime;

		// Increment the top animation in the queue
		if ( pAniState->vecAnimationQueue[0].IncrementTime() )
		{
			// The animation reset to zero on increment...
			// ...meaning that the 1st animation is done
			// (WHAT!? Should you use a vector for this???)
			pAniState->vecAnimationQueue.erase( pAniState->vecAnimationQueue.begin() );

		}//vecAnimationQueue[0].IncrementTime()
	}
	else
	{	// Use the default animation.
		pAniState->defaultAnimation.IncrementTime();

		animationToPlay = pAniState->defaultAnimation.name;
		curFrameTime = pAniState->defaultAnimation.currentTime;

	}//if ( pAniState->vecAnimationQueue.empty()




//	::g_frameTime += ( ::g_frameTimeIncrement );
//	
//	// Is frame time LT zero? 
//	if ( ::g_frameTime <= 0.0f )
//	{
//		::g_frameTime = pTheGO->pSimpleSkinnedMesh->GetDuration();
//	}
//	// Is frame time GT the animation frame time?
//	if ( ::g_frameTime > pTheGO->pSimpleSkinnedMesh->GetDuration() )
//	{
//		::g_frameTime = 0.0f; 
//	}
//

	// Set up the animation pose:
	std::vector< glm::mat4x4 > vecFinalTransformation;	// Replaced by	theMesh.vecFinalTransformation
//	std::vector< glm::mat4x4 > vecObjectBoneTransformation;
	std::vector< glm::mat4x4 > vecOffsets;
	// Final transformation is the bone transformation + boneOffsetPerVertex
	// ObjectBoneTransformation (or "Global") is the final location of the bones
	// vecOffsets is the relative offsets of the bones from each other


	pTheGO->pSimpleSkinnedMesh->BoneTransform( 
	                                curFrameTime,
									animationToPlay,		//**NEW**
		                            vecFinalTransformation,		// Final bone transforms for mesh
									theMesh.vecObjectBoneTransformation,  // final location of bones
									vecOffsets );                 // local offset for each bone

	unsigned int numberOfBonesUsed = static_cast< unsigned int >( vecFinalTransformation.size() );
	glUniform1i( UniformLoc_numBonesUsed, numberOfBonesUsed );

	glm::mat4x4* pBoneMatrixArray = &(vecFinalTransformation[0]);

	// UniformLoc_bonesArray is the getUniformLoc of "bones[0]" from
	//	uniform mat4 bones[MAXNUMBEROFBONES] 
	// in the shader
	glUniformMatrix4fv( UniformLoc_bonesArray, numberOfBonesUsed, GL_FALSE, 
	                    (const GLfloat*) glm::value_ptr( *pBoneMatrixArray ) );


	// Update the extents of the skinned mesh from the bones...
	//	sMeshDrawInfo.minXYZ_from_SM_Bones(glm::vec3(0.0f)), 
	//  sMeshDrawInfo.maxXYZ_from_SM_Bones(glm::vec3(0.0f))
	for ( unsigned int boneIndex = 0; boneIndex != numberOfBonesUsed; boneIndex++ )
	{
		glm::mat4 boneLocal = theMesh.vecObjectBoneTransformation[boneIndex];

		float scale = theMesh.scale;
		boneLocal = glm::scale( boneLocal, glm::vec3(scale, scale, scale) );

		//cPhysicalProperties phyProps;
		//pTheGO->GetPhysState( phyProps );
		//glm::vec4 GameObjectLocalOriginLocation = glm::vec4( phyProps.position, 1.0f );

		//glm::vec4 boneBallLocation = boneLocal * GameObjectLocalOriginLocation;
		glm::vec4 boneBallLocation = boneLocal * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f );
		boneBallLocation *= scale;

		// Update the extents of the mesh
		if ( boneIndex == 0 )
		{	
			// For the 0th bone, just assume this is the extent
			theMesh.minXYZ_from_SM_Bones = glm::vec3(boneBallLocation);
			theMesh.maxXYZ_from_SM_Bones = glm::vec3(boneBallLocation);
		}
		else
		{	// It's NOT the 0th bone, so compare with current max and min
			if ( theMesh.minXYZ_from_SM_Bones.x < boneBallLocation.x ) { theMesh.minXYZ_from_SM_Bones.x = boneBallLocation.x; }
			if ( theMesh.minXYZ_from_SM_Bones.y < boneBallLocation.y ) { theMesh.minXYZ_from_SM_Bones.y = boneBallLocation.y; }
			if ( theMesh.minXYZ_from_SM_Bones.z < boneBallLocation.z ) { theMesh.minXYZ_from_SM_Bones.z = boneBallLocation.z; }

			if ( theMesh.maxXYZ_from_SM_Bones.x > boneBallLocation.x ) { theMesh.maxXYZ_from_SM_Bones.x = boneBallLocation.x; }
			if ( theMesh.maxXYZ_from_SM_Bones.y > boneBallLocation.y ) 
			{ 
				theMesh.maxXYZ_from_SM_Bones.y = boneBallLocation.y;
			}
			if ( theMesh.maxXYZ_from_SM_Bones.z > boneBallLocation.z ) 
			{ 
				theMesh.maxXYZ_from_SM_Bones.z = boneBallLocation.z;
			}
		}//if ( boneIndex == 0 )


		//boneBallLocation += glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			
		//DrawDebugBall( glm::vec3(boneBallLocation), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.2f );

//		if ( boneIndex == 35 )
//		{
//			DrawDebugBall( glm::vec3(boneBallLocation), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.5f );
//		}
	}


	//****************************************************************************************
	return;
}