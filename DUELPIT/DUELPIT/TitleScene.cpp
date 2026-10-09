#include "TitleScene.h"
#include "SystemList.h"
#include "UILayoutImporter.h"
using namespace UIAPI;

TitleScene::TitleScene()
	: Scene("Title", GameMode::Game)
{
}

bool TitleScene::Load()
{
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/White.png");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/Title.png");
	AssetManager::GetInstance()->LoadAsset(AssetType::Texture, L"Assets/Title_Text.png");
	AssetManager::GetInstance()->LoadAsset(AssetType::Audio, L"Assets/Audio/BGM/title.wav");

	UILayoutImporter importer;
	importer.Import(L"Assets/Json/UI/TitleSceneUI.json", m_world);

	UIManager::GetInstance()->ShowCanvas(m_world, HashString("TitleSceneUI"));

	return true;
}

void TitleScene::Start()
{
	m_time = 0.0f;

	// カメラの作成
	float fov = 60.0f;
	float aspect = (float)Screen::GetWidth() / (float)Screen::GetHeight();
	float nearPlane = 0.1f;
	float farPlane = 5000.0f;

	Entity camera = m_world.CreateCamera3D(fov, aspect, nearPlane, farPlane, CameraClearFlags::SolidColor, Color::black, Vector3(0, 0, -10));

	m_world.AddComponent<AudioListener>(camera, AudioListener{});

	AudioClip* bgm = AssetManager::GetInstance()->GetAsset<AudioClip>(AssetType::Audio, L"Assets/Audio/BGM/title.wav");
	AudioSource bgmSource;
	bgmSource.clip = bgm;
	bgmSource.playOnAwake = true;         // 自動再生
	bgmSource.volume = 0.5f;
	bgmSource.spatialBlend = 0.0f;        // 2D（距離減衰なし）
	//m_world.AddComponent<AudioSource>(m_world.CreateEntity("TitleBGM"), bgmSource);
}

void TitleScene::Update()
{
	// タイトルテキストの点滅
	float alpha = (std::sin(Time::GetTime() * 3.0f) + 1.2f) / 2.0f; // 0から1の範囲で変化

 	Entity text = UIManager::GetInstance()->GetUIObject(HashString("TitleSceneUI"), HashString("Text_Title"));
	m_world.GetComponent<UIGraphic>(text)->color.a = alpha;

	if (m_time >= 5.0f && InputManager::IsAnyKeyPressed())
	{
		SceneManager::ChangeScene("Main");
	}

	m_time += Time::GetDeltaTime();
}

void TitleScene::Draw()
{
}
