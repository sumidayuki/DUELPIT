#include "MainScene.h"
#include "SystemList.h"
#include "DebugManager.h"
#include "ArrowSystem.h"
#include "CharacterImporter.h"
#include "UILayoutImporter.h"
#include "AIRuleImporter.h"

MainScene::MainScene()
	:Scene("Main", GameMode::Game) 
{
}

bool MainScene::Load()
{
	CharacterInfoRegistry::CreateSingleton();
	CharacterImporter characterImporter;
	characterImporter.Import();

	AIRuleImporter aiImporter;
	aiImporter.Import();

	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Archer.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Arrow.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/field_1.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Stage/Wall_A.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Warrok.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Stage/Arch_A.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Stage/Tile_A.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Model, L"Assets/Stage/stage.fbx");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/BaseTexture-00.jpg");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/TextureNormal-00.jpg");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/Stage/T_Wall_C.png");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/Stage/T_Wall_N.png");
	AssetManager::GetInstance()->LoadAssetsFromDirectory(AssetType::Texture, L"Assets/Images/InputDevice_Icons/Key_Icons");
	AssetManager::GetInstance()->LoadAssetsFromDirectory(AssetType::Texture, L"Assets/Images/InputDevice_Icons/Gamepad_Icons/xbox");
	AssetManager::GetInstance()->LoadAssetsFromDirectory(AssetType::Texture, L"Assets/Images/InputDevice_Icons/Mouse_Icons");
	AssetManager::GetInstance()->LoadAssetsFromDirectory(AssetType::Texture, L"Assets/Images/Text");
	AssetManager::GetInstance()->LoadAsset(AssetType::Audio, L"Assets/Audio/SE/Archer/se-foot.wav");
	AssetManager::GetInstance()->LoadAsset(AssetType::Audio, L"Assets/Audio/SE/Archer/se-guard.wav");
	AssetManager::GetInstance()->LoadAsset(AssetType::Audio, L"Assets/Audio/SE/Archer/se-kick.wav");
	AssetManager::GetInstance()->LoadAsset(AssetType::Audio, L"Assets/Audio/SE/Archer/se-shageki.wav");
	AssetManager::GetInstance()->LoadAsset(AssetType::Audio, L"Assets/Audio/SE/Archer/se-damage.wav");
	AssetManager::GetInstance()->LoadAsset(AssetType::Audio, L"Assets/Audio/SE/Warrok/se-punch.wav");
	AssetManager::GetInstance()->LoadAsset(AssetType::Audio, L"Assets/Audio/SE/Warrok/se-jump.wav");
	AssetManager::GetInstance()->LoadAssetsFromDirectory(AssetType::Effect, L"Assets/Effects");
	AssetManager::GetInstance()->LoadAssetsFromDirectory(AssetType::UILayout, L"Assets/Json/UI");

	// Worldにシステムを追加
	m_world.AddSystem(std::make_unique<GameManagerSystem>());
	m_world.AddSystem(std::make_unique<StaminaSystem>());
	m_world.AddSystem(std::make_unique<GuardSystem>());
	m_world.AddSystem(std::make_unique<MoveSystem>());
	m_world.AddSystem(std::make_unique<PlayerActionGuideSystem>());
	m_world.AddSystem(std::make_unique<PlayerSystem>());
	m_world.AddSystem(std::make_unique<PlayerCameraSystem>());
	m_world.AddSystem(std::make_unique<BattleCameraSystem>());
	m_world.AddSystem(std::make_unique<AIStateMachineSystem>());
	m_world.AddSystem(std::make_unique<EnemySystem>());
	m_world.AddSystem(std::make_unique<ArrowSystem>());

	UIManager::GetInstance()->ShowCanvas(m_world, HashString("MainSceneUI"));

	return true;
}

void MainScene::Unload()
{
	UIManager::GetInstance()->CloseAllCanvas(m_world);
}

void MainScene::Start()
{
	m_world.CreateWithLight(LightType::Directional, Color(0.75f, 0.80f, 0.88f), 2000, nullptr, Vector3(1000, 250, 1000), Quaternion::Euler(90.0f, 0.0f, 0.0f));

	Entity stage = m_world.CreateWithModel(L"Assets/Stage/stage.fbx", nullptr, Vector3::zero, Quaternion::identity, Layers::Environment);
	Entity floorCol = m_world.CreateCube(5000.0f, 1.0f, 5000.0f, Layers::Environment, Color::white, true);
	
	Entity wallCol1 = m_world.CreateCube(5000.0f, 500.0f, 50.0f, Layers::Environment, Color::white, true);
	Transform* wallCol1Transform = m_world.GetComponent<Transform>(wallCol1);
	wallCol1Transform->position = Vector3(0.0f, 250.0f, 1800.0f);

	Entity wallCol2 = m_world.CreateCube(5000.0f, 500.0f, 50.0f, Layers::Environment, Color::white, true);
	Transform* wallCol2Transform = m_world.GetComponent<Transform>(wallCol2);
	wallCol2Transform->position = Vector3(0.0f, 250.0f, -1800.0f);
	
	Entity wallCol3 = m_world.CreateCube(50.0f, 500.0f, 50000.0f, Layers::Environment, Color::white, true);
	Transform* wallCol3Transform = m_world.GetComponent<Transform>(wallCol3);
	wallCol3Transform->position = Vector3(1400.0f, 250.0f, 0.0f);

	Entity wallCol4 = m_world.CreateCube(50.0f, 500.0f, 5000.0f, Layers::Environment, Color::white, true);
	Transform* wallCol4Transform = m_world.GetComponent<Transform>(wallCol4);
	wallCol4Transform->position = Vector3(-1400.0f, 250.0f, 0.0f);
}

void MainScene::Update()
{
}

void MainScene::Draw()
{
}