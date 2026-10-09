#include "AttackableSystem.h"

void AttackableSystem::Update(World& world)
{
	View<Attackable> view(world);

	for (auto [entity, attackable] : view)
	{
		// Entityの攻撃記録がなければスキップ
		if (attackable.entities.empty())
		{
			continue;
		}

		// 攻撃をしていなければリセット
		if (!attackable.isAttacking)
		{
			attackable.entities.clear();
		}
	}
}