#pragma once

#include "GameCharacter.h"
#include <vector>

//마린이 발사한 총알
struct Bullet
{
	float x;
	float y;

	float dirX;
	float dirY;

	float speed;

	//총알이 너무 오래 남지 않도록 수명 설정
	float life;
};

class GamePlayer : public GameCharacter
{
public:
	GamePlayer();

	//마린 리소스 초기화
	int Init();

	//마린 리소스 해제
	int Destroy();

	//WASD 이동 처리
	void UpdateMovement(
		CollisionBox* collisionBoxes,
		int collisionCount,
		float mapWidth,
		float mapHeight
	);

	//마우스 조준과 사격 처리
	void UpdateCombat(
		float cameraX,
		float cameraY,
		float cameraZoom,
		CollisionBox* collisionBoxes,
		int collisionCount
	);

	//마린, 총알, 총구 화염 출력
	void Render(
		float cameraX,
		float cameraY,
		float cameraZoom
	);
	//총알과 적 충돌 검사
	int CheckBulletHit(
		float x,
		float y,
		float width,
		float height
	);
private:
	//마린과 지형 충돌 검사
	bool CheckCollision(
		float x,
		float y,
		CollisionBox* collisionBoxes,
		int collisionCount
	);

	//총알과 지형 충돌 검사
	bool CheckBulletCollision(
		float x,
		float y,
		CollisionBox* collisionBoxes,
		int collisionCount
	);

	//마우스 위치에 따라 8방향 결정
	void UpdateDirection(
		float cameraX,
		float cameraY,
		float cameraZoom
	);

	//마우스 방향으로 총알 생성
	void ShootBullet(
		float cameraX,
		float cameraY,
		float cameraZoom
	);

	//현재 방향의 실제 총구 위치 계산
	void GetMuzzlePosition(
		float& muzzleX,
		float& muzzleY
	);

private:
	//8방향마다 2프레임씩 총 16장
	int m_txMove[16];

	//총알 이미지
	int m_txBullet = -1;

	//총구 화염 이미지
	int m_txMuzzleFlash = -1;

	//현재 바라보는 방향
	//0 아래
	//1 왼쪽 아래
	//2 왼쪽
	//3 왼쪽 위
	//4 위
	//5 오른쪽 위
	//6 오른쪽
	//7 오른쪽 아래
	int m_facingDir = 0;

	//이동 애니메이션 프레임
	int m_moveFrame = 0;

	//현재 이동 중인지 확인
	bool m_isMoving = false;

	//이동 애니메이션 시간
	unsigned long long m_lastMoveAnimTime = 0;

	//마지막 총알 발사 시간
	unsigned long long m_lastShotTime = 0;

	//연사 간격
	//100ms = 초당 약 10발
	unsigned long long m_fireInterval = 100;

	//마지막으로 발사한 총구 위치
	float m_muzzleFlashX = 0.0f;
	float m_muzzleFlashY = 0.0f;

	//마린 이동 속도
	//초당 이동 거리
	float m_moveSpeed = 150.0f;

	//마린 충돌 크기
	float m_collisionW = 35.0f;
	float m_collisionH = 35.0f;

	//마린 출력 크기
	float m_renderScale = 0.14f;

	//현재 존재하는 총알
	std::vector<Bullet> m_bullets;
};