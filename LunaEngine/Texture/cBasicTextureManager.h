#ifndef _cBasicTextureManager_HG_
#define _cBasicTextureManager_HG_

#include <string>
#include <map>
#include <vector>
#include <Texture/CTextureFromBMP.h>

struct sTextureData
{
	unsigned int width, height;
	std::vector<unsigned char> data;
};

class cBasicTextureManager
{
private:
	cBasicTextureManager() {}
public:
	bool Create2DTextureFromBMPFile( std::string textureFileName, bool bGenerateMIPMap );
	bool Create2DTextureFromPNGFile(std::string textureFileName, bool bGenerateMIPMap);
	bool Create2DTextureFromJPGFile(std::string textureFileName, bool bGenerateMIPMap);

	// Picks a random texture from the textures loaded
	std::string PickRandomTexture(void);

	bool LoadPNGFromFile(std::string textureFileName, sTextureData& textureData);
	bool LoadJPGFromFile(std::string textureFileName, sTextureData& textureData);

	bool Create2DTexture(std::string friendlyName, bool bGenerateMIPMap, unsigned char* data, int width, int height);

	bool Create3DTexture(std::string friendlyName, bool bGenerateMIPMap, unsigned char* data, int width, int height, int depth);

	bool CreateCubeTextureFromBMPFiles( std::string cubeMapName, 
		                                std::string posX_fileName, std::string negX_fileName, 
		                                std::string posY_fileName, std::string negY_fileName, 
										std::string posZ_fileName, std::string negZ_fileName, 
										bool bIsSeamless, std::string &errorString );

	bool CreateCubeTextureFromPNGFiles(	std::string cubeMapName,
										std::string posX_fileName, std::string negX_fileName,
										std::string posY_fileName, std::string negY_fileName,
										std::string posZ_fileName, std::string negZ_fileName,
										bool bIsSeamless, std::string& errorString);

	bool CreateCubeTextureFromJPGFiles(	std::string cubeMapName,
										std::string posX_fileName, std::string negX_fileName,
										std::string posY_fileName, std::string negY_fileName,
										std::string posZ_fileName, std::string negZ_fileName,
										bool bIsSeamless, std::string& errorString);

	unsigned int GenerateTexture();
	void BindTexture(unsigned int texture, int type = GL_TEXTURE_2D);
	void SetStandardTextureParameters(int type = GL_TEXTURE_2D);

	void SetTextureDataRGB(unsigned char* data, size_t width, size_t height, int type = GL_TEXTURE_2D);
	void SetTextureDataRGBA(unsigned char* data, size_t width, size_t height, int type = GL_TEXTURE_2D);
	void SetTextureDataBGRA(unsigned char* data, size_t width, size_t height, int type = GL_TEXTURE_2D);
	void GenerateMipmaps(bool bGenerateMIPMap, int type = GL_TEXTURE_2D);

	// returns 0 on error
	GLuint getTextureIDFromName( std::string textureFileName );

	void SetBasePath(std::string basepath);

	static cBasicTextureManager* Instance()
	{
		static cBasicTextureManager instance;
		return &instance;
	}

private:
	std::string m_basePath;
	std::string m_lastError;
	void m_appendErrorString( std::string nextErrorText );
	void m_appendErrorStringLine( std::string nextErrorTextLine );

	std::map< std::string, CTextureFromBMP* > m_map_TexNameToTexture;
	std::map< std::string, GLuint > m_map_NameToID;


};

#endif
