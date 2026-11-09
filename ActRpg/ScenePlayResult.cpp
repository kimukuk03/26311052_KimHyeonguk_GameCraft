#include "ScenePlayResult.h"
#include "glc2d.h"

#include <Windows.h>

bool ScenePlayResult::s_isClear = false;

ScenePlayResult::ScenePlayResult()
{
}

//승리 또는 패배 결과 설정
void ScenePlayResult::SetResult(bool isClear)
{
	s_isClear = isClear;
}

//결과 화면 초기화
int ScenePlayResult::Init()
{
	//폰트 준비
	GameScene::Init();

	//승리 화면
	if (s_isClear)
	{
		m_txResult =
			g2_TextureLoad(
				"resource/BG/BG_victory.png"
			);

		m_message =
			"VICTORY";
	}
	//패배 화면
	else
	{
		m_txResult =
			g2_TextureLoad(
				"resource/BG/BG_defeat.png"
			);

		m_message =
			"DEFEAT";
	}

	//결과 화면 진입 시간
	m_enterTime =
		GetTickCount64();

	return 0;
}

//결과 화면 입력
int ScenePlayResult::Update()
{
	//이전 입력으로 바로 종료되는 것 방지
	if (
		GetTickCount64() - m_enterTime
		< 500
		)
	{
		return 0;
	}

	//왼쪽 또는 오른쪽 마우스 클릭
	if (
		GetAsyncKeyState(VK_LBUTTON) & 0x8000 ||
		GetAsyncKeyState(VK_RBUTTON) & 0x8000
		)
	{
		PostQuitMessage(0);

		return 0;
	}

	//아무 키 입력
	for (int key = 8; key <= 255; key++)
	{
		if (
			key == VK_LBUTTON ||
			key == VK_RBUTTON ||
			key == VK_MBUTTON
			)
		{
			continue;
		}

		if (
			GetAsyncKeyState(key)
			& 0x8000
			)
		{
			PostQuitMessage(0);

			return 0;
		}
	}

	return 0;
}

//결과 화면 출력
int ScenePlayResult::Render()
{
	if (m_txResult == -1)
	{
		return 0;
	}

	float screenWidth =
		(float)g2_GetScnW();

	float screenHeight =
		(float)g2_GetScnH();

	float imageWidth =
		(float)g2_TextureWidth(
			m_txResult
		);

	float imageHeight =
		(float)g2_TextureHeight(
			m_txResult
		);

	//배경 이미지를 화면 전체에 맞춤
	VEC2 position{
		0.0f,
		0.0f
	};

	VEC2 scale{
		screenWidth / imageWidth,
		screenHeight / imageHeight
	};

	g2_Draw2D(
		m_txResult,
		nullptr,
		&position,
		&scale
	);

	//결과 문구 출력
	if (s_isClear)
	{
		g2_FontDrawText(
			m_font1,
			{ (LONG)(screenWidth * 0.36f), 80 },
			0xFFFFFFFF,
			"VICTORY"
		);
	}
	else
	{
		g2_FontDrawText(
			m_font1,
			{ (LONG)(screenWidth * 0.37f), 80 },
			0xFFFF4444,
			"DEFEAT"
		);
	}

	//종료 안내
	g2_FontDrawText(
		m_font1,
		{
			(LONG)(screenWidth * 0.25f),
			(LONG)(screenHeight - 90.0f)
		},
		0xFFFFFFFF,
		"PRESS ANY KEY TO EXIT"
	);

	return 0;
}

//결과 화면 리소스 해제
int ScenePlayResult::Destroy()
{
	if (m_txResult != -1)
	{
		g2_TextureRelease(
			m_txResult
		);

		m_txResult = -1;
	}

	return 0;
}