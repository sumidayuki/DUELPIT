#include "ButtonFunctionRegistry.h"

ButtonFunctionRegistry::ButtonFunctionRegistry()
{
	RegisterFunction(
		"LoadToScene",
		[](const Json& args)
		{
			const std::string& sceneName = args.value("sceneName", SceneManager::GetCurrentScene()->GetName());
			SceneManager::ChangeScene(sceneName);
		}
	);
}

void ButtonFunctionRegistry::RegisterFunction(const std::string& name, const std::function<void(const Json&)>& function)
{
	m_functions.emplace(name, function);
}

std::function<void(const Json&)> ButtonFunctionRegistry::GetFunction(const std::string& name) const
{
	auto it = m_functions.find(name);
	if (it != m_functions.end())
	{
		return it->second;
	}

	return nullptr;
}
