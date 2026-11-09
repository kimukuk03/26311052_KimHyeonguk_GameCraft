#pragma once

#include "GameScene.h"

class ScenePlayResult : public GameScene
{
public:
	ScenePlayResult();

	int Init() override;
	int Update() override;
	int Render() override;
	int Destroy() override;

	//true면 승리, false면 패배
	static void SetResult(bool isClear);

private:
	//승리 여부
	static bool s_isClear;

	//결과 화면 이미지
	int m_txResult = -1;

	//결과 화면 진입 시간
	unsigned long long m_enterTime = 0;
};