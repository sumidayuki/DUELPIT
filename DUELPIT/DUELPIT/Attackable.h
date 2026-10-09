#pragma once

struct Attackable : IComponentData
{
	bool isAttacking = false;						// 攻撃中か(falseの間はクリアされます)
	float damage = 0.0f;							// ダメージ量(仮)
	DamageType damageType = DamageType::Normal;		// ダメージタイプ
	std::vector<Entity> entities;					// 既に攻撃したEntityの履歴
};