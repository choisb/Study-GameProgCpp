#include "TileMapComponent.h"
#include "Actor.h"
#include <fstream>


TileMapComponent::TileMapComponent(Actor* owner,  int drawOrder)
	: SpriteComponent(owner, drawOrder)

{
}
void TileMapComponent::SetTMTexture(SDL_Texture* texture, const std::string& mapLayer)
{
	std::ifstream file;
	std::string str_buf;
	TMTexture temp;
	int index;
	int cnt = 0;

	file.open(mapLayer, std::ios::in);

	for (int i = 1; i <= widthCnt * heightCnt; i++ ) {
		if (i % widthCnt != 0)
			getline(file, str_buf, ',');
		else
			getline(file, str_buf, '\n');

		index = atoi(str_buf.c_str());

		temp.index = index;
		temp.mOffset.x = ((i-1) % widthCnt) * tileWidth;
		temp.mOffset.y = ((i-1) / widthCnt) * tileHeight;


		mTMTextures.emplace_back(temp);

	}

	file.close();

	SetTexture(texture);
}
void TileMapComponent::Draw(SDL_Renderer* renderer)
{
	if (!mTexture)
		return;

	for (auto& tm : mTMTextures)
	{
		if (tm.index == -1)
			continue;

		SDL_Rect srcR;

		srcR.w = tileWidth;
		srcR.h = tileHeight;
		srcR.x = (tm.index % 8) * 32;
		srcR.y = (tm.index / 8) * 32;

		SDL_Rect dstR;

		dstR.w = tileWidth;
		dstR.h = tileHeight;
		dstR.x = static_cast<int>(tm.mOffset.x);
		dstR.y = static_cast<int>(tm.mOffset.y);


		SDL_RenderCopyEx(renderer,
			mTexture,
			&srcR,
			&dstR,
			0,
			nullptr,
			SDL_FLIP_NONE);
		
	}
	
}
