#include "ScenePlay.h"
#include "GameInput.h"
#include "App.h"
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <windows.h>

ScenePlay::ScenePlay()
{
	m_message = "Scene Play";
}

int ScenePlay::Update()
{
	// =========================
	// 플레이어 이동
	// =========================

	// 이동하려는 다음 위치
	float nextX = m_playerX;
	float nextY = m_playerY;


	// A / D 먼저 계산
	if (GetGameInput()->GetKeyState('A'))
	{
		nextX -= m_playerSpeed;
	}

	if (GetGameInput()->GetKeyState('D'))
	{
		nextX += m_playerSpeed;
	}


	// X 방향에 벽이 없으면 이동
	if (!CheckCollision(nextX, m_playerY))
	{
		m_playerX = nextX;
	}


	// 다시 현재 위치 기준
	nextY = m_playerY;


	// W / S 계산
	if (GetGameInput()->GetKeyState('W'))
	{
		nextY -= m_playerSpeed;
	}

	if (GetGameInput()->GetKeyState('S'))
	{
		nextY += m_playerSpeed;
	}


	// Y 방향에 벽이 없으면 이동
	if (!CheckCollision(m_playerX, nextY))
	{
		m_playerY = nextY;
	}

	// =========================
	// 이동 애니메이션
	// =========================

	unsigned long long currentTime =
		GetTickCount64();


	if (m_isMoving)
	{
		// 약 0.18초마다 프레임 변경
		if (currentTime - m_lastMoveAnimTime > 180)
		{
			m_moveFrame++;

			if (m_moveFrame >= 2)
			{
				m_moveFrame = 0;
			}

			m_lastMoveAnimTime = currentTime;
		}
	}
	else
	{
		// 멈춰있으면 첫 번째 프레임
		m_moveFrame = 0;
	}

	// =========================
	// 마린이 맵 밖으로 못 나가게
	// =========================

	if (m_playerX < 0.0f)
		m_playerX = 0.0f;

	if (m_playerY < 0.0f)
		m_playerY = 0.0f;

	if (m_playerX > m_mapWidth - 30.0f)
		m_playerX = m_mapWidth - 30.0f;

	if (m_playerY > m_mapHeight - 30.0f)
		m_playerY = m_mapHeight - 30.0f;

	// =========================
	// 카메라 계산
	// =========================

	float screenWidth =
		(float)g2_GetScnW();

	float screenHeight =
		(float)g2_GetScnH();


	// 확대된 상태에서 실제로 보이는 월드 영역
	float cameraViewWidth =
		screenWidth / m_cameraZoom;

	float cameraViewHeight =
		screenHeight / m_cameraZoom;


	// 플레이어를 화면 중앙에 두도록 카메라 위치 계산
	m_cameraX =
		m_playerX - cameraViewWidth / 2.0f;

	m_cameraY =
		m_playerY - cameraViewHeight / 2.0f;


	// 카메라가 움직일 수 있는 최대 좌표
	float maxCameraX =
		m_mapWidth - cameraViewWidth;

	float maxCameraY =
		m_mapHeight - cameraViewHeight;


	// 혹시 맵이 화면보다 작은 경우
	if (maxCameraX < 0.0f)
		maxCameraX = 0.0f;

	if (maxCameraY < 0.0f)
		maxCameraY = 0.0f;


	// 왼쪽 / 위 제한
	if (m_cameraX < 0.0f)
		m_cameraX = 0.0f;

	if (m_cameraY < 0.0f)
		m_cameraY = 0.0f;


	// 오른쪽 / 아래 제한
	if (m_cameraX > maxCameraX)
		m_cameraX = maxCameraX;

	if (m_cameraY > maxCameraY)
		m_cameraY = maxCameraY;

	UpdateMarineDirection();
	// =========================
	// 좌클릭 사격
	// =========================

	if (
		GetGameInput()->GetMouseEvent(0)
		== EInput::EINPUT_DOWN
		)
	{
		ShootBullet();
	}

	// =========================
	// 총알 이동
	// =========================

	for (int i = 0; i < (int)m_bullets.size(); i++)
	{
		m_bullets[i].x +=
			m_bullets[i].dirX *
			m_bullets[i].speed;

		m_bullets[i].y +=
			m_bullets[i].dirY *
			m_bullets[i].speed;
	}

	// =========================
// 총알 제거
// =========================

	for (int i = (int)m_bullets.size() - 1; i >= 0; i--)
	{
		bool removeBullet = false;


		// 맵 밖
		if (
			m_bullets[i].x < 0 ||
			m_bullets[i].y < 0 ||
			m_bullets[i].x > m_mapWidth ||
			m_bullets[i].y > m_mapHeight
			)
		{
			removeBullet = true;
		}


		// 벽 또는 바위
		if (
			CheckBulletCollision(
				m_bullets[i].x,
				m_bullets[i].y
			)
			)
		{
			removeBullet = true;
		}


		if (removeBullet)
		{
			m_bullets.erase(
				m_bullets.begin() + i
			);
		}
	}


	// =========================
	// Stage 1 출구
	// =========================

	if (m_currentStage == 1)
	{
		// 오른쪽 출구에 도착
		if (
			m_playerX > m_mapWidth - 120.0f &&
			m_playerY > 415.0f &&
			m_playerY < 510.0f
			)
		{
			LoadStage(2);

			// 여기서 Update 종료
			// 스테이지 변경 직후 이전 카메라 계산 방지
			return 0;
		}
	}
	// =========================
	// 공격 애니메이션 종료
	// =========================

	if (m_isAttacking)
	{
		if (
			GetTickCount64() - m_attackStartTime
			> 160
			)
		{
			m_isAttacking = false;
		}
	}

	return 0;
}
int ScenePlay::Init()
{
	// =========================
// Marine 이미지 로드
// =========================

	for (int i = 0; i < 16; i++)
	{
		m_txMarineMove[i] = -1;
		m_txMarineAttack[i] = -1;
	}

	char filePath[256];


	// 이동 이미지 16장
	for (int i = 0; i < 16; i++)
	{
		sprintf_s(
			filePath,
			"resource/character/marine/move/move_%02d.png",
			i + 1
		);

		m_txMarineMove[i] =
			g2_TextureLoad(filePath);
	}


	// 공격 이미지 16장
	for (int i = 0; i < 16; i++)
	{
		sprintf_s(
			filePath,
			"resource/character/marine/attack/attack_%02d.png",
			i + 1
		);

		m_txMarineAttack[i] =
			g2_TextureLoad(filePath);
	}


	// 총알 이미지
	m_txBullet =
		g2_TextureLoad(
			"resource/character/marine/bullet/bullet_01.png"
		);


	// 초기 상태
	m_facingDir = 0;
	m_moveFrame = 0;

	m_isMoving = false;
	m_isAttacking = false;

	m_lastMoveAnimTime = GetTickCount64();

	m_bullets.clear();

	// Stage 1 시작
	LoadStage(1);

	return 0;
}

int ScenePlay::Render()
{
	// =========================
	// Stage 1 맵 출력
	// =========================

	{
		VEC2 mapPosition{
	-m_cameraX * m_cameraZoom,
	-m_cameraY * m_cameraZoom
		};

		VEC2 mapScale{
			m_cameraZoom,
			m_cameraZoom
		};

		g2_Draw2D(
			m_txStage1,
			nullptr,
			&mapPosition,
			&mapScale
		);
	}


	// =========================
	// 마린 출력
	// =========================

	{
		// 실제 월드 위치 - 카메라 위치
		// = 화면에서 보이는 위치
		/*VEC2 marinePosition{
			(m_playerX - m_cameraX) * m_cameraZoom,
			(m_playerY - m_cameraY) * m_cameraZoom
		};

		VEC2 marineScale{
			0.14f * m_cameraZoom,
			0.14f * m_cameraZoom
		};*/
		{
			VEC2 marinePosition{
				(m_playerX - m_cameraX)
					* m_cameraZoom,

				(m_playerY - m_cameraY)
					* m_cameraZoom
			};


			VEC2 marineScale{
				0.14f * m_cameraZoom,
				0.14f * m_cameraZoom
			};


			int textureIndex =
				m_facingDir * 2;

			// =========================
// 총알 출력
// =========================

			for (int i = 0; i < (int)m_bullets.size(); i++)
			{
				VEC2 bulletPosition{

					(m_bullets[i].x - m_cameraX)
						* m_cameraZoom,

					(m_bullets[i].y - m_cameraY)
						* m_cameraZoom
				};


				VEC2 bulletScale{
					0.10f * m_cameraZoom,
					0.10f * m_cameraZoom
				};


				g2_Draw2D(
					m_txBullet,
					nullptr,
					&bulletPosition,
					&bulletScale
				);
			}

			// 공격 중
			if (m_isAttacking)
			{
				unsigned long long attackTime =
					GetTickCount64() - m_attackStartTime;


				int attackFrame = 0;

				if (attackTime > 80)
				{
					attackFrame = 1;
				}


				textureIndex += attackFrame;


				g2_Draw2D(
					m_txMarineAttack[textureIndex],
					nullptr,
					&marinePosition,
					&marineScale
				);
			}

			// 평상시
			else
			{
				textureIndex += m_moveFrame;


				g2_Draw2D(
					m_txMarineMove[textureIndex],
					nullptr,
					&marinePosition,
					&marineScale
				);
			}
		}
	}


	return 0;
}

int ScenePlay::Destroy()
{
	// Stage 1 맵 해제
	if (m_txStage1 != -1)
	{
		g2_TextureRelease(m_txStage1);
		m_txStage1 = -1;
	}


	// 마린 이미지 해제
	// Marine 이동/공격 이미지 해제
	for (int i = 0; i < 16; i++)
	{
		if (m_txMarineMove[i] != -1)
		{
			g2_TextureRelease(
				m_txMarineMove[i]
			);

			m_txMarineMove[i] = -1;
		}


		if (m_txMarineAttack[i] != -1)
		{
			g2_TextureRelease(
				m_txMarineAttack[i]
			);

			m_txMarineAttack[i] = -1;
		}
	}


	// 총알
	if (m_txBullet != -1)
	{
		g2_TextureRelease(m_txBullet);

		m_txBullet = -1;
	}


	m_bullets.clear();


	return 0;
}

bool ScenePlay::CheckCollision(float x, float y)
{
	for (int i = 0; i < m_collisionCount; i++)
	{
		CollisionBox& box = m_collisionBoxes[i];

		// 마린과 벽/바위가 겹치는지 확인
		if (
			x < box.x + box.width &&
			x + m_playerCollisionW > box.x &&
			y < box.y + box.height &&
			y + m_playerCollisionH > box.y
			)
		{
			return true;
		}
	}

	return false;
}

void ScenePlay::LoadStage(int stage)
{
	m_currentStage = stage;

	// 기존 맵 이미지가 있으면 해제
	if (m_txStage1 != -1)
	{
		g2_TextureRelease(m_txStage1);
		m_txStage1 = -1;
	}


	// =========================
	// Stage 1
	// =========================
	if (stage == 1)
	{
		m_txStage1 =
			g2_TextureLoad("resource/BG/BG_stage1.png");

		m_playerX = 300.0f;
		m_playerY = 500.0f;
	}


	// =========================
	// Stage 2
	// =========================
	else if (stage == 2)
	{
		m_txStage1 =
			g2_TextureLoad("resource/BG/BG_stage2.png");

		// Stage 2 시작 위치
		// 왼쪽에서 시작
		m_playerX = 180.0f;
		m_playerY = 470.0f;
	}


	// 새 맵의 실제 크기 읽기
	m_mapWidth =
		(float)g2_TextureWidth(m_txStage1);

	m_mapHeight =
		(float)g2_TextureHeight(m_txStage1);


	// 카메라 초기화
	m_cameraX = 0.0f;
	m_cameraY = 0.0f;


	// 일단 이전 스테이지 충돌박스 제거
	// Stage 2 충돌은 다음 단계에서 따로 넣음
	m_collisionCount = 0;


	// =========================
	// Stage 1 충돌박스
	// =========================
	if (stage == 1)
	{
		// 위쪽 벽
		m_collisionBoxes[m_collisionCount++] =
		{ 0.0f, 0.0f, m_mapWidth, 145.0f };

		// 왼쪽 벽
		m_collisionBoxes[m_collisionCount++] =
		{ 0.0f, 0.0f, 110.0f, m_mapHeight };

		// 아래쪽 벽
		m_collisionBoxes[m_collisionCount++] =
		{ 0.0f, 760.0f, m_mapWidth, 181.0f };

		// 오른쪽 위 벽
		m_collisionBoxes[m_collisionCount++] =
		{ 1505.0f, 0.0f, 167.0f, 420.0f };

		// 오른쪽 아래 벽
		m_collisionBoxes[m_collisionCount++] =
		{ 1505.0f, 500.0f, 167.0f, 441.0f };


		// 배럭
		m_collisionBoxes[m_collisionCount++] =
		{ 205.0f, 110.0f, 300.0f, 270.0f };


		// 바위들
		m_collisionBoxes[m_collisionCount++] =
		{ 435.0f, 430.0f, 105.0f, 60.0f };

		m_collisionBoxes[m_collisionCount++] =
		{ 1035.0f, 310.0f, 105.0f, 55.0f };

		m_collisionBoxes[m_collisionCount++] =
		{ 1195.0f, 565.0f, 125.0f, 50.0f };

		m_collisionBoxes[m_collisionCount++] =
		{ 465.0f, 690.0f, 85.0f, 45.0f };

		m_collisionBoxes[m_collisionCount++] =
		{ 275.0f, 605.0f, 50.0f, 30.0f };

		m_collisionBoxes[m_collisionCount++] =
		{ 1400.0f, 730.0f, 75.0f, 40.0f };
	}
}

void ScenePlay::UpdateMarineDirection()
{
	// 마우스 화면 좌표
	VEC3i mousePos =
		GetGameInput()->MousePosition();


	// 마린의 화면상 중심 위치
	float playerScreenX =
		(m_playerX - m_cameraX) * m_cameraZoom
		+ (m_playerCollisionW * 0.5f * m_cameraZoom);

	float playerScreenY =
		(m_playerY - m_cameraY) * m_cameraZoom
		+ (m_playerCollisionH * 0.5f * m_cameraZoom);


	// 마린 → 마우스
	float dx =
		(float)mousePos.x - playerScreenX;

	float dy =
		(float)mousePos.y - playerScreenY;


	// atan2 결과를 각도로 변경
	float angle =
		atan2f(dy, dx) * 180.0f / 3.14159265f;


	// =========================
	// 8방향 결정
	// =========================

	// 오른쪽
	if (angle >= -22.5f && angle < 22.5f)
	{
		m_facingDir = 2;
	}

	// 오른쪽 아래
	else if (angle >= 22.5f && angle < 67.5f)
	{
		m_facingDir = 1;
	}

	// 아래
	else if (angle >= 67.5f && angle < 112.5f)
	{
		m_facingDir = 0;
	}

	// 왼쪽 아래
	else if (angle >= 112.5f && angle < 157.5f)
	{
		m_facingDir = 7;
	}

	// 왼쪽
	else if (angle >= 157.5f || angle < -157.5f)
	{
		m_facingDir = 6;
	}

	// 왼쪽 위
	else if (angle >= -157.5f && angle < -112.5f)
	{
		m_facingDir = 5;
	}

	// 위
	else if (angle >= -112.5f && angle < -67.5f)
	{
		m_facingDir = 4;
	}

	// 오른쪽 위
	else
	{
		m_facingDir = 3;
	}
}

void ScenePlay::ShootBullet()
{
	VEC3i mousePos =
		GetGameInput()->MousePosition();


	// 마린 화면상 중심
	float playerScreenX =
		(m_playerX - m_cameraX) * m_cameraZoom
		+ (m_playerCollisionW * 0.5f * m_cameraZoom);

	float playerScreenY =
		(m_playerY - m_cameraY) * m_cameraZoom
		+ (m_playerCollisionH * 0.5f * m_cameraZoom);


	// 마우스 방향
	float dx =
		(float)mousePos.x - playerScreenX;

	float dy =
		(float)mousePos.y - playerScreenY;


	float length =
		sqrtf(dx * dx + dy * dy);


	// 마우스가 마린과 거의 같은 위치에 있으면 발사 X
	if (length < 1.0f)
	{
		return;
	}


	// 방향 벡터 정규화
	dx /= length;
	dy /= length;


	Bullet bullet;

	// 마린 중앙에서 총알 시작
	bullet.x =
		m_playerX + m_playerCollisionW * 0.5f;

	bullet.y =
		m_playerY + m_playerCollisionH * 0.5f;

	bullet.dirX = dx;
	bullet.dirY = dy;

	bullet.speed = 12.0f;


	m_bullets.push_back(bullet);


	// 공격 애니메이션 시작
	m_isAttacking = true;

	m_attackStartTime =
		GetTickCount64();
}

bool ScenePlay::CheckBulletCollision(
	float x,
	float y
)
{
	for (
		int i = 0;
		i < m_collisionCount;
		i++
		)
	{
		CollisionBox& box =
			m_collisionBoxes[i];


		if (
			x >= box.x &&
			x <= box.x + box.width &&
			y >= box.y &&
			y <= box.y + box.height
			)
		{
			return true;
		}
	}

	return false;
}
