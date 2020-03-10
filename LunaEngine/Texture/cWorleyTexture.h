#pragma once

#include <vector>
#include <glm/glm_common.h>

class cWorleyTexture
{
private:
	cWorleyTexture(size_t width, size_t gridWidth);
	~cWorleyTexture();

	struct Pixel
	{
		Pixel():
			r(0),
			g(0),
			b(0)
		{ }
		unsigned char r, g, b;
	};

	std::vector<Pixel> mPixels;
	std::vector<vec2> mEdgePositions;
	std::vector<vec2> mGridPositions;
	size_t mWidth;
	size_t mGridWidth;

	void GenerateGrid();
	void GeneratePixels();
	float GetClosestDistance(const vec2& position);

public:
	

	inline static cWorleyTexture* Generate(size_t pixelWidth, size_t gridWidth)
	{
		return new cWorleyTexture(pixelWidth, gridWidth);
	}

	inline unsigned char* GetDataRGB(size_t& width, size_t& height)
	{
		width = mWidth;
		height = mWidth;
		return &mPixels[0].r;
	}

	inline size_t GetPixelWidth() { return mWidth; }
	inline size_t GetGridWidth() { return mWidth; }
};