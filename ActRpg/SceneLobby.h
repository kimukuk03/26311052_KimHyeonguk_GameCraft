#pragma once
#include "GameScene.h"

class SceneLobby : public GameScene
{
public:
	SceneLobby();

	int Init() override;
	int Update() override;
	int Render() override;
	int Destroy() override;

private:
	int m_txLobbyBG = -1;
};