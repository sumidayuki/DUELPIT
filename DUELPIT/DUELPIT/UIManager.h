#pragma once

class UIManager : public Singleton<UIManager>
{
	friend class Singleton<UIManager>;

private:
	// CanvasのEntityをIDで管理するmap
	std::unordered_map<uint64_t, Entity> m_canvasEntities;
	
	// CanvasIDからオブジェクトのIDでオブジェクトを管理するmap
	std::unordered_map<uint64_t, std::unordered_map<uint64_t, Entity>> m_uiObjects;
	
	// 開いているCanvasを溜めるスタック
	std::vector<Entity> m_canvasStack;

private:
	UIManager() = default;
	~UIManager() = default;

public:
	// キャンバスを追加
	void AddCanvas(Entity entity, uint64_t canvasID);
	
	// キャンバスを取得
	Entity GetCanvas(uint64_t canvasID);
	
	// UIオブジェクトを追加
	void AddUIObject(Entity entity, uint64_t canvasID, uint64_t objectID);
	
	// UIオブジェクトを取得
	Entity GetUIObject(uint64_t canvasID, uint64_t objectID);

	void ShowCanvas(World& world, uint64_t canvasID);
	void HideCanvas(World& world, uint64_t canvasID);

	// 指定したCanvasを開きます
	bool OpenCanvas(World& world, uint64_t canvasID);

	// 開いているCanvasを閉じます
	bool CloseCanvas(World& world);

	// スタックしているCanvasをすべて閉じます
	void CloseAllCanvas(World& world);

	// 現在開いているCanvasを取得します
	Entity GetCurrentCanvas() const;

	bool HasOpenCanvas() const;

	// スタックにキャンバスが入っているかを確認します
	bool IsCanvasOpen(Entity canvas) const;
};