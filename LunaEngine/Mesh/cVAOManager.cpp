#include "cVAOManager.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

#include <sstream>

sModelDrawInfo::sModelDrawInfo()
{
	this->VAO_ID = 0;

	this->VertexBufferID = 0;
	this->VertexBuffer_Start_Index = 0;
	this->numberOfVertices = 0;

	this->IndexBufferID = 0;
	this->IndexBuffer_Start_Index = 0;
	this->numberOfIndices = 0;
	this->numberOfTriangles = 0;

	// The "local" (i.e. "CPU side" temporary array)
	this->pVertices = 0;	// or NULL
	this->pIndices = 0;		// or NULL

	// You could store the max and min values of the 
	//  vertices here (determined when you load them):
	glm::vec3 maxValues;
	glm::vec3 minValues;

//	scale = 5.0/maxExtent;		-> 5x5x5
	float maxExtent;

	return;
}


bool cVAOManager::LoadModelIntoVAO(
		std::string fileName, 
		cMesh& theMesh,					// NEW
		sModelDrawInfo &drawInfo,
	    unsigned int shaderProgramID)

{
	// Write some code to copy the infomation from cMesh& theMesh
	//  to the sModelDrawInfo& drawInfo...

	drawInfo.mesh = &theMesh;
	drawInfo.numberOfVertices = theMesh.vecVertices.size();
	// Allocate an array big enough
	drawInfo.pVertices = new sVAOVertex[drawInfo.numberOfVertices];

	// Copy the data from the vecVertices...
	for (unsigned int index = 0; index != drawInfo.numberOfVertices; index++)
	{
		drawInfo.pVertices[index].x = theMesh.vecVertices[index].x;
		drawInfo.pVertices[index].y = theMesh.vecVertices[index].y;
		drawInfo.pVertices[index].z = theMesh.vecVertices[index].z;
		drawInfo.pVertices[index].w = 1.0f;

		drawInfo.pVertices[index].nx = theMesh.vecVertices[index].nx;
		drawInfo.pVertices[index].ny = theMesh.vecVertices[index].ny;
		drawInfo.pVertices[index].nz = theMesh.vecVertices[index].nz;
		drawInfo.pVertices[index].nw = 1.0f;

		// Texture coordinates
		drawInfo.pVertices[index].u0 = theMesh.vecVertices[index].u;
		drawInfo.pVertices[index].v0 = theMesh.vecVertices[index].v;
		drawInfo.pVertices[index].u1 = 1.0f;
		drawInfo.pVertices[index].v1 = 1.0f;

		drawInfo.pVertices[index].tx = theMesh.vecVertices[index].tx;
		drawInfo.pVertices[index].ty = theMesh.vecVertices[index].ty;
		drawInfo.pVertices[index].tz = theMesh.vecVertices[index].tz;
		drawInfo.pVertices[index].tw = theMesh.vecVertices[index].tw;

		drawInfo.pVertices[index].bx = theMesh.vecVertices[index].bx;
		drawInfo.pVertices[index].by = theMesh.vecVertices[index].by;
		drawInfo.pVertices[index].bz = theMesh.vecVertices[index].bz;
		drawInfo.pVertices[index].bw = theMesh.vecVertices[index].bw;

		drawInfo.pVertices[index].boneID[0] = theMesh.vecVertices[index].boneID[0];
		drawInfo.pVertices[index].boneID[1] = theMesh.vecVertices[index].boneID[1];
		drawInfo.pVertices[index].boneID[2] = theMesh.vecVertices[index].boneID[2];
		drawInfo.pVertices[index].boneID[3] = theMesh.vecVertices[index].boneID[3];

		drawInfo.pVertices[index].weights[0] = theMesh.vecVertices[index].boneWeights[0];
		drawInfo.pVertices[index].weights[1] = theMesh.vecVertices[index].boneWeights[1];
		drawInfo.pVertices[index].weights[2] = theMesh.vecVertices[index].boneWeights[2];
		drawInfo.pVertices[index].weights[3] = theMesh.vecVertices[index].boneWeights[3];
	}

	// Now copy the index information, too
	drawInfo.numberOfTriangles = theMesh.vecTriangles.size();
	drawInfo.numberOfIndices = theMesh.vecTriangles.size() * 3;

	// Allocate the index array
	drawInfo.pIndices = new unsigned int[drawInfo.numberOfIndices];

	unsigned int indexTri = 0;
	unsigned int indexIndex = 0;
	for ( ; indexTri != drawInfo.numberOfTriangles; indexTri++, indexIndex += 3 )
	{
		drawInfo.pIndices[indexIndex + 0] = theMesh.vecTriangles[indexTri].vert_index_1;
		drawInfo.pIndices[indexIndex + 1] = theMesh.vecTriangles[indexTri].vert_index_2;
		drawInfo.pIndices[indexIndex + 2] = theMesh.vecTriangles[indexTri].vert_index_3;
	}


	drawInfo.meshName = fileName;

	// TODO: Load the model from file

	// 
	// Model is loaded and the vertices and indices are in the drawInfo struct
	// 

	// Create a VAO (Vertex Array Object), which will 
	//	keep track of all the 'state' needed to draw 
	//	from this buffer...

	// Ask OpenGL for a new buffer ID...
	glGenVertexArrays( 1, &(drawInfo.VAO_ID) );
	// "Bind" this buffer:
	// - aka "make this the 'current' VAO buffer
	glBindVertexArray(drawInfo.VAO_ID);

	// Now ANY state that is related to vertex or index buffer
	//	and vertex attribute layout, is stored in the 'state' 
	//	of the VAO... 


	// NOTE: OpenGL error checks have been omitted for brevity
//	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &(drawInfo.VertexBufferID) );

//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
	// sVert vertices[3]
	glBufferData( GL_ARRAY_BUFFER, 
				  sizeof(sVAOVertex) * drawInfo.numberOfVertices,	// ::g_NumberOfVertsToDraw,	// sizeof(vertices), 
				  (GLvoid*) drawInfo.pVertices,							// pVertices,			//vertices, 
				  GL_STATIC_DRAW );


	// Copy the index buffer into the video card, too
	// Create an index buffer.
	glGenBuffers( 1, &(drawInfo.IndexBufferID) );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);

	glBufferData( GL_ELEMENT_ARRAY_BUFFER,			// Type: Index element array
	              sizeof( unsigned int ) * drawInfo.numberOfIndices, 
	              (GLvoid*) drawInfo.pIndices,
                  GL_STATIC_DRAW );

	// Set the vertex attributes.

	GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPosition");	// program
	GLint vnorm_location = glGetAttribLocation(shaderProgramID, "vNormal");	// program;
	GLint vuv_location = glGetAttribLocation(shaderProgramID, "vUVx2");	// program;
	GLint vtan_location = glGetAttribLocation(shaderProgramID, "vTangent");	// program;
	GLint vbi_location = glGetAttribLocation(shaderProgramID, "vBiNormal");	// program;
	GLint vbone_location = glGetAttribLocation(shaderProgramID, "vBoneID");	// program;
	GLint vweight_location = glGetAttribLocation(shaderProgramID, "vBoneWeight");	// program;

	// Set the vertex attributes for this shader
	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer( vpos_location, 4,
						   GL_FLOAT, GL_FALSE,
						   sizeof(sVAOVertex), 
						   ( void* )offsetof(sVAOVertex,x));

	glEnableVertexAttribArray(vnorm_location);
	glVertexAttribPointer(vnorm_location, 4,
						  GL_FLOAT, GL_FALSE,
						  sizeof(sVAOVertex),
						  (void*)offsetof(sVAOVertex, nx));

	glEnableVertexAttribArray(vuv_location);
	glVertexAttribPointer(vuv_location, 4,
						  GL_FLOAT, GL_FALSE,
						  sizeof(sVAOVertex),
						  (void*)offsetof(sVAOVertex, u0));

	glEnableVertexAttribArray(vtan_location);
	glVertexAttribPointer( vtan_location, 4,
						   GL_FLOAT, GL_FALSE,
						   sizeof(sVAOVertex), 
						   ( void* )offsetof(sVAOVertex,tx));

	glEnableVertexAttribArray(vbi_location);
	glVertexAttribPointer(vbi_location, 4,
						   GL_FLOAT, GL_FALSE,
						   sizeof(sVAOVertex), 
						   ( void* )offsetof(sVAOVertex,bx));

	glEnableVertexAttribArray(vbone_location);
	glVertexAttribPointer(vbone_location, (int)NUMBEROFBONES,
						   GL_FLOAT, GL_FALSE,
						   sizeof(sVAOVertex), 
						   ( void* )offsetof(sVAOVertex, boneID[0]));

	glEnableVertexAttribArray(vweight_location);
	glVertexAttribPointer(vweight_location, (int)NUMBEROFBONES,
						   GL_FLOAT, GL_FALSE,
						   sizeof(sVAOVertex), 
						   ( void* )offsetof(sVAOVertex, weights[0]));


	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(vpos_location);
	//glDisableVertexAttribArray(vcol_location);


	// Store the draw information into the map
	this->m_map_ModelName_to_VAOID[ drawInfo.meshName ] = drawInfo;
	this->m_map_ModelName_to_cMesh[ drawInfo.meshName ] = &theMesh;


	return true;
}


// We don't want to return an int, likely
bool cVAOManager::FindDrawInfoByModelName(
		std::string filename,
		sModelDrawInfo &drawInfo) 
{
	std::map< std::string /*model name*/,
			sModelDrawInfo /* info needed to draw*/ >::iterator 
		itDrawInfo = this->m_map_ModelName_to_VAOID.find( filename );

	// Find it? 
	if ( itDrawInfo == this->m_map_ModelName_to_VAOID.end() )
	{
		// Nope
		return false;
	}

	// Else we found the thing to draw
	// ...so 'return' that information
	drawInfo = itDrawInfo->second;
	return true;
}


cMesh* cVAOManager::FindMeshByModelName(std::string filename)
{
	std::map< std::string /*model name*/,
		cMesh* /* cMesh pointer */ >::iterator
		itMesh = this->m_map_ModelName_to_cMesh.find(filename);

	// Find it? 
	if (itMesh == this->m_map_ModelName_to_cMesh.end())
	{
		// Nope
		return nullptr;
	}

	// Else we found the thing to draw
	// ...so 'return' that information
	return (itMesh->second);
}