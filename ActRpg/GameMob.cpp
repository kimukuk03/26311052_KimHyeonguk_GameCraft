#include "GameMob.h"
#include "glc2d.h"

#include <Windows.h>
#include <cmath>
#include <cstdio>

int GameMob::s_txMove[3][8][4];
int GameMob::s_txProjectile = -1;

int GameMob::s_attackSound[3] =
{
	-1, -1, -1
};

int GameMob::s_deathSound[3] =
{
	-1, -1, -1
};

bool GameMob::s_loaded[3] =
{
	false, false, false
};

GameMob::GameMob()
{
}

void GameMob::LoadSharedResources(MobType type)
{
	int typeIndex =
		(int)type;

	if (s_loaded[typeIndex])
	{
		return;
	}

	const char* directionNames[8] =
	{
		"down",
		"down_left",
		"left",
		"up_left",
		"up",
		"up_right",
		"right",
		"down_right"
	};

	const char* basePath = "";

	if (type == MobType::ZERGLING)
	{
		basePath =
			"resource/character/zergling_split_assets/move";
	}
	else if (type == MobType::HYDRALISK)
	{
		basePath =
			"resource/character/hydralisk/move";
	}
	else
	{
		basePath =
			"resource/character/ultralisk/move";
	}

	char path[256];

	//8방향 x 4프레임 자동 로드
	for (int dir = 0; dir < 8; dir++)
	{
		for (int frame = 0; frame < 4; frame++)
		{
			sprintf_s(
				path,
				"%s/move_%s_%02d.png",
				basePath,
				directionNames[dir],
				frame + 1
			);

			s_txMove[typeIndex][dir][frame] =
				g2_TextureLoad(path);
		}
	}

	//저글링 사운드
	if (type == MobType::ZERGLING)
	{
		s_attackSound[typeIndex] =
			g2_SoundLoad(
				"resource/sound/zergling_attack.wav"
			);

		s_deathSound[typeIndex] =
			g2_SoundLoad(
				"resource/sound/zergling_death.wav"
			);
	}

	//히드라 사운드와 투사체
	else if (type == MobType::HYDRALISK)
	{
		s_attackSound[typeIndex] =
			g2_SoundLoad(
				"resource/sound/hydralisk_attack.wav"
			);

		s_deathSound[typeIndex] =
			g2_SoundLoad(
				"resource/sound/hydralisk_death.wav"
			);

		if (s_txProjectile == -1)
		{
			s_txProjectile =
				g2_TextureLoad(
					"resource/character/hydralisk/projectile/projectile_01.png"
				);
		}
	}

	//울트라 사운드
	else
	{
		s_attackSound[typeIndex] =
			g2_SoundLoad(
				"resource/sound/ultralisk_attack.wav"
			);

		s_deathSound[typeIndex] =
			g2_SoundLoad(
				"resource/sound/ultralisk_death.wav"
			);
	}

	s_loaded[typeIndex] = true;
}

int GameMob::Init(MobType type)
{
	m_type = type;

	LoadSharedResources(type);

	m_projectiles.clear();

	m_isDead = false;

	m_facingDir = 0;
	m_moveFrame = 0;

	m_isMoving = false;

	m_lastMoveAnimTime =
		GetTickCount64();

	m_lastAttackTime = 0;

	//저글링
	if (type == MobType::ZERGLING)
	{
		m_maxHp = 100;
		m_hp = 100;

		m_moveSpeed = 80.0f;

		m_collisionW = 45.0f;
		m_collisionH = 45.0f;

		m_renderScale = 0.30f;

		m_attackType =
			MobAttackType::MELEE;

		m_attackRange = 55.0f;

		m_attackDamage = 10;

		m_attackCooldown = 700;
	}

	//히드라
	else if (type == MobType::HYDRALISK)
	{
		m_maxHp = 180;
		m_hp = 180;

		m_moveSpeed = 60.0f;

		m_collisionW = 55.0f;
		m_collisionH = 55.0f;

		m_renderScale = 0.24f;

		m_attackType =
			MobAttackType::RANGED;

		m_attackRange = 260.0f;

		m_attackDamage = 15;

		m_attackCooldown = 1000;
	}

	//울트라
	else
	{
		m_maxHp = 700;
		m_hp = 700;

		m_moveSpeed = 40.0f;

		m_collisionW = 90.0f;
		m_collisionH = 80.0f;

		m_renderScale = 0.40f;

		m_attackType =
			MobAttackType::MELEE;

		m_attackRange = 90.0f;

		m_attackDamage = 30;

		m_attackCooldown = 1100;
	}

	return 0;
}

void GameMob::Update(
	GameCharacter* target,
	float deltaTime,
	CollisionBox* collisionBoxes,
	int collisionCount
)
{
	if (target == nullptr)
	{
		return;
	}

	//죽었어도 이미 발사된 히드라 탄은 이동
	UpdateProjectiles(
		target,
		deltaTime,
		collisionBoxes,
		collisionCount
	);

	if (m_isDead)
	{
		return;
	}

	float targetX =
		target->GetX() + 17.5f;

	float targetY =
		target->GetY() + 17.5f;

	float mobCenterX =
		m_x + m_collisionW * 0.5f;

	float mobCenterY =
		m_y + m_collisionH * 0.5f;

	float dx =
		targetX - mobCenterX;

	float dy =
		targetY - mobCenterY;

	float distance =
		sqrtf(
			dx * dx +
			dy * dy
		);

	if (distance < 1.0f)
	{
		return;
	}

	//마린 방향으로 바라보기
	UpdateDirection(
		dx,
		dy
	);

	//공격 사거리보다 멀면 추적
	if (distance > m_attackRange)
	{
		m_isMoving = true;

		dx /= distance;
		dy /= distance;

		float moveX =
			dx *
			m_moveSpeed *
			deltaTime;

		float moveY =
			dy *
			m_moveSpeed *
			deltaTime;

		float nextX =
			m_x + moveX;

		if (!CheckCollision(
			nextX,
			m_y,
			collisionBoxes,
			collisionCount
		))
		{
			m_x = nextX;
		}

		float nextY =
			m_y + moveY;

		if (!CheckCollision(
			m_x,
			nextY,
			collisionBoxes,
			collisionCount
		))
		{
			m_y = nextY;
		}
	}
	else
	{
		//사거리 안에서는 멈추고 공격
		m_isMoving = false;

		AttackTarget(
			target,
			targetX,
			targetY
		);
	}

	//이동 애니메이션
	unsigned long long currentTime =
		GetTickCount64();

	if (m_isMoving)
	{
		if (
			currentTime - m_lastMoveAnimTime
		> 120
			)
		{
			m_moveFrame++;

			if (m_moveFrame >= 4)
			{
				m_moveFrame = 0;
			}

			m_lastMoveAnimTime =
				currentTime;
		}
	}
	else
	{
		m_moveFrame = 0;
	}
}

void GameMob::UpdateDirection(
	float dx,
	float dy
)
{
	float angle =
		atan2f(dy, dx)
		* 180.0f
		/ 3.14159265f;

	//오른쪽
	if (
		angle >= -22.5f &&
		angle < 22.5f
		)
	{
		m_facingDir = 6;
	}
	//오른쪽 아래
	else if (
		angle >= 22.5f &&
		angle < 67.5f
		)
	{
		m_facingDir = 7;
	}
	//아래
	else if (
		angle >= 67.5f &&
		angle < 112.5f
		)
	{
		m_facingDir = 0;
	}
	//왼쪽 아래
	else if (
		angle >= 112.5f &&
		angle < 157.5f
		)
	{
		m_facingDir = 1;
	}
	//왼쪽
	else if (
		angle >= 157.5f ||
		angle < -157.5f
		)
	{
		m_facingDir = 2;
	}
	//왼쪽 위
	else if (
		angle >= -157.5f &&
		angle < -112.5f
		)
	{
		m_facingDir = 3;
	}
	//위
	else if (
		angle >= -112.5f &&
		angle < -67.5f
		)
	{
		m_facingDir = 4;
	}
	//오른쪽 위
	else
	{
		m_facingDir = 5;
	}
}

void GameMob::AttackTarget(
	GameCharacter* target,
	float targetX,
	float targetY
)
{
	unsigned long long currentTime =
		GetTickCount64();

	if (
		currentTime - m_lastAttackTime
		< m_attackCooldown
		)
	{
		return;
	}

	int typeIndex =
		(int)m_type;

	//공격 사운드
	if (s_attackSound[typeIndex] != -1)
	{
		g2_SoundPlay(
			s_attackSound[typeIndex],
			false
		);
	}

	//근접 공격
	if (
		m_attackType ==
		MobAttackType::MELEE
		)
	{
		target->Damage(
			m_attackDamage
		);
	}

	//원거리 공격
	else
	{
		ShootProjectile(
			targetX,
			targetY
		);
	}

	m_lastAttackTime =
		currentTime;
}

void GameMob::ShootProjectile(
	float targetX,
	float targetY
)
{
	float startX =
		m_x + m_collisionW * 0.5f;

	float startY =
		m_y + m_collisionH * 0.5f;

	float dx =
		targetX - startX;

	float dy =
		targetY - startY;

	float length =
		sqrtf(
			dx * dx +
			dy * dy
		);

	if (length < 1.0f)
	{
		return;
	}

	dx /= length;
	dy /= length;

	MobProjectile projectile;

	projectile.x = startX;
	projectile.y = startY;

	projectile.dirX = dx;
	projectile.dirY = dy;

	projectile.speed = 320.0f;
	projectile.life = 3.0f;

	m_projectiles.push_back(
		projectile
	);
}

void GameMob::UpdateProjectiles(
	GameCharacter* target,
	float deltaTime,
	CollisionBox* collisionBoxes,
	int collisionCount
)
{
	for (
		int i = (int)m_projectiles.size() - 1;
		i >= 0;
		i--
		)
	{
		MobProjectile& p =
			m_projectiles[i];

		p.x +=
			p.dirX *
			p.speed *
			deltaTime;

		p.y +=
			p.dirY *
			p.speed *
			deltaTime;

		p.life -= deltaTime;

		bool remove =
			false;

		//지형 충돌
		for (
			int j = 0;
			j < collisionCount;
			j++
			)
		{
			CollisionBox& box =
				collisionBoxes[j];

			if (
				p.x >= box.x &&
				p.x <= box.x + box.width &&
				p.y >= box.y &&
				p.y <= box.y + box.height
				)
			{
				remove = true;
				break;
			}
		}

		//마린 충돌
		float playerX =
			target->GetX();

		float playerY =
			target->GetY();

		if (
			p.x >= playerX &&
			p.x <= playerX + 35.0f &&
			p.y >= playerY &&
			p.y <= playerY + 35.0f
			)
		{
			target->Damage(
				m_attackDamage
			);

			remove = true;
		}

		if (p.life <= 0.0f)
		{
			remove = true;
		}

		if (remove)
		{
			m_projectiles.erase(
				m_projectiles.begin() + i
			);
		}
	}
}

bool GameMob::CheckCollision(
	float x,
	float y,
	CollisionBox* collisionBoxes,
	int collisionCount
)
{
	for (
		int i = 0;
		i < collisionCount;
		i++
		)
	{
		CollisionBox& box =
			collisionBoxes[i];

		if (
			x < box.x + box.width &&
			x + m_collisionW > box.x &&
			y < box.y + box.height &&
			y + m_collisionH > box.y
			)
		{
			return true;
		}
	}

	return false;
}

int GameMob::Damage(int damage)
{
	if (m_isDead)
	{
		return 0;
	}

	m_hp -= damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_isDead = true;

		int typeIndex =
			(int)m_type;

		if (
			s_deathSound[typeIndex]
			!= -1
			)
		{
			g2_SoundPlay(
				s_deathSound[typeIndex],
				false
			);
		}
	}

	return m_hp;
}

bool GameMob::IsDead() const
{
	return m_isDead;
}

float GameMob::GetCollisionWidth() const
{
	return m_collisionW;
}

float GameMob::GetCollisionHeight() const
{
	return m_collisionH;
}

void GameMob::Render(
	float cameraX,
	float cameraY,
	float cameraZoom
)
{
	//살아있는 몬스터 출력
	if (!m_isDead)
	{
		VEC2 position{
			(m_x - cameraX)
				* cameraZoom,

			(m_y - cameraY)
				* cameraZoom
		};

		VEC2 scale{
			m_renderScale
				* cameraZoom,

			m_renderScale
				* cameraZoom
		};

		g2_Draw2D(
			s_txMove[(int)m_type]
			[m_facingDir]
			[m_moveFrame],

			nullptr,
			&position,
			&scale
		);
	}

	//히드라 투사체 출력
	if (
		m_type ==
		MobType::HYDRALISK
		)
	{
		for (
			int i = 0;
			i < (int)m_projectiles.size();
			i++
			)
		{
			VEC2 position{
				(m_projectiles[i].x - cameraX)
					* cameraZoom,

				(m_projectiles[i].y - cameraY)
					* cameraZoom
			};

			VEC2 scale{
				0.08f * cameraZoom,
				0.08f * cameraZoom
			};

			g2_Draw2D(
				s_txProjectile,
				nullptr,
				&position,
				&scale
			);
		}
	}
}

int GameMob::Destroy()
{
	m_projectiles.clear();

	return 0;
}

void GameMob::ReleaseSharedResources()
{
	for (
		int type = 0;
		type < 3;
		type++
		)
	{
		if (!s_loaded[type])
		{
			continue;
		}

		for (
			int dir = 0;
			dir < 8;
			dir++
			)
		{
			for (
				int frame = 0;
				frame < 4;
				frame++
				)
			{
				if (
					s_txMove[type][dir][frame]
					!= -1
					)
				{
					g2_TextureRelease(
						s_txMove[type][dir][frame]
					);

					s_txMove[type][dir][frame]
						= -1;
				}
			}
		}

		if (s_attackSound[type] != -1)
		{
			g2_SoundRelease(
				s_attackSound[type]
			);

			s_attackSound[type] =
				-1;
		}

		if (s_deathSound[type] != -1)
		{
			g2_SoundRelease(
				s_deathSound[type]
			);

			s_deathSound[type] =
				-1;
		}

		s_loaded[type] = false;
	}

	if (s_txProjectile != -1)
	{
		g2_TextureRelease(
			s_txProjectile
		);

		s_txProjectile = -1;
	}
}