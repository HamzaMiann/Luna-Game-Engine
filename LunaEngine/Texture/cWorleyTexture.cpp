#include "cWorleyTexture.h"
#include <Physics/Mathf.h>

// FROM https://github.com/Reputeless/PerlinNoise/blob/master/PerlinNoise.hpp
#include <Texture/PerlinNoise.hpp>

cWorleyTexture::cWorleyTexture(size_t width, size_t gridWidth):
	mWidth(width),
	mGridWidth(gridWidth)
{
	GenerateGrid();
	GeneratePixels();
}

cWorleyTexture::~cWorleyTexture()
{
	mPixels.clear();
}

void cWorleyTexture::GenerateGrid()
{
	mGridPositions.resize(mGridWidth * mGridWidth);

	for (size_t y = 0; y < mGridWidth; ++y)
	{
		for (size_t x = 0; x < mGridWidth; ++x)
		{
			size_t index = (y * mGridWidth) + x;
			mGridPositions[index].x = Mathf::randInRange(0.f, 1.f) + x;
			mGridPositions[index].y = Mathf::randInRange(0.f, 1.f) + y;
		}
	}

	for (size_t x = 0; x < mGridWidth; ++x)
	{
		size_t index = x;
		vec2 pos = mGridPositions[index];
		pos.y += mGridWidth;
		mEdgePositions.push_back(pos);

		index = ((mGridWidth - 1) * mGridWidth) + x;
		pos = mGridPositions[index];
		pos.y -= mGridWidth;
		mEdgePositions.push_back(pos);
	}

	for (size_t y = 0; y < mGridWidth; ++y)
	{
		size_t index = y * mGridWidth;
		vec2 pos = mGridPositions[index];
		pos.x += mGridWidth;
		mEdgePositions.push_back(pos);

		index = (y * mGridWidth) + (mGridWidth - 1);
		pos = mGridPositions[index];
		pos.x -= mGridWidth;
		mEdgePositions.push_back(pos);
	}
}

void cWorleyTexture::GeneratePixels()
{
	mPixels.resize(mWidth * mWidth);
	siv::BasicPerlinNoise<float> perlin;

	for (size_t y = 0; y < mWidth; ++y)
	{
		for (size_t x = 0; x < mWidth; ++x)
		{
			float gridX = (x / (float)mWidth) * mGridWidth;
			float gridY = (y / (float)mWidth) * mGridWidth;
			vec2 pos(gridX, gridY);


			float d = 1.f - GetClosestDistance(pos) * 1.1f;

			//float noise = perlin.noise2D_0_1(gridX * 10.f, gridY * 10.f);

			//d = glm::mix(d, noise, 0.3f);

			float colour = glm::clamp(d, 0.f, 1.f) * 255;

			size_t pixelIndex = (y * mWidth) + x;
			mPixels[pixelIndex].r = colour;
			mPixels[pixelIndex].g = colour;
			mPixels[pixelIndex].b = colour;

			

		}
	}
}

float cWorleyTexture::GetClosestDistance(const vec2& position)
{
	float max = FLT_MAX;
	for (size_t i = 0; i < mGridPositions.size(); ++i)
	{
		float d = glm::distance(mGridPositions[i], position);
		if (d < max) max = d;
	}
	for (size_t i = 0; i < mEdgePositions.size(); ++i)
	{
		float d = glm::distance(mEdgePositions[i], position);
		if (d < max) max = d;
	}
	return max;
}
