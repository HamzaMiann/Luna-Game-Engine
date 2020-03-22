
#pragma warning(disable)

#include <Texture/cBasicTextureManager.h>
#include <sstream>
#include <LodePNG/lodepng.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



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

struct Pixel
{
	uchar r;
	uchar g;
	uchar b;
};

struct PixelA
{
	uchar r;
	uchar g;
	uchar b;
	uchar a = UCHAR_MAX;
};

bool cBasicTextureManager::Create2DTextureFromPNGFile(std::string textureFileName, bool bGenerateMIPMap)
{
	std::string fileToLoadFullPath = this->m_basePath + "/" + textureFileName;

	sTextureData tex;
	if (!LoadPNGFromFile(fileToLoadFullPath, tex))
	{
		return false;
	}

	unsigned int texture = GenerateTexture();
	BindTexture(texture);
	SetStandardTextureParameters();
	SetTextureDataRGBA(&tex.data[0], tex.width, tex.height);
	GenerateMipmaps(bGenerateMIPMap);

	this->m_map_NameToID[textureFileName] = texture;

	return true;
}

bool cBasicTextureManager::Create2DTextureFromJPGFile(std::string textureFileName, bool bGenerateMIPMap)
{
	std::string fileToLoadFullPath = this->m_basePath + "/" + textureFileName;

	sTextureData tex;
	if (!LoadJPGFromFile(fileToLoadFullPath, tex))
	{
		return false;
	}

	unsigned int texture = GenerateTexture();
	BindTexture(texture);
	SetStandardTextureParameters();
	SetTextureDataRGB(&tex.data[0], tex.width, tex.height);
	GenerateMipmaps(bGenerateMIPMap);

	this->m_map_NameToID[textureFileName] = texture;

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

bool cBasicTextureManager::LoadPNGFromFile(std::string textureFileName, sTextureData& textureData)
{
	std::vector<unsigned char> image;
	unsigned int width, height;
	unsigned error = lodepng::decode(image, width, height, textureFileName);
	if (error != 0)
	{
		return false;
	}

	std::vector<PixelA> pixels;
	pixels.resize(width * height);
	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			unsigned int newY = height - y - 1;
			uchar* rgba = &image[x * 4u + y * width * 4u];
			pixels[x + newY * width].r = rgba[0];
			pixels[x + newY * width].g = rgba[1];
			pixels[x + newY * width].b = rgba[2];
			pixels[x + newY * width].a = rgba[3];
		}
	}

	textureData.width = width;
	textureData.height = height;
	for (unsigned int i = 0; i < pixels.size(); ++i)
	{
		textureData.data.push_back(pixels[i].r);
		textureData.data.push_back(pixels[i].g);
		textureData.data.push_back(pixels[i].b);
		textureData.data.push_back(pixels[i].a);
	}

	return true;
}

bool cBasicTextureManager::LoadJPGFromFile(std::string textureFileName, sTextureData& textureData)
{
	// TAKEN FROM https://learnopengl.com/Getting-started/Textures
	// load the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(textureFileName.c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		stbi_image_free(data);
		return false;
	}

	std::vector<Pixel> pixels;
	pixels.resize(width * height);
	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			unsigned int newY = height - y - 1;
			uchar* rgb = &data[x * 3u + y * width * 3u];
			pixels[x + newY * width].r = rgb[0];
			pixels[x + newY * width].g = rgb[1];
			pixels[x + newY * width].b = rgb[2];
		}
	}

	textureData.width = width;
	textureData.height = height;
	for (unsigned int i = 0; i < pixels.size(); ++i)
	{
		textureData.data.push_back(pixels[i].r);
		textureData.data.push_back(pixels[i].g);
		textureData.data.push_back(pixels[i].b);
	}

	stbi_image_free(data);

	return true;
}

bool cBasicTextureManager::Create2DTexture(std::string friendlyName, bool bGenerateMIPMap, unsigned char* data, int width, int height)
{
	unsigned int texture = GenerateTexture();
	BindTexture(texture);
	SetStandardTextureParameters();
	SetTextureDataBGRA(data, width, height);
	GenerateMipmaps(bGenerateMIPMap);

	this->m_map_NameToID[friendlyName] = texture;

	return true;
}

bool cBasicTextureManager::Create3DTexture(std::string friendlyName, bool bGenerateMIPMap, unsigned char* data, int width, int height, int depth)
{
	unsigned int texture = GenerateTexture();
	BindTexture(texture, GL_TEXTURE_3D);
	SetStandardTextureParameters(GL_TEXTURE_3D);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	GenerateMipmaps(bGenerateMIPMap, GL_TEXTURE_3D);
	this->m_map_NameToID[friendlyName] = texture;

	return true;
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

bool cBasicTextureManager::CreateCubeTextureFromPNGFiles(	std::string cubeMapName,
															std::string posX_fileName, std::string negX_fileName,
															std::string posY_fileName, std::string negY_fileName,
															std::string posZ_fileName, std::string negZ_fileName,
															bool bIsSeamless,
															std::string& errorString)
{

	std::vector<std::string> files = { posX_fileName, negX_fileName, posY_fileName, negY_fileName, posZ_fileName, negZ_fileName };

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (unsigned int i = 0; i < files.size(); ++i)
	{
		std::string& file = files[i];
		std::string fileToLoadFullPath = this->m_basePath + "/" + file;
		std::vector<unsigned char> image;
		unsigned width, height;
		unsigned error = lodepng::decode(image, width, height, fileToLoadFullPath);
		if (error != 0)
		{
			errorString = "Error: Could not load cubemap (" + error + std::string(")");
			return false;
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	m_map_NameToID[cubeMapName] = textureID;

	return true;
}

bool cBasicTextureManager::CreateCubeTextureFromJPGFiles(	std::string cubeMapName,
															std::string posX_fileName, std::string negX_fileName,
															std::string posY_fileName, std::string negY_fileName,
															std::string posZ_fileName, std::string negZ_fileName,
															bool bIsSeamless,
															std::string& errorString)
{
	// TAKEN FROM https://learnopengl.com/Advanced-OpenGL/Cubemaps

	std::vector<std::string> files = { posX_fileName, negX_fileName, posY_fileName, negY_fileName, posZ_fileName, negZ_fileName };

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < files.size(); i++)
	{
		std::string fileToLoadFullPath = this->m_basePath + "/" + files[i];
		unsigned char* data = stbi_load(fileToLoadFullPath.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			stbi_image_free(data);
			return false;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	m_map_NameToID[cubeMapName] = textureID;

	return true;
}

unsigned int cBasicTextureManager::GenerateTexture()
{
	unsigned int texture;
	glGenTextures(1, &texture);
	return texture;
}

void cBasicTextureManager::BindTexture(unsigned int texture, int type)
{
	glBindTexture(type, texture);
}

void cBasicTextureManager::SetStandardTextureParameters(int type)
{
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void cBasicTextureManager::SetTextureDataRGB(unsigned char* data, size_t width, size_t height, int type)
{
	glTexImage2D(type, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}

void cBasicTextureManager::SetTextureDataRGBA(unsigned char* data, size_t width, size_t height, int type)
{
	glTexImage2D(type, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void cBasicTextureManager::SetTextureDataBGRA(unsigned char* data, size_t width, size_t height, int type)
{
	glTexImage2D(type, 0, GL_BGRA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
}

void cBasicTextureManager::GenerateMipmaps(bool bGenerateMIPMap, int type)
{
	if (bGenerateMIPMap)
	{
		glGenerateMipmap(type);
	}
}

