#pragma once

enum class GameMode
{
	Game,
	UI
};

/// <summary>
/// クラス名：Scene
/// 概要：シーンの開始処理・更新処理・描画処理を行う基底クラスです。
/// 用途：このクラスを継承することでシーンをシーンを作成できます。
/// 備考：使用するには、事前にApplicationにシーンを登録しておく必要があります。
/// </summary>
class Scene
{
public:
	bool isStart;

protected:
	std::string m_name;
	GameMode m_gameMode;
	World m_world;

protected:
	virtual bool Load() = 0;
	virtual void Unload() {}// シーンが終了するときの後片付け
	virtual void Start() = 0;// シーンが始まった時
	virtual void Update() = 0;// シーンの更新処理
	virtual void Draw() = 0;// シーンの描画処理

public:
	explicit Scene(const std::string& name, const GameMode& gameMode);

	// デストラクタ
	virtual ~Scene() = default;

	const std::string& GetName() const { return m_name; }
	
	void SetGameMode(const GameMode& gameMode);
	const GameMode& GetGameMode() const { return m_gameMode; }

	World& GetWorld() { return m_world; }

	bool OnLoad();

	void OnUnload();

	void OnStart();

	void OnBeginFrame(UINT frameIndex);

	void OnUpdate();

	void OnDraw();
};