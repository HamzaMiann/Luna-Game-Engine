#pragma once

#include <vector>
#include <glm/glm_common.h>

class cWorleyTexture
{
private:
	cWorleyTexture(size_t width, size_t redChannelSize, size_t greenChannelSize, size_t blueChannelSize);
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

	struct Grid
	{
		Grid(size_t gridWidth):
			mGridWidth(gridWidth)
		{
		}

		void Generate();

		size_t mGridWidth;

		std::vector<vec2> mEdgePositions;
		std::vector<vec2> mGridPositions;

	} GridA, GridB, GridC;

	std::vector<Pixel> mPixels;
	
	size_t mWidth;

	void GenerateGrids();
	void GeneratePixels();
	float GetClosestDistance(const Grid& grid, float x, float y);

public:
	

	inline static cWorleyTexture* Generate(size_t pixelWidth, size_t redChannelSize, size_t greenChannelSize, size_t blueChannelSize)
	{
		return new cWorleyTexture(pixelWidth, redChannelSize, greenChannelSize, blueChannelSize);
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