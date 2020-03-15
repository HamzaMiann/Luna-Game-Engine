#include "cWorleyTexture.h"
#include <_GL/GLCommon.h>
#include <iostream>
#include <Physics/Mathf.h>

// FROM https://github.com/Reputeless/PerlinNoise/blob/master/PerlinNoise.hpp
#include <Texture/PerlinNoise.hpp>


#define SEPARATION 1.f
#define THREADED

cWorleyTexture::cWorleyTexture(size_t width, size_t redChannelSize, size_t greenChannelSize, size_t blueChannelSize):
	mWidth(width),
	GridA(redChannelSize),
	GridB(greenChannelSize),
	GridC(blueChannelSize)
{
	srand(23);
	GenerateGrids();

#ifdef THREADED
	GeneratePixelsThreaded();
#else
	GeneratePixels();
#endif
}

void cWorleyTexture::_Generate(cWorleyTexture* texture, unsigned int zMin, unsigned int zMax)
{
	siv::BasicPerlinNoise<float> perlin;

	float mWidth = texture->mWidth;
	auto& mPixels = texture->mPixels;

	for (size_t z = zMin; z < zMax; ++z)
	{
		for (size_t y = 0; y < mWidth; ++y)
		{
			for (size_t x = 0; x < mWidth; ++x)
			{
				size_t pixelIndex = (z * mWidth * mWidth) + (y * mWidth) + x;

				float rColour = glm::clamp(1.f - GetClosestDistance(texture->GridA, x, y, z, mWidth) * SEPARATION, 0.f, 1.f);
				//float rColour = perlin.noise2D_0_1(x / (float)mWidth, y / (float)mWidth);
				//rColour = perlin.noise3D_0_1(x / (float)mWidth, y / (float)mWidth, z / (float)mWidth);
				mPixels[pixelIndex].r = rColour * 255;
				mPixels[pixelIndex].g = glm::clamp(1.f - GetClosestDistance(texture->GridB, x, y, z, mWidth) * SEPARATION, 0.f, 1.f) * 255;
				mPixels[pixelIndex].b = glm::clamp(1.f - GetClosestDistance(texture->GridC, x, y, z, mWidth) * SEPARATION, 0.f, 1.f) * 255;
			}
		}
	}
}

cWorleyTexture::~cWorleyTexture()
{
	mPixels.clear();
}

void cWorleyTexture::GenerateGrids()
{
	GridA.Generate();
	GridB.Generate();
	GridC.Generate();
}


void cWorleyTexture::GeneratePixels()
{
	mPixels.resize(mWidth * mWidth * mWidth);

	_Generate(this, 0, mWidth);

}

void cWorleyTexture::GeneratePixelsThreaded()
{
	mPixels.resize(mWidth * mWidth * mWidth);

	std::thread* thread1 = new std::thread(_Generate, this, 0, (mWidth / 2));
	std::thread* thread2 = new std::thread(_Generate, this, (mWidth / 2), mWidth);

	thread1->join();
	thread2->join();
}

float cWorleyTexture::GetClosestDistance(const Grid& grid, float x, float y, float z, unsigned int mWidth)
{
	float gridX = (x / (float)mWidth) * grid.mGridWidth;
	float gridY = (y / (float)mWidth) * grid.mGridWidth;
	float gridZ = (z / (float)mWidth) * grid.mGridWidth;
	vec3 position(gridX, gridY, gridZ);

	float max = FLT_MAX;
	for (size_t i = 0; i < grid.mGridPositions.size(); ++i)
	{
		float d = glm::distance(grid.mGridPositions[i], position);
		if (d < max) max = d;
	}
	for (size_t i = 0; i < grid.mEdgePositions.size(); ++i)
	{
		float d = glm::distance(grid.mEdgePositions[i], position);
		if (d < max) max = d;
	}
	return max;
}

void cWorleyTexture::Grid::Generate()
{
	mGridPositions.resize(mGridWidth * mGridWidth * mGridWidth);

	for (size_t z = 0; z < mGridWidth; ++z)
	{
		for (size_t y = 0; y < mGridWidth; ++y)
		{
			for (size_t x = 0; x < mGridWidth; ++x)
			{
				size_t index = (z * mGridWidth * mGridWidth) + (y * mGridWidth) + x;
				mGridPositions[index].x = Mathf::randInRange(0.f, 1.f) + x;
				mGridPositions[index].y = Mathf::randInRange(0.f, 1.f) + y;
				mGridPositions[index].z = Mathf::randInRange(0.f, 1.f) + z;
			}
		}
	}

	for (size_t z = 0; z < mGridWidth; ++z)
	{
		for (size_t x = 0; x < mGridWidth; ++x)
		{
			size_t index = x + (z * mGridWidth * mGridWidth);
			vec3 pos = mGridPositions[index];
			pos.y += mGridWidth;
			mEdgePositions.push_back(pos);

			index = ((mGridWidth - 1) * mGridWidth) + x + (z * mGridWidth * mGridWidth);
			pos = mGridPositions[index];
			pos.y -= mGridWidth;
			mEdgePositions.push_back(pos);
		}
	}

	for (size_t z = 0; z < mGridWidth; ++z)
	{
		for (size_t y = 0; y < mGridWidth; ++y)
		{
			size_t index = y * mGridWidth + (z * mGridWidth * mGridWidth);
			vec3 pos = mGridPositions[index];
			pos.x += mGridWidth;
			mEdgePositions.push_back(pos);

			index = (y * mGridWidth) + (mGridWidth - 1) + (z * mGridWidth * mGridWidth);
			pos = mGridPositions[index];
			pos.x -= mGridWidth;
			mEdgePositions.push_back(pos);
		}
	}

	for (size_t z = 0; z < mGridWidth; ++z)
	{
		vec3 pos = mGridPositions[0 + z * mGridWidth * mGridWidth];
		pos.x += mGridWidth;
		pos.y += mGridWidth;
		mEdgePositions.push_back(pos);

		pos = mGridPositions[mGridWidth - 1 + (z * mGridWidth * mGridWidth)];
		pos.x -= mGridWidth;
		pos.y += mGridWidth;
		mEdgePositions.push_back(pos);

		pos = mGridPositions[mGridWidth * (mGridWidth - 1) + (z * mGridWidth * mGridWidth)];
		pos.x += mGridWidth;
		pos.y -= mGridWidth;
		mEdgePositions.push_back(pos);

		pos = mGridPositions[mGridWidth * (mGridWidth - 1) + (mGridWidth - 1) + (z * mGridWidth * mGridWidth)];
		pos.x -= mGridWidth;
		pos.y -= mGridWidth;
		mEdgePositions.push_back(pos);
	}
}
