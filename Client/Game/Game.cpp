#include "pch.h"
#include "Game.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "UIManager.h"

void Game::Init(const WindowInfo &info)
{
	GEngine->Init(info);
	GET_SINGLE(UIManager)->Init(info.hwnd);
	GET_SINGLE(SceneManager)->LoadScene(LoadGameScene());
}

void Game::Update()
{
	GEngine->Update(); // 매 프레임 업데이트
}
