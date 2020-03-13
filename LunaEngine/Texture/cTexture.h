#pragma once

#include <string>

class cTexture
{
	bool initialized;
	std::string filename;
	unsigned int ID;
	float blend;
	float tiling;

public:
	~cTexture() {}

	// CONSTRUCTORS
	cTexture();
	cTexture(int ID);
	cTexture(std::string name);

	// SET TEXTURES
	void SetTexture(std::string name);
	void SetTexture(std::string name, float blend);
	void SetTexture(int ID);
	void SetTexture(int ID, float blend);

	int GetID();

	inline std::string GetName() { return filename; }

	inline void SetBlend(float amount) { blend = amount; }

	inline float GetBlend() { return blend; }

	inline void SetTiling(float value) { tiling = value; }

	inline float GetTiling() { return tiling; }
	
};