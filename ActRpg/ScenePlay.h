#pragma once
#include "GameScene.h"
#include <vector>

struct CollisionBox
{
	float x;		// 충돌 영역 왼쪽 위 X
	float y;		// 충돌 영역 왼쪽 위 Y
	float width;	// 충돌 영역 가로 크기
	float height;	// 충돌 영역 세로 크기
};

struct Bullet
{
	float x;
	float y;

	float dirX;
	float dirY;

	float speed;
};

class ScenePlay : public GameScene
{
public:
	ScenePlay();

	int Init() override;
	int Update() override;
	int Render() override;
	int Destroy() override;

protected:
	// =========================
	// Stage 1 맵
	// =========================

	int m_txStage1 = -1;

	float m_mapWidth = 0.0f;
	float m_mapHeight = 0.0f;

	// =========================
	// 충돌 영역
	// =========================

	// 최대 30개의 벽/바위 충돌 영역 저장
	CollisionBox m_collisionBoxes[30];

	// 실제 사용하는 충돌 영역 개수
	int m_collisionCount = 0;


	// 마린의 충돌 크기
	// 실제 이미지보다 작게 잡는 게 자연스러움
	float m_playerCollisionW = 35.0f;
	float m_playerCollisionH = 35.0f;


	// =========================
	// 플레이어(마린)
	// =========================

	// =========================
// Marine
// =========================

// 8방향 × 2프레임 = 16장
	int m_txMarineMove[16];
	int m_txMarineAttack[16];

	// 총알 이미지
	int m_txBullet = -1;


	// 현재 바라보는 방향
	//
	// 0 = 아래
	// 1 = 오른쪽 아래
	// 2 = 오른쪽
	// 3 = 오른쪽 위
	// 4 = 위
	// 5 = 왼쪽 위
	// 6 = 왼쪽
	// 7 = 왼쪽 아래
	int m_facingDir = 0;


	// 이동 애니메이션 프레임
	// 0 또는 1
	int m_moveFrame = 0;

	bool m_isMoving = false;
	bool m_isAttacking = false;


	// 애니메이션 시간
	unsigned long long m_lastMoveAnimTime = 0;
	unsigned long long m_attackStartTime = 0;


	// 총알들
	std::vector<Bullet> m_bullets;


	// 함수
	void UpdateMarineDirection();
	void ShootBullet();

	bool CheckBulletCollision(float x, float y);

	// 실제 맵 안에서 마린 위치
	float m_playerX = 300.0f;
	float m_playerY = 500.0f;

	// 마린 이동 속도
	float m_playerSpeed = 3.0f;

	bool CheckCollision(float x, float y);

	// =========================
	// 카메라
	// =========================

	float m_cameraX = 0.0f;
	float m_cameraY = 0.0f;

	float m_cameraZoom = 1.5f;

	// 현재 스테이지
	int m_currentStage = 1;

	void LoadStage(int stage);
};