#include "GameTime.h"
#include <glc2d.h>

int GameTime::Init()
{
	begintime = g2_TimeGetTime();
	eplapsedTime = {};
	deltaTime = {};

	return 0;
}

int GameTime::Update()
{
	auto currentTime = g2_TimeGetTime();
	auto prevTime = eplapsedTime;

	eplapsedTime = currentTime - begintime;
	deltaTime = eplapsedTime - prevTime;

	return 0;
}