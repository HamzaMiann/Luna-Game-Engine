#ifndef _FBO_HG_
#define _FBO_HG_

#include <string>
#include <_GL/GLCommon.h>

class cFBO
{
public:
	~cFBO() { shutdown(); }
	cFBO() : 
		ID(0), 
		colourTexture_0_ID(0),
		depthTexture_ID(0), 
		normalTexture_ID(0), 
		positionTexture_ID(0),
		bloomTexture_ID(0),
		unlitTexture_ID(0),
		width(-1), height(-1) {};

	GLuint ID;						// = 0;
	GLuint colourTexture_0_ID;		// = 0;
	GLuint depthTexture_ID;			// = 0;
	GLuint normalTexture_ID;		// = 0;
	GLuint positionTexture_ID;		// = 0;
	GLuint bloomTexture_ID;		// = 0;
	GLuint unlitTexture_ID;		// = 0;

//	GLuint TBDTexture_1_ID;
//	GLuint TBDTexture_2_ID;

	GLint width;		// = 512 the WIDTH of the framebuffer, in pixels;
	GLint height;

	// Inits the FBP
	bool init(int width, int height, std::string &error);
	bool shutdown(void);
	// Calls shutdown(), then init()
	bool reset(int width, int height, std::string &error);
	inline void use() { glBindFramebuffer(GL_FRAMEBUFFER, ID); }
	
	void clearBuffers(bool bClearColour = true, bool bClearDepth = true, bool bClearNormal = true);

	void clearColourBuffer(int bufferindex);
	void clearAllColourBuffers(void);
	void clearDepthBuffer(void);
	void clearStencilBuffer( int clearColour, int mask = 0xFF );

	int getMaxColourAttachments(void);
	int getMaxDrawBuffers(void);
};

#endif
