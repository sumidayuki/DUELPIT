#pragma once

enum class UICanvasMode
{
	HUD,			// HUDなど。ゲーム中に表示する
	Interactive,	// Pause、Inventoryなど。操作する
	InteractiveHUD,	// Pause、Inventoryなど。操作するが、ゲーム中に表示する
};

enum RenderMode
{
	ScreenSpaceOverlay,		// スクリーンスペースオーバーレイ
	ScreenSpaceCamera,		// スクリーンスペースカメラ
	WorldSpace,				// ワールドスペース
};

struct Canvas : IComponentData
{
	RenderMode renderMode = RenderMode::ScreenSpaceOverlay;
	UICanvasMode canvasMode = UICanvasMode::HUD;

	int sortingOrder = 0;	// 描画順序 (同じRenderMode内で、値が小さいほど先に描画されます)

	// 開いている間ゲームを停止するか
	bool pauseGame = false;

	// UI操作開始時に選択するオブジェクトID
	uint64_t firstSelected = -1;
};