#include "GameCharacter.h"

GameCharacter::GameCharacter()
{
}

//데미지를 받고 남은 체력 반환
int GameCharacter::Damage(int damage)
{
	m_hp -= damage;

	if (m_hp < 0)
	{
		m_hp = 0;
	}

	return m_hp;
}

//대상에게 데미지 적용
int GameCharacter::Attack(
	GameCharacter* target,
	int damage
)
{
	if (target == nullptr)
	{
		return -1;
	}

	return target->Damage(
		damage
	);
}

//X 위치 반환
float GameCharacter::GetX() const
{
	return m_x;
}

//Y 위치 반환
float GameCharacter::GetY() const
{
	return m_y;
}

//현재 체력 반환
int GameCharacter::GetHP() const
{
	return m_hp;
}

//최대 체력 반환
int GameCharacter::GetMaxHP() const
{
	return m_maxHp;
}

//위치 설정
void GameCharacter::SetPosition(
	float x,
	float y
)
{
	m_x = x;
	m_y = y;
}