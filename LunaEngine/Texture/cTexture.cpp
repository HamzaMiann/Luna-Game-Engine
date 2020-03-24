#include <Texture/cTexture.h>
#include <Texture/cBasicTextureManager.h>

cTexture::cTexture() :
	ID(0),
	filename(""),
	initialized(false),
	blend(0.f),
	tiling(1.f)
{
}

cTexture::cTexture(int ID) :
	ID(ID),
	filename(""),
	initialized(true),
	blend(0.f),
	tiling(1.f)
{
}

cTexture::cTexture(std::string name) :
	filename(name),
	//ID(cBasicTextureManager::Instance()->getTextureIDFromName(name)),
	ID(0),
	initialized(true),
	blend(0.f),
	tiling(1.f)
{
	cBasicTextureManager::Instance()->GetTextureIDAsync(name, &ID);
}

void cTexture::SetTexture(std::string name)
{
	cBasicTextureManager::Instance()->GetTextureIDAsync(name, &ID);
	//ID = cBasicTextureManager::Instance()->getTextureIDFromName(name);
	filename = name;
	initialized = true;
}

void cTexture::SetTexture(std::string name, float blend)
{
	//ID = cBasicTextureManager::Instance()->getTextureIDFromName(name);
	cBasicTextureManager::Instance()->GetTextureIDAsync(name, &ID);
	filename = name;
	this->blend = blend;
	initialized = true;
}

void cTexture::SetTexture(int ID)
{
	this->ID = ID;
	this->filename = "";
	initialized = true;
}

void cTexture::SetTexture(int ID, float blend)
{
	this->ID = ID;
	this->filename = "";
	this->blend = blend;
	initialized = true;
}

int cTexture::GetID()
{
	/*if (!initialized)
	{
		cBasicTextureManager::Instance()->getTextureIDFromName(filename);
		initialized = true;
	}*/
	return ID;
}
