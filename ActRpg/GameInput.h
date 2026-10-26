#pragma once
#include <glc2d.h>

class GameInput
{
public:
	int Init();
	int Update();

public:
	VEC3i MousePosition() const;
	int MouseDelta() const;
	int GetKeyState(KEYCODE keyCode) const;
	int	GetMouseEvent(int nMouse) const ;	// 마우스 이벤트 (0: LButton, 1: RButton, 2: MButton)

protected:
};

extern GameInput* GetGameInput();

