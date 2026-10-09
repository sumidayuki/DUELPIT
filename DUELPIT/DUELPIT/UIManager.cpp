#include "UIManager.h"

void UIManager::AddCanvas(Entity entity, uint64_t canvasID)
{
	m_canvasEntities[canvasID] = entity;
}

Entity UIManager::GetCanvas(uint64_t canvasID)
{
	auto it = m_canvasEntities.find(canvasID);
	if (it != m_canvasEntities.end())
	{
		return it->second;
	}
	return INVALID_ENTITY;
}

void UIManager::AddUIObject(Entity entity, uint64_t canvasID, uint64_t objectID)
{
	m_uiObjects[canvasID][objectID] = entity;
}

Entity UIManager::GetUIObject(uint64_t canvasID, uint64_t objectID)
{
	auto canvasIt = m_uiObjects.find(canvasID);
	if (canvasIt != m_uiObjects.end())
	{
		auto objectIt = canvasIt->second.find(objectID);
		if (objectIt != canvasIt->second.end())
		{
			
			return objectIt->second;
		}
	}
	return INVALID_ENTITY;
}

void UIManager::ShowCanvas(World& world, uint64_t canvasID)
{
	Entity canvas = GetCanvas(canvasID);

	if (canvas == INVALID_ENTITY)
	{
		return;
	}

	UIAPI::SetEnableCanvas(world, canvas, true);
}

void UIManager::HideCanvas(World& world, uint64_t canvasID)
{
	Entity canvas = GetCanvas(canvasID);

	if (canvas == INVALID_ENTITY)
	{
		return;
	}

	UIAPI::SetEnableCanvas(world, canvas, false);
}

bool UIManager::OpenCanvas(World& world, uint64_t canvasID)
{
	Entity canvas = GetCanvas(canvasID);

	if (canvas == INVALID_ENTITY)
	{
		return false;
	}

	Canvas* canvasData = world.GetComponent<Canvas>(canvas);

	if (!canvasData)
	{
		return false;
	}

	if (canvasData->canvasMode == UICanvasMode::HUD)
	{
		return false;
	}

	if (IsCanvasOpen(canvas))
	{
		return false;
	}

	m_canvasStack.push_back(canvas);

	UIAPI::SetEnableCanvas(world, canvas, true);

	if (canvasData->firstSelected != INVALID_ENTITY && 
		InputManager::GetCurrentInputDeviceType() == InputDeviceType::Gamepad)
	{
		UIEventSystem::SetSelected(canvasData->firstSelected);
	}

	if (canvasData->pauseGame)
	{
		SceneManager::GetCurrentScene()->SetGameMode(GameMode::UI);
	}

	return true;
}

bool UIManager::CloseCanvas(World& world)
{
	if (m_canvasStack.empty())
	{
		return false;
	}

	Entity canvas = m_canvasStack.back();

	m_canvasStack.pop_back();

	UIAPI::SetEnableCanvas(world, canvas, false);

	// ‰º‚ÉCanvas‚ªŽc‚Á‚Ä‚¢‚é
	if (!m_canvasStack.empty())
	{
		Entity previousCanvas = m_canvasStack.back();

		Canvas* canvasData = world.GetComponent<Canvas>(previousCanvas);

		if (canvasData && canvasData->firstSelected != INVALID_ENTITY && 
			InputManager::GetCurrentInputDeviceType() == InputDeviceType::Gamepad)
		{
			UIEventSystem::SetSelected(canvasData->firstSelected);
		}
	}
	else
	{
		// UI‘€ì‘ÎÛ‚ª‚È‚­‚È‚Á‚½
		UIEventSystem::SetSelected(INVALID_ENTITY);
		SceneManager::GetCurrentScene()->SetGameMode(GameMode::Game);
	}

	return true;
}

void UIManager::CloseAllCanvas(World& world)
{
	while (!m_canvasStack.empty())
	{
		CloseCanvas(world);
	}
}

Entity UIManager::GetCurrentCanvas() const
{
	if (m_canvasStack.empty())
	{
		return INVALID_ENTITY;
	}

	return m_canvasStack.back();
}

bool UIManager::HasOpenCanvas() const
{
	return !m_canvasStack.empty();
}

bool UIManager::IsCanvasOpen(Entity canvas) const
{
	for (const Entity& entity : m_canvasStack)
	{
		if (entity == canvas)
		{
			return true;
		}
	}

	return false;
}