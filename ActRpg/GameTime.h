#pragma once

struct GameTime
{
public:
	//게임 시간 초기화
	int Init();

	//매 프레임 시간 갱신
	int Update();

	//프레임 간 경과 시간 반환
	float GetDeltaTime() const
	{
		return (float)deltaTime;
	}

private:
	//게임 시작 시간
	long long begintime{};

	//게임 시작 후 경과 시간
	long long eplapsedTime{};

	//이전 프레임과 현재 프레임 사이 시간
	long long deltaTime{};
};