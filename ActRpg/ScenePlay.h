#pragma once

#include "GameScene.h"
#include "GamePlayer.h"
#include "GameMob.h"

class ScenePlay : public GameScene
{
public:
	ScenePlay();

	int Init() override;
	int Update() override;
	int Render() override;
	int Destroy() override;

private:
	//스테이지 설정
	void LoadStage(int stage);

	//카메라 이동
	void UpdateCamera();

	//현재 스테이지 몬스터 생성
	void SpawnStageMobs();

private:
	int m_txMap = -1;

	float m_mapWidth = 0.0f;
	float m_mapHeight = 0.0f;

	float m_cameraX = 0.0f;
	float m_cameraY = 0.0f;

	float m_cameraZoom = 1.5f;

	int m_currentStage = 1;
	//마린 HP UI용 폰트
	int m_fontHp = -1;

	GamePlayer m_player;

	static const int MAX_MOBS = 16;

	GameMob m_mobs[MAX_MOBS];

	int m_mobCount = 0;

	CollisionBox m_collisionBoxes[30];

	int m_collisionCount = 0;
};