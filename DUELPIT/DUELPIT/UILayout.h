#pragma once

struct CanvasData
{
	std::string name;
	Canvas canvas;
	std::vector<UIElement> uiElements;
};

// UI要素を表す構造体
struct UIElement
{
	std::string name;
	std::unique_ptr<UIGraphic> graphic;
	std::unique_ptr<RectTransform> rectTransform;
	std::unique_ptr<std::variant<std::monostate, Image, Button, Slider, VerticalLayoutGroup>> component;
	std::unique_ptr<UINavigation> navigation;
	std::vector<std::unique_ptr<UIElement>> children;
};

// UIレイアウトを表すクラス
class UILayout
{
private:
	std::unique_ptr<CanvasData> m_canvasData;

private:
	UILayout(CanvasData&& canvasData);
	~UILayout() = default;

public:
	static UILayout* Create(CanvasData&& canvasData);
};