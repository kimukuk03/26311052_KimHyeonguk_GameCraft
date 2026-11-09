#include "ScenePlay.h"
#include "App.h"
#include "glc2d.h"
#include "ScenePlayResult.h"

ScenePlay::ScenePlay()
{
	m_message = "Scene Play";
}

//플레이 화면 초기화
int ScenePlay::Init()
{
	//마린 준비
	m_player.Init();
	//HP UI용 작은 폰트 생성
	m_fontHp = g2_FontCreate("netmarble Medium", 28);
	//Stage 1부터 시작
	LoadStage(1);

	return 0;
}

//현재 스테이지에 맞는 몬스터 생성
void ScenePlay::SpawnStageMobs()
{
	m_mobCount = 0;

	//Stage 1 저글링
	if (m_currentStage == 1)
	{
		float positions[12][2] =
		{
			{ 650.0f, 220.0f },
			{ 800.0f, 180.0f },
			{ 1200.0f, 200.0f },
			{ 1350.0f, 300.0f },

			{ 700.0f, 380.0f },
			{ 880.0f, 430.0f },
			{ 1100.0f, 470.0f },
			{ 1400.0f, 450.0f },

			{ 700.0f, 600.0f },
			{ 900.0f, 650.0f },
			{ 1100.0f, 700.0f },
			{ 1300.0f, 680.0f }
		};

		for (int i = 0; i < 12; i++)
		{
			m_mobs[i].Init(
				MobType::ZERGLING
			);

			m_mobs[i].SetPosition(
				positions[i][0],
				positions[i][1]
			);
		}

		m_mobCount = 12;
	}

	//Stage 2 히드라
	else if (m_currentStage == 2)
	{
		float positions[8][2] =
		{
			{ 520.0f, 220.0f },
			{ 760.0f, 180.0f },
			{ 1050.0f, 220.0f },
			{ 1320.0f, 260.0f },

			{ 560.0f, 610.0f },
			{ 820.0f, 650.0f },
			{ 1080.0f, 620.0f },
			{ 1340.0f, 580.0f }
		};

		for (int i = 0; i < 8; i++)
		{
			m_mobs[i].Init(
				MobType::HYDRALISK
			);

			m_mobs[i].SetPosition(
				positions[i][0],
				positions[i][1]
			);
		}

		m_mobCount = 8;
	}

	//Stage 3 울트라 보스
	else if (m_currentStage == 3)
	{
		m_mobs[0].Init(
			MobType::ULTRALISK
		);

		m_mobs[0].SetPosition(
			m_mapWidth * 0.68f,
			m_mapHeight * 0.50f
		);

		m_mobCount = 1;
	}
}

//스테이지 불러오기
void ScenePlay::LoadStage(int stage)
{
	//기존 몬스터 상태 정리
	for (int i = 0; i < m_mobCount; i++)
	{
		m_mobs[i].Destroy();
	}

	m_mobCount = 0;

	//기존 맵 해제
	if (m_txMap != -1)
	{
		g2_TextureRelease(
			m_txMap
		);

		m_txMap = -1;
	}

	m_currentStage = stage;

	//Stage 1 맵
	if (stage == 1)
	{
		m_txMap =
			g2_TextureLoad(
				"resource/BG/BG_stage1.png"
			);
	}

	//Stage 2 맵
	else if (stage == 2)
	{
		m_txMap =
			g2_TextureLoad(
				"resource/BG/BG_stage2.png"
			);
	}

	//Stage 3 맵
	else
	{
		m_txMap =
			g2_TextureLoad(
				"resource/BG/BG_stage3.png"
			);
	}

	//현재 맵 실제 크기
	m_mapWidth =
		(float)g2_TextureWidth(
			m_txMap
		);

	m_mapHeight =
		(float)g2_TextureHeight(
			m_txMap
		);

	//새 스테이지 시작 위치
	m_player.SetPosition(
		150.0f,
		m_mapHeight * 0.5f
	);

	//카메라 초기화
	m_cameraX = 0.0f;
	m_cameraY = 0.0f;

	//충돌박스 초기화
	m_collisionCount = 0;

	//위쪽 벽
	m_collisionBoxes[m_collisionCount++] =
	{
		0.0f,
		0.0f,
		m_mapWidth,
		80.0f
	};

	//왼쪽 벽
	m_collisionBoxes[m_collisionCount++] =
	{
		0.0f,
		0.0f,
		80.0f,
		m_mapHeight
	};

	//아래쪽 벽
	m_collisionBoxes[m_collisionCount++] =
	{
		0.0f,
		m_mapHeight - 80.0f,
		m_mapWidth,
		80.0f
	};

	//Stage 1과 Stage 2는 오른쪽 출구가 있음
	if (stage < 3)
	{
		float exitCenterY =
			m_mapHeight * 0.5f;

		float exitHalfHeight =
			85.0f;

		//오른쪽 위쪽 벽
		m_collisionBoxes[m_collisionCount++] =
		{
			m_mapWidth - 80.0f,
			0.0f,
			80.0f,
			exitCenterY - exitHalfHeight
		};

		//오른쪽 아래쪽 벽
		m_collisionBoxes[m_collisionCount++] =
		{
			m_mapWidth - 80.0f,
			exitCenterY + exitHalfHeight,
			80.0f,
			m_mapHeight
		};
	}
	else
	{
		//Stage 3은 보스맵이라 출구 없음
		m_collisionBoxes[m_collisionCount++] =
		{
			m_mapWidth - 80.0f,
			0.0f,
			80.0f,
			m_mapHeight
		};
	}

	//Stage 1 전용 장애물
	if (stage == 1)
	{
		//배럭
		m_collisionBoxes[m_collisionCount++] =
		{
			205.0f,
			110.0f,
			300.0f,
			270.0f
		};

		//바위
		m_collisionBoxes[m_collisionCount++] =
		{
			435.0f,
			430.0f,
			105.0f,
			60.0f
		};

		m_collisionBoxes[m_collisionCount++] =
		{
			1035.0f,
			310.0f,
			105.0f,
			55.0f
		};

		m_collisionBoxes[m_collisionCount++] =
		{
			1195.0f,
			565.0f,
			125.0f,
			50.0f
		};
	}

	//Stage 2는 현재 내부 장애물 없이 외곽 벽만 사용

	//현재 스테이지 몬스터 생성
	SpawnStageMobs();
}

int ScenePlay::Update()
{
	//마린 이동
	m_player.UpdateMovement(
		m_collisionBoxes,
		m_collisionCount,
		m_mapWidth,
		m_mapHeight
	);

	//카메라 이동
	UpdateCamera();

	//마린 조준과 사격
	m_player.UpdateCombat(
		m_cameraX,
		m_cameraY,
		m_cameraZoom,
		m_collisionBoxes,
		m_collisionCount
	);

	//프레임 시간
	float deltaTime =
		g_pApp->GetGameTime()->GetDeltaTime()
		/ 1000.0f;

	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	//모든 몬스터 처리
	for (int i = 0; i < m_mobCount; i++)
	{
		//몬스터 이동과 공격
		m_mobs[i].Update(
			&m_player,
			deltaTime,
			m_collisionBoxes,
			m_collisionCount
		);

		//살아있는 몬스터만 총알 피격 처리
		if (!m_mobs[i].IsDead())
		{
			int hitCount =
				m_player.CheckBulletHit(
					m_mobs[i].GetX(),
					m_mobs[i].GetY(),
					m_mobs[i].GetCollisionWidth(),
					m_mobs[i].GetCollisionHeight()
				);

			//총알 한 발당 25 데미지
			if (hitCount > 0)
			{
				m_mobs[i].Damage(
					hitCount * 25
				);
			}
		}
	}

	//마린 체력이 0이면 게임오버
	if (m_player.GetHP() <= 0)
	{
		ScenePlayResult::SetResult(
			false
		);

		g_pApp->ChangeScene(
			GAME_PHASE::SCENE_PLAYRESULT
		);

		return 0;
	}

	//Stage 1과 2는 오른쪽 출구로 다음 스테이지 이동
	if (m_currentStage < 3)
	{
		float exitCenterY =
			m_mapHeight * 0.5f;

		if (
			m_player.GetX()
	> m_mapWidth - 130.0f &&
			m_player.GetY()
				> exitCenterY - 90.0f &&
			m_player.GetY()
			< exitCenterY + 90.0f
			)
		{
			LoadStage(
				m_currentStage + 1
			);

			return 0;
		}
	}

	//Stage 3 보스 처치 확인
	if (
		m_currentStage == 3 &&
		m_mobCount > 0
		)
	{
		bool bossDead =
			m_mobs[0].IsDead();

		//울트라를 죽이면 게임 클리어
		if (bossDead)
		{
			ScenePlayResult::SetResult(
				true
			);

			g_pApp->ChangeScene(
				GAME_PHASE::SCENE_PLAYRESULT
			);

			return 0;
		}
	}

	return 0;
}

//카메라 이동
void ScenePlay::UpdateCamera()
{
	float screenWidth =
		(float)g2_GetScnW();

	float screenHeight =
		(float)g2_GetScnH();

	float cameraViewWidth =
		screenWidth / m_cameraZoom;

	float cameraViewHeight =
		screenHeight / m_cameraZoom;

	//마린을 중심으로 카메라 이동
	m_cameraX =
		m_player.GetX()
		- cameraViewWidth * 0.5f;

	m_cameraY =
		m_player.GetY()
		- cameraViewHeight * 0.5f;

	float maxCameraX =
		m_mapWidth
		- cameraViewWidth;

	float maxCameraY =
		m_mapHeight
		- cameraViewHeight;

	if (maxCameraX < 0.0f)
	{
		maxCameraX = 0.0f;
	}

	if (maxCameraY < 0.0f)
	{
		maxCameraY = 0.0f;
	}

	if (m_cameraX < 0.0f)
	{
		m_cameraX = 0.0f;
	}

	if (m_cameraY < 0.0f)
	{
		m_cameraY = 0.0f;
	}

	if (m_cameraX > maxCameraX)
	{
		m_cameraX = maxCameraX;
	}

	if (m_cameraY > maxCameraY)
	{
		m_cameraY = maxCameraY;
	}
}

//게임 화면 출력
int ScenePlay::Render()
{
	//맵 화면 위치
	VEC2 mapPosition{
		-m_cameraX * m_cameraZoom,
		-m_cameraY * m_cameraZoom
	};

	//맵 확대
	VEC2 mapScale{
		m_cameraZoom,
		m_cameraZoom
	};

	//맵 출력
	g2_Draw2D(
		m_txMap,
		nullptr,
		&mapPosition,
		&mapScale
	);

	//몬스터 출력
	for (int i = 0; i < m_mobCount; i++)
	{
		m_mobs[i].Render(
			m_cameraX,
			m_cameraY,
			m_cameraZoom
		);
	}

	//마린 출력
	m_player.Render(
		m_cameraX,
		m_cameraY,
		m_cameraZoom
	);
	//마린 HP UI 출력
	{
		int hp =
			m_player.GetHP();

		int maxHp =
			m_player.GetMaxHP();

		float hpRatio =
			(float)hp / (float)maxHp;

		//체력에 따라 글자 색 변경
		unsigned long hpColor =
			0xFF00FF00;

		if (hpRatio <= 0.3f)
		{
			hpColor =
				0xFFFF4444;
		}
		else if (hpRatio <= 0.6f)
		{
			hpColor =
				0xFFFFFF00;
		}

		//20칸짜리 체력바
		const int barCount = 20;

		int filledCount =
			(int)(hpRatio * barCount);

		char hpBar[64] = {};

		int index = 0;

		hpBar[index++] = '[';

		for (int i = 0; i < barCount; i++)
		{
			if (i < filledCount)
			{
				hpBar[index++] = '#';
			}
			else
			{
				hpBar[index++] = '-';
			}
		}

		hpBar[index++] = ']';
		hpBar[index] = '\0';

		char hpText[128];

		sprintf_s(
			hpText,
			"HP %s %d / %d",
			hpBar,
			hp,
			maxHp
		);

		//화면 왼쪽 아래에 고정 출력
		g2_FontDrawText(
			m_fontHp,
			{ 20, g2_GetScnH() - 55 },
			hpColor,
			hpText
		);
	}
	return 0;
}

//리소스 해제
int ScenePlay::Destroy()
{
	//몬스터 상태 정리
	for (int i = 0; i < m_mobCount; i++)
	{
		m_mobs[i].Destroy();
	}

	m_mobCount = 0;

	//몬스터 공용 이미지와 사운드 해제
	GameMob::ReleaseSharedResources();

	//마린 해제
	m_player.Destroy();

	//맵 해제
	if (m_txMap != -1)
	{
		g2_TextureRelease(
			m_txMap
		);

		m_txMap = -1;
	}

	return 0;
}