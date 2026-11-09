#pragma once

//맵의 벽, 바위, 건물 충돌 영역
struct CollisionBox
{
	float x;
	float y;
	float width;
	float height;
};

class GameCharacter
{
public:
	GameCharacter();

	//데미지 처리
	virtual int Damage(int damage);

	//대상 공격
	virtual int Attack(
		GameCharacter* target,
		int damage
	);

	//현재 위치 반환
	float GetX() const;
	float GetY() const;

	//현재 체력 반환
	int GetHP() const;

	//최대 체력 반환
	int GetMaxHP() const;

	//위치 설정
	void SetPosition(
		float x,
		float y
	);

protected:
	//월드 위치
	float m_x = 0.0f;
	float m_y = 0.0f;

	//체력
	int m_hp = 100;
	int m_maxHp = 100;
};