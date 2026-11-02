#include "SceneLobby.h"
#include "App.h"
#include "GraphicEngine.h"
#include "GameInput.h"

extern App* g_pApp;

SceneLobby::SceneLobby()
{
}

int SceneLobby::Init()
{
	GameScene::Init();

	m_txLobbyBG = g2_TextureLoad("resource/BG/BG_lobby.png");

	return 0;
}

int SceneLobby::Destroy()
{
	if (m_txLobbyBG != -1)
	{
		g2_TextureRelease(m_txLobbyBG);
		m_txLobbyBG = -1;
	}

	return 0;
}

int SceneLobby::Update()
{
	// 마우스 왼쪽 클릭 시 플레이 화면으로 이동
	if (GetGameInput()->GetMouseEvent(0) == EInput::EINPUT_DOWN)
	{
		g_pApp->ChangeScene(GAME_PHASE::SCENE_PLAY);
	}

	return 0;
}

int SceneLobby::Render()
{
	// =========================
	// 1. 배경 이미지 전체 출력
	// =========================
	{
		float screenW = (float)g2_GetScnW();
		float screenH = (float)g2_GetScnH();

		float imageW = (float)g2_TextureWidth(m_txLobbyBG);
		float imageH = (float)g2_TextureHeight(m_txLobbyBG);

		VEC2 pos{ 0.0f, 0.0f };
		VEC2 scale{ screenW / imageW, screenH / imageH };

		g2_Draw2D(m_txLobbyBG, nullptr, &pos, &scale);
	}

	// =========================
	// 2. 스테이지 제목
	// =========================
	{
		RECT titleRect{ 60, 40, 500, 100 };
		g2_FontDrawText(m_font1, titleRect, 0xFFFFFFFF, "STAGE 1");
	}

	{
		RECT subTitleRect{ 60, 100, 700, 160 };
		g2_FontDrawText(m_font1, subTitleRect, 0xFFFFFFFF, "FORWARD BARRACKS");
	}

	// =========================
	// 3. 시작 안내
	// =========================
	{
		RECT startRect{ 60, 180, 700, 240 };
		g2_FontDrawText(m_font1, startRect, 0xFFFFFF00, "CLICK TO START");
	}

	// =========================
	// 4. 조작법 안내
	// =========================
	{
		RECT guideTitleRect{ 60, 300, 400, 350 };
		g2_FontDrawText(m_font1, guideTitleRect, 0xFFFFFFFF, "[CONTROL]");
	}

	{
		RECT guide1{ 60, 350, 500, 390 };
		g2_FontDrawText(m_font1, guide1, 0xFFFFFFFF, "W A S D  : MOVE");
	}

	{
		RECT guide2{ 60, 390, 500, 430 };
		g2_FontDrawText(m_font1, guide2, 0xFFFFFFFF, "MOUSE    : AIM");
	}

	{
		RECT guide3{ 60, 430, 500, 470 };
		g2_FontDrawText(m_font1, guide3, 0xFFFFFFFF, "L-CLICK  : ATTACK");
	}

	{
		RECT guide4{ 60, 470, 500, 510 };
		g2_FontDrawText(m_font1, guide4, 0xFFFFFFFF, "ESC      : EXIT");
	}

	return 0;
}