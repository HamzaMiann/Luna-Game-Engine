#include <Texture/cBasicTextureManager.h>
#include <sstream>
#include <LodePNG/lodepng.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#pragma warning(disable)


void cBasicTextureManager::SetBasePath(std::string basepath)
{
	this->m_basePath = basepath;
	return;
}


bool cBasicTextureManager::Create2DTextureFromBMPFile( std::string textureFileName, bool bGenerateMIPMap )
{
	std::string fileToLoadFullPath = this->m_basePath + "/" + textureFileName;


	CTextureFromBMP* pTempTexture = new CTextureFromBMP();
	if ( ! pTempTexture->CreateNewTextureFromBMPFile2( textureFileName, fileToLoadFullPath, /*textureUnit,*/ bGenerateMIPMap ) )
	{
		this->m_appendErrorString( "Can't load " );
		this->m_appendErrorString( fileToLoadFullPath );
		this->m_appendErrorString( "\n" );
		return false;
	}

	// Texture is loaded OK
	//this->m_nextTextureUnitOffset++;
	
	this->m_map_TexNameToTexture[ textureFileName ] = pTempTexture;
	this->m_map_NameToID[textureFileName] = pTempTexture->getTextureNumber();

	return true;
}


bool cBasicTextureManager::Create2DTextureFromPNGFile(std::string textureFileName, bool bGenerateMIPMap)
{
	std::string fileToLoadFullPath = this->m_basePath + "/" + textureFileName;
	std::vector<unsigned char> image;
	unsigned width, height;
	unsigned error = lodepng::decode(image, width, height, fileToLoadFullPath);
	if (error != 0)
	{
		return false;
	}

	// Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
	size_t u2 = 1; while (u2 < width) u2 *= 2;
	size_t v2 = 1; while (v2 < height) v2 *= 2;
	// Ratio for power of two version compared to actual version, to render the non power of two image with proper size.
	double u3 = (double)width / u2;
	double v3 = (double)height / v2;

	std::vector<unsigned char> image2(u2 * v2 * 4);
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			for (size_t c = 0; c < 4; c++)
			{
				image2[4 * u2 * y + 4 * x + c] = image[4 * width * y + 4 * x + c];
			}
		}
	}

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image2[0]);
	//glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image2[0]);
	if (bGenerateMIPMap)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	this->m_map_NameToID[textureFileName] = texture;

	return true;
}

bool cBasicTextureManager::Create2DTextureFromJPGFile(std::string textureFileName, bool bGenerateMIPMap)
{
	std::string fileToLoadFullPath = this->m_basePath + "/" + textureFileName;

	// TAKEN FROM https://learnopengl.com/Getting-started/Textures

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(fileToLoadFullPath.c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		stbi_image_free(data);
		return false;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	if (bGenerateMIPMap)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	this->m_map_NameToID[textureFileName] = texture;

	stbi_image_free(data);
	return true;

}


void cBasicTextureManager::m_appendErrorString( std::string nextErrorText )
{
	std::stringstream ss;
	ss << this->m_lastError << nextErrorText;
	this->m_lastError = ss.str();
	return;
}

GLuint cBasicTextureManager::getTextureIDFromName( std::string textureFileName )
{
	//std::map< std::string, CTextureFromBMP* >::iterator itTexture
	//	= this->m_map_TexNameToTexture.find( textureFileName );
	//// Found it?
	//if ( itTexture == this->m_map_TexNameToTexture.end() )
	//{
	//	return 0;
	//}
	//// Reutrn texture number (from OpenGL genTexture)
	//return itTexture->second->getTextureNumber();



	std::map< std::string, GLuint >::iterator itTexture
		= this->m_map_NameToID.find(textureFileName);
	// Found it?
	if (itTexture == this->m_map_NameToID.end())
	{
		return 0;
	}
	// Reutrn texture number (from OpenGL genTexture)
	return itTexture->second;
}


void cBasicTextureManager::m_appendErrorStringLine( std::string nextErrorTextLine )
{
	std::stringstream ss;
	ss << this->m_lastError << std::endl;
	ss << nextErrorTextLine << std::endl;
	this->m_lastError = ss.str();
	return;
}


// Picks a random texture from the textures loaded
std::string cBasicTextureManager::PickRandomTexture(void)
{
	if ( this->m_map_TexNameToTexture.empty() )
	{
		// There are no textures loaded, yet.
		return "";
	}

	int textureIndexRand = rand() % (this->m_map_TexNameToTexture.size() + 1);
	if ( textureIndexRand >= this->m_map_TexNameToTexture.size() )
	{
		textureIndexRand = 0;
	}

	std::map< std::string, CTextureFromBMP* >::iterator itTex = this->m_map_TexNameToTexture.begin();
	for ( unsigned int count = 0; count != textureIndexRand; count++, itTex++ ) 
	{}

	return itTex->second->getTextureName();
}


bool cBasicTextureManager::CreateCubeTextureFromBMPFiles( 
                                    std::string cubeMapName, 
		                            std::string posX_fileName, std::string negX_fileName, 
		                            std::string posY_fileName, std::string negY_fileName, 
									std::string posZ_fileName, std::string negZ_fileName, 
									bool bIsSeamless, std::string &errorString )
{
	std::string posX_fileName_FullPath = this->m_basePath + "/" + posX_fileName;
	std::string negX_fileName_FullPath = this->m_basePath + "/" + negX_fileName;
	std::string posY_fileName_FullPath = this->m_basePath + "/" + posY_fileName;
	std::string negY_fileName_FullPath = this->m_basePath + "/" + negY_fileName;
	std::string posZ_fileName_FullPath = this->m_basePath + "/" + posZ_fileName;
	std::string negZ_fileName_FullPath = this->m_basePath + "/" + negZ_fileName;

	GLenum errorEnum;
	std::string errorDetails;
	CTextureFromBMP* pTempTexture = new CTextureFromBMP();
	if ( ! pTempTexture->CreateNewCubeTextureFromBMPFiles( 
				cubeMapName, 
				posX_fileName_FullPath, negX_fileName_FullPath, 
	            posY_fileName_FullPath, negY_fileName_FullPath, 
	            posZ_fileName_FullPath, negZ_fileName_FullPath, 
	            bIsSeamless, errorEnum, errorString, errorDetails ) )
	{
		this->m_appendErrorString( "Can't load " );
		this->m_appendErrorString( cubeMapName );
		this->m_appendErrorString( " because:\n" );
		this->m_appendErrorString( errorString );
		this->m_appendErrorString( "\n" );
		this->m_appendErrorString( errorDetails );
		errorString += ("\n" + errorDetails);
		return false;
	}//if ( ! pTempTexture->CreateNewCubeTextureFromBMPFiles()

	// Texture is loaded OK
	//this->m_nextTextureUnitOffset++;
	
	this->m_map_TexNameToTexture[ cubeMapName ] = pTempTexture;
	this->m_map_NameToID[ cubeMapName ] = pTempTexture->getTextureNumber();

	return true;
}
