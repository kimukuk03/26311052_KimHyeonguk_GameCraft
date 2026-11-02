#pragma once
#include "GameScene.h"

class SceneBegin : public GameScene
{
public:
	SceneBegin();

	// 메인 화면 이미지 불러오기
	int Init() override;

	// 입력 처리
	int Update() override;

	// 메인 화면 출력
	int Render() override;

	// 이미지 메모리 해제
	int Destroy() override;

protected:
	// 메인 화면 배경 이미지의 텍스처 번호
	int m_txBG = -1;
};