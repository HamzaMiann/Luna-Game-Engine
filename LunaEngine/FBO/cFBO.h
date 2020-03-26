#ifndef _FBO_HG_
#define _FBO_HG_

#include <string>
#include <_GL/GLCommon.h>
#include <FBO/cSimpleFBO.h>

class cFBO : public cSimpleFBO
{
public:
	virtual ~cFBO() { }
	cFBO() : 
		cSimpleFBO(),
		normalTexture_ID(0), 
		positionTexture_ID(0),
		bloomTexture_ID(0),
		unlitTexture_ID(0)
	{ };

	GLuint normalTexture_ID;		// = 0;
	GLuint positionTexture_ID;		// = 0;
	GLuint bloomTexture_ID;			// = 0;
	GLuint unlitTexture_ID;			// = 0;

	// Inits the FBP
	virtual bool init(int width, int height, std::string &error) override;
	virtual bool shutdown(void) override;
	// Calls shutdown(), then init()
	virtual bool reset(int width, int height, std::string &error) override;
	
	void clearBuffers(bool bClearColour = true, bool bClearDepth = true, bool bClearNormal = true);

	void clearColourBuffer(int bufferindex);
	void clearAllColourBuffers(void);
	void clearDepthBuffer(void);
	void clearStencilBuffer( int clearColour, int mask = 0xFF );

	int getMaxColourAttachments(void);
	int getMaxDrawBuffers(void);
};

#endif
