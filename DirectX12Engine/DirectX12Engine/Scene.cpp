#include "Precompiled.h"

Scene::Scene(const std::string& name, const GameMode& startGameMode)
	: m_name(name)
	, m_gameMode(startGameMode)
	, isStart(false)
{
}

void Scene::SetGameMode(const GameMode& gameMode)
{
	m_gameMode = gameMode;
	if (m_gameMode == GameMode::Game)
	{
		Time::SetTimeScale(1.0f);
	}
	else
	{
		Time::SetTimeScale(0.0f);
	}
}

bool Scene::OnLoad()
{
	if (!m_world.Load(m_world))
	{
		return false;
	}

	return Load();
}

void Scene::OnUnload()
{
	m_world.Unload(m_world);
	m_world.Clear();
	AudioManager::GetInstance()->CleanupFinishedVoices();
	Unload();
}

void Scene::OnStart()
{
	Start();
	m_world.Start(m_world);
}

void Scene::OnBeginFrame(UINT frameIndex)
{
	m_world.BeginFrame(frameIndex);
}

void Scene::OnUpdate()
{
	Update();
	m_world.Update(m_world);
}

void Scene::OnDraw()
{
	Draw();
	m_world.Draw(m_world);
}