#pragma once

#include "UINavigation.h"

struct Button;
class UILayout;
struct CanvasData;
struct UIElement;
struct RectOffset;
struct RectTransform;
struct UIGraphic;

class UILayoutImporter : public AssetImporter
{
public:
	UILayoutImporter();
	virtual ~UILayoutImporter() override = default;
	UILayout* Import(const std::wstring path);
	UILayout* Import(const wchar_t* path);
	UILayout* Import();

private:
	void ProcessCanvas(const Json& json, CanvasData& canvasData);
	void ProcessElement(const Json& json, UIElement& element);
	void ProcessChildren(const Json& json, UIElement& element);
	void ProcessNavigation(const Json& json, UINavigation& navigation);
	void ProcessImage(const Json& json, UIElement& element);
	void ProcessPanel(const Json& json, UIElement& element);
	void ProcessButton(const Json& json, UIElement& element);
	void ProcessOnClick(const Json& json, Button& button);
	void ProcessSlider(const Json& json, UIElement& element);
	void ProcessVerticalLayout(const Json& json, UIElement& element);
	Sprite* ProcessSprite(const Json& json);
	void ProcessRectTransform(const Json& json, RectTransform& rect);
	void ProcessRectOffset(const Json& json, RectOffset& rectOffset);
	void ProcessUIGraphic(const Json& json, UIGraphic& graphic);
};