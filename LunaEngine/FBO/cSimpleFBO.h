#pragma once

#include <string>
#include <_GL/GLCommon.h>

class cSimpleFBO
{
public:
	virtual ~cSimpleFBO() { shutdown(); }
	cSimpleFBO() :
		ID(0),
		colourTexture_ID(0),
		depthTexture_ID(0),
		width(-1), height(-1)
	{
	};

	GLuint ID;						// = 0;
	GLuint colourTexture_ID;		// = 0;
	GLuint depthTexture_ID;			// = 0;

	GLint width;		// = 512 the WIDTH of the framebuffer, in pixels;
	GLint height;

	// Inits the FBP
	virtual bool init(int width, int height, std::string& error);
	virtual bool shutdown(void);
	// Calls shutdown(), then init()
	virtual bool reset(int width, int height, std::string& error);
	virtual bool clear_all();
	inline void use() { glBindFramebuffer(GL_FRAMEBUFFER, ID); }

};
