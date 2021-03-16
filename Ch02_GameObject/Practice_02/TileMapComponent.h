#pragma once
#include "SpriteComponent.h"
#include "Math.h"
#include <vector>
#include <string>
class TileMapComponent : public SpriteComponent
{
public:
	// 기본 그리기 순서값을 낮은 값으로 설정 (배경이므로)
	TileMapComponent(class Actor* owner, int drawOrder = 10);

	// 부모로부터 오버라이딩한 draw 함수
	//void Draw(SDL_Renderer* renderer) override;
	void SetTMTexture(SDL_Texture* texture, const std::string& mapLayer);
	void Draw(SDL_Renderer* renderer) override;

private:
	struct TMTexture
	{
		int index;
		Vector2 mOffset;
	};
	
	const int tileWidth = 32;
	const int tileHeight = 32;
	const int widthCnt = 32;
	const int heightCnt = 24;
	std::vector<TMTexture> mTMTextures;
	

};