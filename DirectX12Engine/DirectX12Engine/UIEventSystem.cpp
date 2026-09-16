#include "UIEventSystem.h"
#include "InputDeviceType.h"

EventSystem* UIEventSystem::m_eventSystem = nullptr;

void UIEventSystem::SetSelected(Entity entity)
{
	if (!m_eventSystem)
	{
		return;
	}

	m_eventSystem->currentSelected = entity;
}

Entity UIEventSystem::GetSelected() const
{
	if (!m_eventSystem)
	{
		return INVALID_ENTITY;
	}

	return m_eventSystem->currentSelected;
}

void UIEventSystem::RaycastPointer(World& world)
{
	Vector2Int mousePos = Mouse::GetPosition();

	float mx = (float)mousePos.x;
	float my = (float)mousePos.y;

	// ウィンドウを取得
	Windows::NativeWindow* win = Application::GetGameWindow();
	
	// クライアントサイズ
	Vector2Int clientSize = win->GetClientSize();

	if (clientSize.x > 0 && clientSize.y > 0)
	{
		// 表示されている画面内のマウス位置を計算
		mx = mx * (float)Screen::GetWidth() / (float)clientSize.x;
		my = my * (float)Screen::GetHeight() / (float)clientSize.y;
	}

	// ヒット判定
	HitRecord bestHit;

	View<Canvas> canvasView(world);

	for (auto [canvasEntity, canvas] : canvasView)
	{
		// 現時点ではOverlayのみ
		if (canvas.renderMode != RenderMode::ScreenSpaceOverlay)
		{
			continue;
		}

		// 入力対象ではないCanvasは無視
		if (!canvas.interactable)
		{
			continue;
		}

		RaycastChildren(world, canvasEntity, canvas.sortingOrder, mx, my, bestHit);
	}

	// ホバー中
	m_eventSystem->currentHovered = bestHit.entity;

	// 押した時の判定
	bool mouseDown = Mouse::GetButtonState(MouseButton::Left).IsPressed();

	if (mouseDown && bestHit.entity != INVALID_ENTITY)
	{
		m_eventSystem->currentPressed = bestHit.entity;
	}
	else
	{
		m_eventSystem->currentPressed = INVALID_ENTITY;
	}
}

void UIEventSystem::ProcessControllerNavigation(World& world)
{
	const Gamepad* gamepad = InputManager::GetGamepad(0);

	Entity current = m_eventSystem->currentSelected;
	if (current == INVALID_ENTITY)
	{
		return;
	}

	if (!world.IsAlive(current))
	{
		m_eventSystem->currentSelected = INVALID_ENTITY;

		return;
	}

	UIManager* ins = UIManager::GetInstance();
	uint64_t canvasID = HashString(ins->GetCurrentCanvas().name.c_str());

	Button* button = world.GetComponent<Button>(current);

	if (!button)
	{
		return;
	}

	if (button->state == UIButtonState::Disabled)
	{
		return;
	}

	UINavigation* navigation = world.GetComponent<UINavigation>(current);
	if (!navigation)
	{
		return;
	}

	// 上
	if (gamepad->LeftStick().Up().WasPressedThisFrame())
	{
		MoveSelection(world, ins->GetUIObject(canvasID, navigation->up));
		return;
	}
	// 下
	if (gamepad->LeftStick().Down().WasPressedThisFrame())
	{
		MoveSelection(world, ins->GetUIObject(canvasID, navigation->down));
		return;
	}
	// 左
	if (gamepad->LeftStick().Left().WasPressedThisFrame())
	{
		MoveSelection(world, ins->GetUIObject(canvasID, navigation->left));
		return;
	}
	// 右
	if (gamepad->LeftStick().Right().WasPressedThisFrame())
	{
		MoveSelection(world, ins->GetUIObject(canvasID, navigation->right));
		return;
	}
}

void UIEventSystem::RaycastChildren(World& world, Entity parent, int sortingOrder, float mx, float my, HitRecord& bestHit)
{
	Transform* parentTransform = world.GetComponent<Transform>(parent);

	int childCount = TransformAPI::GetChildCount(parentTransform);

	for (int i = 0; i < childCount; ++i)
	{
		Transform* childTransform = TransformAPI::GetChild(parentTransform, i);


		Entity childEntity = childTransform->entity;

		if (!world.IsAlive(childEntity))
		{
			continue;
		}

		RectTransform* rect = world.GetComponent<RectTransform>(childEntity);

		UIGraphic* graphic = world.GetComponent<UIGraphic>(childEntity);

		if (rect && graphic && graphic->isEnabled && graphic->raycastTarget)
		{
			if (mx >= rect->rectMin.x && mx <= rect->rectMax.x && my >= rect->rectMin.y && my <= rect->rectMax.y)
			{
				if (graphic->depth > bestHit.depth ||
					(graphic->depth == bestHit.depth &&
						sortingOrder > bestHit.sortingOrder))
				{
					bestHit.entity = childEntity;
					bestHit.sortingOrder = sortingOrder;
					bestHit.depth = graphic->depth;
				}
			}
		}

		// 子供も再帰的に調べる
		RaycastChildren(world, childEntity, sortingOrder, mx, my, bestHit);
	}
}

void UIEventSystem::MoveSelection(World& world, Entity target)
{
	if (target == INVALID_ENTITY)
	{
		return;
	}

	if (!world.IsAlive(target))
	{
		return;
	}

	UINavigation* navigation = world.GetComponent<UINavigation>(target);
	if (!navigation)
	{
		return;
	}

	Button* button = world.GetComponent<Button>(target);
	if (!button)
	{
		return;
	}

	if (button->state == UIButtonState::Disabled)
	{
		return;
	}

	m_eventSystem->currentSelected = target;
}

void UIEventSystem::Start(World& world)
{
	m_eventSystem = nullptr;
	m_previousInputDeviceType = InputDeviceType::Keyboard_Mouse;

	View<EventSystem> view(world);

	for (auto [entity, eventSystem] : view)
	{
		if (m_eventSystem == nullptr)
		{
			m_eventSystem = &eventSystem;
		}
		else
		{
			// EventSystemは1個だけ
			world.DestroyEntity(entity);
		}
	}

	// イベントシステムの作成
	if (m_eventSystem == nullptr)
	{
		Entity entity = world.CreateEntity("EventSystem", Layers::UI);
		world.AddComponent<EventSystem>(entity, EventSystem{});
		m_eventSystem = world.GetComponent<EventSystem>(entity);
	}
}

void UIEventSystem::Update(World& world)
{
	if (!m_eventSystem)
	{
		return;
	}

	if (SceneManager::GetCurrentScene()->GetGameMode() == GameMode::Game)
	{
		return;
	}

	InputDeviceType currentDevice = InputManager::GetCurrentInputDeviceType();

	// デバイスが切り替わった瞬間
	if (currentDevice != m_previousInputDeviceType)
	{
		if (currentDevice == InputDeviceType::Keyboard_Mouse)
		{
			// マウス操作になったらSelectedを消す
			SetSelected(INVALID_ENTITY);
		}
		else if (currentDevice == InputDeviceType::Gamepad)
		{
			// コントローラーになったら現在のCanvasのfirstSelectedを選択
			UIManager* uiManager = UIManager::GetInstance();

			Entity canvas = uiManager->GetCurrentCanvas();

			if (canvas != INVALID_ENTITY)
			{
				Canvas* canvasData = world.GetComponent<Canvas>(canvas);

				if (canvasData && canvasData->firstSelected != INVALID_ENTITY)
				{
					SetSelected(canvasData->firstSelected);
				}
			}
		}

		m_previousInputDeviceType = currentDevice;
	}

	switch (InputManager::GetCurrentInputDeviceType())
	{
	case InputDeviceType::Keyboard_Mouse:
		
		RaycastPointer(world);
		break;

	case InputDeviceType::Gamepad:

		ProcessControllerNavigation(world);
		break;

	default:
		break;
	}
}
