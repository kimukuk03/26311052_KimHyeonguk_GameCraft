#pragma once

#include "GameCharacter.h"
#include <vector>

enum class MobType
{
	ZERGLING,
	HYDRALISK,
	ULTRALISK
};

enum class MobAttackType
{
	MELEE,
	RANGED
};

struct MobProjectile
{
	float x;
	float y;

	float dirX;
	float dirY;

	float speed;
	float life;
};

class GameMob : public GameCharacter
{
public:
	GameMob();

	//몬스터 종류에 맞게 초기화
	int Init(MobType type);

	//개별 몬스터 상태 정리
	int Destroy();

	//공유 리소스 해제
	static void ReleaseSharedResources();

	//추적, 이동, 공격, 투사체 처리
	void Update(
		GameCharacter* target,
		float deltaTime,
		CollisionBox* collisionBoxes,
		int collisionCount
	);

	//몬스터와 투사체 출력
	void Render(
		float cameraX,
		float cameraY,
		float cameraZoom
	);

	//피격 처리
	int Damage(int damage) override;

	//사망 여부
	bool IsDead() const;

	float GetCollisionWidth() const;
	float GetCollisionHeight() const;

private:
	//공유 이미지와 사운드 로드
	static void LoadSharedResources(MobType type);

	//방향 계산
	void UpdateDirection(
		float dx,
		float dy
	);

	//지형 충돌 검사
	bool CheckCollision(
		float x,
		float y,
		CollisionBox* collisionBoxes,
		int collisionCount
	);

	//공격
	void AttackTarget(
		GameCharacter* target,
		float targetX,
		float targetY
	);

	//히드라 투사체 생성
	void ShootProjectile(
		float targetX,
		float targetY
	);

	//히드라 투사체 처리
	void UpdateProjectiles(
		GameCharacter* target,
		float deltaTime,
		CollisionBox* collisionBoxes,
		int collisionCount
	);

private:
	MobType m_type = MobType::ZERGLING;
	MobAttackType m_attackType = MobAttackType::MELEE;

	float m_moveSpeed = 80.0f;

	float m_collisionW = 45.0f;
	float m_collisionH = 45.0f;

	float m_renderScale = 0.30f;

	float m_attackRange = 55.0f;

	int m_attackDamage = 10;

	unsigned long long m_attackCooldown = 700;
	unsigned long long m_lastAttackTime = 0;

	bool m_isDead = false;

	int m_facingDir = 0;
	int m_moveFrame = 0;

	bool m_isMoving = false;

	unsigned long long m_lastMoveAnimTime = 0;

	std::vector<MobProjectile> m_projectiles;

private:
	//종류별 공유 이동 이미지
	static int s_txMove[3][8][4];

	//히드라 투사체
	static int s_txProjectile;

	//종류별 공격 사운드
	static int s_attackSound[3];

	//종류별 사망 사운드
	static int s_deathSound[3];

	//공유 리소스 로드 여부
	static bool s_loaded[3];
};