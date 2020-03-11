#include "cWorleyTexture.h"
#include <_GL/GLCommon.h>
#include <iostream>
#include <Physics/Mathf.h>

// FROM https://github.com/Reputeless/PerlinNoise/blob/master/PerlinNoise.hpp
//#include <Texture/PerlinNoise.hpp>


#define SEPARATION 1.f

cWorleyTexture::cWorleyTexture(size_t width, size_t redChannelSize, size_t greenChannelSize, size_t blueChannelSize):
	mWidth(width),
	GridA(redChannelSize),
	GridB(greenChannelSize),
	GridC(blueChannelSize)
{
	srand((int)(glfwGetTime() * 10.f));
	GenerateGrids();
	GeneratePixels();
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
	//siv::BasicPerlinNoise<float> perlin;

	for (size_t z = 0; z < mWidth; ++z)
	{
		for (size_t y = 0; y < mWidth; ++y)
		{
			for (size_t x = 0; x < mWidth; ++x)
			{
				size_t pixelIndex = (z * mWidth * mWidth) + (y * mWidth) + x;

				mPixels[pixelIndex].r = glm::clamp(1.f - GetClosestDistance(GridA, x, y, z) * SEPARATION, 0.f, 1.f) * 255;
				mPixels[pixelIndex].g = glm::clamp(1.f - GetClosestDistance(GridB, x, y, z) * SEPARATION, 0.f, 1.f) * 255;
				mPixels[pixelIndex].b = glm::clamp(1.f - GetClosestDistance(GridC, x, y, z) * SEPARATION, 0.f, 1.f) * 255;
			}
		}
	}
}

float cWorleyTexture::GetClosestDistance(const Grid& grid, float x, float y, float z)
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
