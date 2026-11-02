#include "SceneBegin.h"
#include "GameInput.h"
#include "App.h"
#include <glc2d.h>
#include <windows.h>	

SceneBegin::SceneBegin()
{
	m_message = "SceneBegin";
}


// 메인 화면 초기화
int SceneBegin::Init()
{
	// BG_main.png 이미지 불러오기
	m_txBG = g2_TextureLoad(
		"resource/BG/BG_main.png"
	);

	return 0;
}


int SceneBegin::Update()
{
	// 현재 마우스 위치 가져오기
	VEC3i mousePos = GetGameInput()->MousePosition();

	int mouseX = mousePos.x;
	int mouseY = mousePos.y;


	// 마우스 왼쪽 버튼을 눌렀을 때
	if (GetGameInput()->GetMouseEvent(0) == EInput::EINPUT_DOWN)
	{
		// START 버튼 영역
		if (mouseX >= 170 && mouseX <= 440 &&
			mouseY >= 390 && mouseY <= 465)
		{
			// Lobby Scene으로 이동
			g_pApp->ChangeScene(GAME_PHASE::SCENE_LOBBY);
		}

		// EXIT 버튼 영역
		else if (mouseX >= 180 && mouseX <= 435 &&
			mouseY >= 475 && mouseY <= 545)
		{
			// 프로그램 종료 요청
			PostQuitMessage(0);
		}
	}


	return 0;
}


// 메인 화면 출력
int SceneBegin::Render()
{
	// 원본 이미지 크기 가져오기
	float imageWidth =
		(float)g2_TextureWidth(m_txBG);

	float imageHeight =
		(float)g2_TextureHeight(m_txBG);


	// 현재 게임 화면 크기
	float screenWidth =
		(float)g2_GetScnW();

	float screenHeight =
		(float)g2_GetScnH();


	// 이미지를 화면 크기에 맞추기 위한 비율
	VEC2 scaleBG{
		screenWidth / imageWidth,
		screenHeight / imageHeight
	};


	// 화면 왼쪽 위에서 시작
	VEC2 positionBG{
		0.0f,
		0.0f
	};


	// 배경 이미지 출력
	g2_Draw2D(
		m_txBG,
		nullptr,
		&positionBG,
		&scaleBG
	);


	return 0;
}


// Scene 종료 시 리소스 정리
int SceneBegin::Destroy()
{
	// BG 이미지 메모리에서 제거
	g2_TextureRelease(m_txBG);

	// 유효한 텍스처가 없다는 표시
	m_txBG = -1;

	return 0;
}