#include "GamePlayer.h"
#include "GameInput.h"
#include "App.h"
#include "glc2d.h"

#include <Windows.h>
#include <cmath>
#include <cstdio>

GamePlayer::GamePlayer()
{
	//텍스처 배열 초기화
	for (int i = 0; i < 16; i++)
	{
		m_txMove[i] = -1;
	}
}

int GamePlayer::Init()
{
	char path[256];

	//마린 이동 이미지 로드
	for (int i = 0; i < 16; i++)
	{
		sprintf_s(
			path,
			"resource/character/marine/move/move_%02d.png",
			i + 1
		);

		m_txMove[i] =
			g2_TextureLoad(path);
	}

	//총알 이미지 로드
	m_txBullet =
		g2_TextureLoad(
			"resource/character/marine/bullet/bullet_01.png"
		);

	//총구 화염 이미지 로드
	m_txMuzzleFlash =
		g2_TextureLoad(
			"resource/character/muzzle/muzzle_flash.png"
		);

	//파일명이 muzzle_flash_radial.png인 경우도 확인
	if (m_txMuzzleFlash == -1)
	{
		m_txMuzzleFlash =
			g2_TextureLoad(
				"resource/character/marine/muzzle/muzzle_flash_radial.png"
			);
	}
	//마린 체력 초기화
	m_maxHp = 100;
	m_hp = 100;
	m_facingDir = 0;
	m_moveFrame = 0;
	m_isMoving = false;

	m_lastMoveAnimTime =
		GetTickCount64();

	m_lastShotTime = 0;

	m_muzzleFlashX = 0.0f;
	m_muzzleFlashY = 0.0f;

	m_bullets.clear();
	//마린 체력 초기화
	m_maxHp = 100;
	m_hp = 100;

	return 0;
}

//WASD 이동 처리
void GamePlayer::UpdateMovement(
	CollisionBox* collisionBoxes,
	int collisionCount,
	float mapWidth,
	float mapHeight
)
{
	//프레임 시간을 초 단위로 변환
	float deltaTime =
		g_pApp->GetGameTime()->GetDeltaTime()
		/ 1000.0f;

	//프레임이 순간적으로 너무 길어지는 경우 제한
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	float inputX = 0.0f;
	float inputY = 0.0f;

	//왼쪽 이동
	if (GetGameInput()->GetKeyState('A'))
	{
		inputX -= 1.0f;
	}

	//오른쪽 이동
	if (GetGameInput()->GetKeyState('D'))
	{
		inputX += 1.0f;
	}

	//위쪽 이동
	if (GetGameInput()->GetKeyState('W'))
	{
		inputY -= 1.0f;
	}

	//아래쪽 이동
	if (GetGameInput()->GetKeyState('S'))
	{
		inputY += 1.0f;
	}

	//키 입력이 있는지 확인
	m_isMoving =
		inputX != 0.0f ||
		inputY != 0.0f;

	//대각선 이동이 직선보다 빨라지지 않도록 보정
	if (
		inputX != 0.0f &&
		inputY != 0.0f
		)
	{
		inputX *= 0.707106f;
		inputY *= 0.707106f;
	}

	//이번 프레임 이동 거리
	float moveX =
		inputX *
		m_moveSpeed *
		deltaTime;

	float moveY =
		inputY *
		m_moveSpeed *
		deltaTime;

	//X축 이동 예정 위치
	float nextX =
		m_x + moveX;

	//X축 충돌 검사
	if (!CheckCollision(
		nextX,
		m_y,
		collisionBoxes,
		collisionCount
	))
	{
		m_x = nextX;
	}

	//Y축 이동 예정 위치
	float nextY =
		m_y + moveY;

	//Y축 충돌 검사
	if (!CheckCollision(
		m_x,
		nextY,
		collisionBoxes,
		collisionCount
	))
	{
		m_y = nextY;
	}

	//맵 왼쪽 제한
	if (m_x < 0.0f)
	{
		m_x = 0.0f;
	}

	//맵 위쪽 제한
	if (m_y < 0.0f)
	{
		m_y = 0.0f;
	}

	//맵 오른쪽 제한
	if (m_x > mapWidth - m_collisionW)
	{
		m_x =
			mapWidth - m_collisionW;
	}

	//맵 아래쪽 제한
	if (m_y > mapHeight - m_collisionH)
	{
		m_y =
			mapHeight - m_collisionH;
	}

	unsigned long long currentTime =
		GetTickCount64();

	//이동 중일 때 애니메이션 재생
	if (m_isMoving)
	{
		if (
			currentTime - m_lastMoveAnimTime
	> 180
			)
		{
			m_moveFrame++;

			if (m_moveFrame >= 2)
			{
				m_moveFrame = 0;
			}

			m_lastMoveAnimTime =
				currentTime;
		}
	}
	else
	{
		//정지하면 첫 번째 프레임 사용
		m_moveFrame = 0;
	}
}

//마우스 조준과 사격 처리
void GamePlayer::UpdateCombat(
	float cameraX,
	float cameraY,
	float cameraZoom,
	CollisionBox* collisionBoxes,
	int collisionCount
)
{
	//마우스 방향으로 마린 회전
	UpdateDirection(
		cameraX,
		cameraY,
		cameraZoom
	);

	unsigned long long currentTime =
		GetTickCount64();

	//좌클릭을 누르고 있는 동안 연사
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (
			currentTime - m_lastShotTime
			>= m_fireInterval
			)
		{
			ShootBullet(
				cameraX,
				cameraY,
				cameraZoom
			);

			m_lastShotTime =
				currentTime;
		}
	}

	//프레임 시간을 초 단위로 변환
	float deltaTime =
		g_pApp->GetGameTime()->GetDeltaTime()
		/ 1000.0f;

	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	//모든 총알 이동
	for (int i = 0; i < (int)m_bullets.size(); i++)
	{
		m_bullets[i].x +=
			m_bullets[i].dirX
			* m_bullets[i].speed
			* deltaTime;

		m_bullets[i].y +=
			m_bullets[i].dirY
			* m_bullets[i].speed
			* deltaTime;

		//총알 수명 감소
		m_bullets[i].life -=
			deltaTime;
	}

	//충돌하거나 수명이 끝난 총알 삭제
	for (
		int i = (int)m_bullets.size() - 1;
		i >= 0;
		i--
		)
	{
		bool removeBullet =
			false;

		//지형에 맞은 총알 삭제
		if (
			CheckBulletCollision(
				m_bullets[i].x,
				m_bullets[i].y,
				collisionBoxes,
				collisionCount
			)
			)
		{
			removeBullet = true;
		}

		//수명이 끝난 총알 삭제
		if (m_bullets[i].life <= 0.0f)
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
}

//마우스 방향을 8방향으로 변환
void GamePlayer::UpdateDirection(
	float cameraX,
	float cameraY,
	float cameraZoom
)
{
	VEC3i mousePos =
		GetGameInput()->MousePosition();

	//현재 방향 이미지
	int textureIndex =
		m_facingDir * 2;

	//실제로 그려지는 마린 크기 계산
	float marineWidth =
		(float)g2_TextureWidth(
			m_txMove[textureIndex]
		)
		* m_renderScale;

	float marineHeight =
		(float)g2_TextureHeight(
			m_txMove[textureIndex]
		)
		* m_renderScale;

	//마린의 월드 중심 좌표
	float centerX =
		m_x +
		marineWidth * 0.5f;

	float centerY =
		m_y +
		marineHeight * 0.5f;

	//화면상의 마린 중심 좌표
	float playerScreenX =
		(centerX - cameraX)
		* cameraZoom;

	float playerScreenY =
		(centerY - cameraY)
		* cameraZoom;

	//마린에서 마우스로 향하는 방향
	float dx =
		(float)mousePos.x -
		playerScreenX;

	float dy =
		(float)mousePos.y -
		playerScreenY;

	//각도 계산
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

//현재 방향의 총구 위치 계산
void GamePlayer::GetMuzzlePosition(
	float& muzzleX,
	float& muzzleY
)
{
	//현재 방향 첫 번째 프레임 사용
	int textureIndex =
		m_facingDir * 2;

	float marineWidth =
		(float)g2_TextureWidth(
			m_txMove[textureIndex]
		)
		* m_renderScale;

	float marineHeight =
		(float)g2_TextureHeight(
			m_txMove[textureIndex]
		)
		* m_renderScale;

	//기본 위치
	muzzleX = m_x;
	muzzleY = m_y;

	//아래
	if (m_facingDir == 0)
	{
		muzzleX += marineWidth * 0.50f;
		muzzleY += marineHeight * 0.78f;
	}
	//왼쪽 아래
	else if (m_facingDir == 1)
	{
		muzzleX += marineWidth * 0.24f;
		muzzleY += marineHeight * 0.65f;
	}
	//왼쪽
	else if (m_facingDir == 2)
	{
		muzzleX += marineWidth * 0.10f;
		muzzleY += marineHeight * 0.48f;
	}
	//왼쪽 위
	else if (m_facingDir == 3)
	{
		muzzleX += marineWidth * 0.25f;
		muzzleY += marineHeight * 0.31f;
	}
	//위
	else if (m_facingDir == 4)
	{
		muzzleX += marineWidth * 0.50f;
		muzzleY += marineHeight * 0.17f;
	}
	//오른쪽 위
	else if (m_facingDir == 5)
	{
		muzzleX += marineWidth * 0.75f;
		muzzleY += marineHeight * 0.31f;
	}
	//오른쪽
	else if (m_facingDir == 6)
	{
		muzzleX += marineWidth * 0.90f;
		muzzleY += marineHeight * 0.48f;
	}
	//오른쪽 아래
	else
	{
		muzzleX += marineWidth * 0.76f;
		muzzleY += marineHeight * 0.65f;
	}
}

void GamePlayer::ShootBullet(
	float cameraX,
	float cameraY,
	float cameraZoom
)
{
	VEC3i mousePos =
		GetGameInput()->MousePosition();

	float muzzleX;
	float muzzleY;

	//현재 방향의 총구 위치 가져오기
	GetMuzzlePosition(
		muzzleX,
		muzzleY
	);

	float muzzleScreenX =
		(muzzleX - cameraX)
		* cameraZoom;

	float muzzleScreenY =
		(muzzleY - cameraY)
		* cameraZoom;

	//총구에서 마우스로 향하는 방향
	float dx =
		(float)mousePos.x
		- muzzleScreenX;

	float dy =
		(float)mousePos.y
		- muzzleScreenY;

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

	Bullet bullet;

	//총구 바로 앞에서 총알 생성
	bullet.x =
		muzzleX + dx * 4.0f;

	bullet.y =
		muzzleY + dy * 4.0f;

	bullet.dirX = dx;
	bullet.dirY = dy;

	bullet.speed = 850.0f;
	bullet.life = 2.0f;

	m_bullets.push_back(bullet);

	//총구 화염 위치 저장
	m_muzzleFlashX =
		muzzleX;

	m_muzzleFlashY =
		muzzleY;

	//화염 표시 시작 시간
	m_lastShotTime =
		GetTickCount64();
}

//마린과 장애물 충돌 검사
bool GamePlayer::CheckCollision(
	float x,
	float y,
	CollisionBox* collisionBoxes,
	int collisionCount
)
{
	for (int i = 0; i < collisionCount; i++)
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

//총알과 지형 충돌 검사
bool GamePlayer::CheckBulletCollision(
	float x,
	float y,
	CollisionBox* collisionBoxes,
	int collisionCount
)
{
	//총알 충돌 범위
	float bulletRadius =
		2.0f;

	for (int i = 0; i < collisionCount; i++)
	{
		CollisionBox& box =
			collisionBoxes[i];

		if (
			x + bulletRadius > box.x &&
			x - bulletRadius < box.x + box.width &&
			y + bulletRadius > box.y &&
			y - bulletRadius < box.y + box.height
			)
		{
			return true;
		}
	}

	return false;
}

void GamePlayer::Render(
	float cameraX,
	float cameraY,
	float cameraZoom
)
{
	//총알 출력
	for (int i = 0; i < (int)m_bullets.size(); i++)
	{
		VEC2 bulletPosition{
			(m_bullets[i].x - cameraX)
				* cameraZoom,

			(m_bullets[i].y - cameraY)
				* cameraZoom
		};

		VEC2 bulletScale{
			0.035f * cameraZoom,
			0.035f * cameraZoom
		};

		g2_Draw2D(
			m_txBullet,
			nullptr,
			&bulletPosition,
			&bulletScale
		);
	}

	//마린 위치
	VEC2 marinePosition{
		(m_x - cameraX)
			* cameraZoom,

		(m_y - cameraY)
			* cameraZoom
	};

	VEC2 marineScale{
		m_renderScale
			* cameraZoom,

		m_renderScale
			* cameraZoom
	};

	int textureIndex =
		m_facingDir * 2
		+ m_moveFrame;

	//마린 출력
	g2_Draw2D(
		m_txMove[textureIndex],
		nullptr,
		&marinePosition,
		&marineScale
	);
}

//마린 리소스 해제
int GamePlayer::Destroy()
{
	//마린 이동 이미지 해제
	for (int i = 0; i < 16; i++)
	{
		if (m_txMove[i] != -1)
		{
			g2_TextureRelease(
				m_txMove[i]
			);

			m_txMove[i] = -1;
		}
	}

	//총알 이미지 해제
	if (m_txBullet != -1)
	{
		g2_TextureRelease(
			m_txBullet
		);

		m_txBullet = -1;
	}

	//총구 화염 이미지 해제
	if (m_txMuzzleFlash != -1)
	{
		g2_TextureRelease(
			m_txMuzzleFlash
		);

		m_txMuzzleFlash = -1;
	}

	m_bullets.clear();

	return 0;
}
//총알과 적 충돌 검사
int GamePlayer::CheckBulletHit(
	float x,
	float y,
	float width,
	float height
)
{
	int hitCount = 0;

	//뒤에서부터 검사해야 erase가 안전함
	for (
		int i = (int)m_bullets.size() - 1;
		i >= 0;
		i--
		)
	{
		//총알이 적의 충돌박스 안에 들어왔는지 확인
		if (
			m_bullets[i].x >= x &&
			m_bullets[i].x <= x + width &&
			m_bullets[i].y >= y &&
			m_bullets[i].y <= y + height
			)
		{
			//맞은 총알 삭제
			m_bullets.erase(
				m_bullets.begin() + i
			);

			hitCount++;
		}
	}

	return hitCount;
}