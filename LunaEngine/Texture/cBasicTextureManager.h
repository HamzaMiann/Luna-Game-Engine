#ifndef _cBasicTextureManager_HG_
#define _cBasicTextureManager_HG_

#include <string>
#include <map>
#include <vector>
#include <Texture/CTextureFromBMP.h>
#include <mutex>

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
	bool LoadWorleyFromFile(std::string textureFileName, sTextureData& textureData);

	bool Create2DTexture(std::string friendlyName, bool bGenerateMIPMap, unsigned char* data, int width, int height);
	bool Create2DTextureRGB(std::string friendlyName, sTextureData& texData, bool bGenerateMIPMap);
	bool Create2DTextureRGBA(std::string friendlyName, sTextureData& texData, bool bGenerateMIPMap);

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

	

	// returns 0 on error
	GLuint getTextureIDFromName( std::string textureFileName );
	void GetTextureIDAsync( std::string textureFileName, unsigned int* texture );

	void SetBasePath(std::string basepath);

	static cBasicTextureManager* Instance()
	{
		static cBasicTextureManager instance;
		return &instance;
	}

private:
	typedef std::lock_guard<std::mutex> auto_lock;
	std::mutex mtx;

	std::map<std::string, std::vector<unsigned int*>> async_queue;

	std::string m_basePath;
	std::string m_lastError;
	void m_appendErrorString( std::string nextErrorText );
	void m_appendErrorStringLine( std::string nextErrorTextLine );

	std::map< std::string, CTextureFromBMP* > m_map_TexNameToTexture;
	std::map< std::string, GLuint > m_map_NameToID;

	unsigned int GenerateTexture();
	void BindTexture(unsigned int texture, int type = GL_TEXTURE_2D);
	void SetStandardTextureParameters(int type = GL_TEXTURE_2D);

	void SetTextureDataRGB(unsigned char* data, size_t width, size_t height, int type = GL_TEXTURE_2D);
	void SetTextureDataRGBA(unsigned char* data, size_t width, size_t height, int type = GL_TEXTURE_2D);
	void SetTextureDataBGRA(unsigned char* data, size_t width, size_t height, int type = GL_TEXTURE_2D);
	void GenerateMipmaps(bool bGenerateMIPMap, int type = GL_TEXTURE_2D);
	void SetTextureIDForName(std::string textureFileName, unsigned int texture);
	void UpdateTextureRequests(std::string textureFileName, unsigned int texture);
};

#endif
