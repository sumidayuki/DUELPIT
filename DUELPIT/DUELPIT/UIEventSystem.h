#pragma once

class UIEventSystem : public System
{
private:
	struct HitRecord
	{
		Entity entity = INVALID_ENTITY;
		int sortingOrder = -999999;
		int depth = -999999;
	};


	static EventSystem* m_eventSystem;

	InputDeviceType m_previousInputDeviceType;

public:
	static void SetSelected(Entity entity);
	Entity GetSelected() const;

private:
	// マウスポインターのレイキャスト処理
	void RaycastPointer(World& world);

	// 子オブジェクトにもレイキャストする
	void RaycastChildren(World& world, Entity parent, int sortingOrder, float mx, float my, HitRecord& bestHit);
	
	// 選択処理
	void MoveSelection(World& world, Entity target);

	// コントローラーのナビゲーション処理
	void ProcessControllerNavigation(World& world);

public:
	void Start(World& world) override;
	void Update(World& world) override;
};