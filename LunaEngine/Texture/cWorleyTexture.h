#pragma once

#include <vector>
#include <glm/glm_common.h>
#include <Threading/safe_promise.h>

class cWorleyTexture
{
private:
	cWorleyTexture(size_t width, size_t redChannelSize, size_t greenChannelSize, size_t blueChannelSize);

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

		std::vector<vec3> mEdgePositions;
		std::vector<vec3> mGridPositions;

	} GridA, GridB, GridC;

	std::vector<Pixel> mPixels;
	
	size_t mWidth;

	void GenerateGrids();
	void GeneratePixels();
	void GeneratePixelsThreaded();

	static void _Generate(cWorleyTexture* texture, unsigned int zMin, unsigned int zMax);
	static float GetClosestDistance(const Grid& grid, float x, float y, float z, unsigned int mWidth);

public:
	
	~cWorleyTexture();

	inline static cWorleyTexture* Generate(size_t pixelWidth, size_t redChannelSize, size_t greenChannelSize, size_t blueChannelSize)
	{
		return new cWorleyTexture(pixelWidth, redChannelSize, greenChannelSize, blueChannelSize);
	}

	static void GenerateAsyncPromise(std::promise<cWorleyTexture*>* promise, size_t pixelWidth, size_t redChannelSize, size_t greenChannelSize, size_t blueChannelSize)
	{
		cWorleyTexture* tex = new cWorleyTexture(pixelWidth, redChannelSize, greenChannelSize, blueChannelSize);
		promise->set_value(tex);
	}

	static void GenerateAsync(safe_promise<cWorleyTexture*>* promise, size_t pixelWidth, size_t redChannelSize, size_t greenChannelSize, size_t blueChannelSize)
	{
		cWorleyTexture* tex = new cWorleyTexture(pixelWidth, redChannelSize, greenChannelSize, blueChannelSize);
		promise->set_value(tex);
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