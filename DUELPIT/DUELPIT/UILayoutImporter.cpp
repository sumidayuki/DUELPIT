#include "UILayoutImporter.h"
#include "ButtonFunctionRegistry.h"
#include "UILayout.h"
#include "UINavigation.h"

static const std::unordered_map<std::string, TextAnchor> TextAnchorMap =
{
	{ "UpperLeft",		TextAnchor::UpperLeft },		// 上左
	{ "UpperCenter",	TextAnchor::UpperCenter },		// 上中央
	{ "UpperRight",		TextAnchor::UpperRight },		// 上右
	{ "MiddleLeft",		TextAnchor::MiddleLeft },		// 中左
	{ "MiddleCenter",	TextAnchor::MiddleCenter },		// 中央
	{ "MiddleRight",	TextAnchor::MiddleRight },		// 中右
	{ "LowerLeft",		TextAnchor::LowerLeft },		// 下左
	{ "LowerCenter",	TextAnchor::LowerCenter },		// 下中央
	{ "LowerRight",		TextAnchor::LowerRight }		// 下右
};

std::unordered_map<std::string, RenderMode> StrToRenderModeMap =
{
	{ "ScreenSpaceOverlay", RenderMode::ScreenSpaceOverlay },
	{ "ScreenSpaceCamera", RenderMode::ScreenSpaceCamera },
	{ "WorldSpace", RenderMode::WorldSpace }
};

std::unordered_map<std::string, UICanvasMode> StrToCanvasModeMap =
{
	{ "HUD", UICanvasMode::HUD },
	{ "Interactive", UICanvasMode::Interactive },
	{ "InteractiveHUD", UICanvasMode::InteractiveHUD }
};

UILayoutImporter::UILayoutImporter()
{
}

UILayout* UILayoutImporter::Import(const std::wstring path)
{
	SetAssetPath(path.c_str());
	return Import();
}

UILayout* UILayoutImporter::Import(const wchar_t* path)
{
	SetAssetPath(path);
	return Import();
}

UILayout* UILayoutImporter::Import()
{
	// パスを取得
	const std::wstring& path = GetAssetPath();
	
	// パスが空なら無効
	if (path.empty())
	{
		return false;
	}

	std::unique_ptr<CanvasData> canvasData = std::make_unique<CanvasData>();

	try
	{
		std::string text = StringUtility::ReadAllText(path);
		Json json = Json::parse(text);

		ProcessCanvas(json, *canvasData);
	}
	catch (const Json::parse_error& e)
	{
		std::ostringstream oss;
		oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";
		OutputDebugStringA(oss.str().c_str());
	}
	catch (const Json::type_error& e)
	{
		std::ostringstream oss;
		oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";
		OutputDebugStringA(oss.str().c_str());
	}
	catch (const Json::other_error& e)
	{
		std::ostringstream oss;
		oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";
		OutputDebugStringA(oss.str().c_str());
	}
	catch (const Json::exception& e)
	{
		std::ostringstream oss;
		oss << "JSON要素の解析中にエラーが発生: " << e.what() << "\n";
		OutputDebugStringA(oss.str().c_str());
	}

	OutputDebugStringA("UILayoutImporter: インポートに成功しました。");

	CanvasData* data = canvasData.release();
	UILayout* layout = UILayout::Create(std::move(*data));

	if (!layout)
	{
		delete data;
		return nullptr;
	}

	return layout;
}

void UILayoutImporter::ProcessCanvas(const Json& json, CanvasData& canvasData)
{
	const Json& canvasJson = json.value("canvas", Json::object());
	
	// レンダーモードを設定
	std::string renderMode = canvasJson.value("renderMode", "ScreenSpaceOverlay");
	canvasData.canvas.renderMode = StrToRenderModeMap.at(renderMode);

	// キャンバスモードを設定
	std::string canvasMode = canvasJson.value("canvasMode", "HUD");
	canvasData.canvas.canvasMode = StrToCanvasModeMap.at(canvasMode);
	
	// 描画順を設定
	canvasData.canvas.sortingOrder = canvasJson.value("sortingOrder", 0);
	
	// このキャンバスを開いているときにpauseするか
	canvasData.canvas.pauseGame = canvasJson.value("pauseGame", true);

	// キャンバスの名前を取得
	canvasData.name = UTF16LEtoUTF8::Convert(GetFileNameWithoutExtension());

	// キャンバス内で描画するUIObjectの作成
	const Json& elements = canvasJson.value("elements", Json::array());
	for (const auto& elementJson : elements)
	{
		UIElement element;
		ProcessElement(elementJson, element);
		canvasData.uiElements.push_back(element);
	}

	// キャンバスを開いた際に選択するオブジェクトを設定
	if (canvasJson.contains("firstSelected") && canvasJson["firstSelected"].is_string())
	{
		const std::string firstSelectedName = canvasJson["firstSelected"].get<std::string>();

		const uint64_t objectID = HashString(firstSelectedName.c_str());

		canvasData.canvas.firstSelected = objectID;
	}
}

void UILayoutImporter::ProcessElement(const Json& json, UIElement& element)
{
	element.name = json.value("name", "UIElement");
	const std::string& type = json["type"].get<std::string>();

	if(json.contains("rectTransform"))
	{
		const Json& rectJson = json.value("rectTransform", Json::object());
		RectTransform rectT;
		ProcessRectTransform(rectJson, rectT);
		element.rectTransform = std::make_unique<RectTransform>(std::move(rectT));
	}

	if(json.contains("graphic"))
	{
		const Json& graphicJson = json.value("graphic", Json::object());
		UIGraphic graphic;
		ProcessUIGraphic(graphicJson, graphic);
		element.graphic = std::make_unique<UIGraphic>(std::move(graphic));
	}

	if (type == "Image")
	{
		ProcessImage(json, element);
	}
	else if (type == "Panel")
	{
		ProcessPanel(json, element);
	}
	else if (type == "Button")
	{
		ProcessButton(json, element);
	}
	else if (type == "Slider")
	{
		ProcessSlider(json, element);
	}
	else if(type == "VerticalLayoutGroup")
	{
		 ProcessVerticalLayout(json, element);
	}
	else
	{
		std::ostringstream oss;
		oss << "不明なUI要素のタイプ: " << type << "\n";
		OutputDebugStringA(oss.str().c_str());
	}

	if (json.contains("navigation"))
	{
		const Json& navJson = json.value("navigation", Json::object());
		UINavigation navigation;
		ProcessNavigation(navJson, navigation);
		element.navigation = std::make_unique<UINavigation>(std::move(navigation));
	}

	if (json.contains("children"))
	{
		const Json& childrenJson = json.value("children", Json::array());
		ProcessChildren(childrenJson, element);
	}
}

void UILayoutImporter::ProcessChildren(const Json& json, UIElement& element)
{
	for (const auto& childJson : json)
	{
		UIElement childElement;
		ProcessElement(childJson, childElement);
		element.children.push_back(std::make_unique<UIElement>(std::move(childElement)));
	}
}

void UILayoutImporter::ProcessNavigation(const Json& json, UINavigation& navigation)
{
	if (json.contains("up"))
	{
		navigation.up = HashString(json.value("up", "").c_str());
	}
	if (json.contains("down"))
	{
		navigation.down = HashString(json.value("down", "").c_str());
	}
	if (json.contains("left"))
	{
		navigation.left = HashString(json.value("left", "").c_str());
	}
	if (json.contains("right"))
	{
		navigation.right = HashString(json.value("right", "").c_str());
	}
}

void UILayoutImporter::ProcessImage(const Json& json, UIElement& element)
{
	const Json& spriteJson = json.value("sprite", Json::object());
	Sprite* sprite = ProcessSprite(spriteJson);

	Image image;
	image.sprite = sprite;

	element.component = std::make_unique<std::variant<std::monostate, Image, Button, Slider, VerticalLayoutGroup>>(std::move(image));
}

void UILayoutImporter::ProcessPanel(const Json& json, UIElement& element)
{

}

void UILayoutImporter::ProcessButton(const Json& json, UIElement& element)
{
	const Json& spriteJson = json.value("sprite", Json::object());
	Sprite* sprite = ProcessSprite(spriteJson);

	std::vector<float> normalColor = json.value("normalColor", std::vector<float>{ 0.8f, 0.8f, 0.8f, 1.0f });
	std::vector<float> hoveredColor = json.value("hoveredColor", std::vector<float>{ 0.9f, 0.9f, 0.9f, 1.0f });
	std::vector<float> selectedColor = json.value("selectedColor", std::vector<float>{ 0.7f, 0.7f, 0.7f, 1.0f });
	std::vector<float> pressedColor = json.value("pressedColor", std::vector<float>{ 0.6f, 0.6f, 0.6f, 1.0f });
	std::vector<float> disabledColor = json.value("disabledColor", std::vector<float>{ 0.5f, 0.5f, 0.5f, 1.0f });

	Button button;
	button.sprite = sprite;
	button.normalColor = Color(normalColor[0], normalColor[1], normalColor[2], normalColor[3]);
	button.hoveredColor = Color(hoveredColor[0], hoveredColor[1], hoveredColor[2], hoveredColor[3]);
	button.selectedColor = Color(selectedColor[0], selectedColor[1], selectedColor[2], selectedColor[3]);
	button.pressedColor = Color(pressedColor[0], pressedColor[1], pressedColor[2], pressedColor[3]);
	button.disabledColor = Color(disabledColor[0], disabledColor[1], disabledColor[2], disabledColor[3]);

	const Json& onClick = json.value("onClick", Json::object());
	ProcessOnClick(onClick, button);

	element.component = std::make_unique<std::variant<std::monostate, Image, Button, Slider, VerticalLayoutGroup>>(std::move(button));
}

void UILayoutImporter::ProcessOnClick(const Json& json, Button& button)
{
	// 関数名を取得
	std::string functionName = json.value("function", "");

	// レジストリから関数を取得
	auto function = ButtonFunctionRegistry::GetInstance()->GetFunction(functionName);

	if (function)
	{
		// 引数を取得
		auto args = json.value("args", Json::object());

		// 関数と引数をラムダ式で実行
		button.onClick = [function, args]()
		{
			function(args);
		};
	}
}

void UILayoutImporter::ProcessSlider(const Json& json, UIElement& element)
{
	// 背景画像と色を取得
	const std::string& background = json.value("background", Json::object());
	Sprite* backgroundSprite = ProcessSprite(background);
	std::vector<float> backgroundColor = json.value("backgroundColor", std::vector<float>{ 1.0f, 1.0f, 1.0f, 1.0f });
	Color bgColor = Color(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);

	// 画像と色を取得
	const std::string& fill = json.value("fill", Json::object());
	Sprite* fillSprite = ProcessSprite(fill);
	std::vector<float> fillColor = json.value("fillColor", std::vector<float>{ 1.0f, 1.0f, 1.0f, 1.0f });
	Color fColor = Color(fillColor[0], fillColor[1], fillColor[2], fillColor[3]);

	float initialValue = json.value("initialValue", 0.5f);

	// Sliderを作成
	Slider slider;
	slider.backgroundSprite = backgroundSprite;
	slider.backgroundColor = bgColor;
	slider.fillSprite = fillSprite;
	slider.fillColor = fColor;
	slider.value = initialValue;
	slider.isLeftToRight = json.value("isLeftToRight", true);

	// UIElementにSliderを設定
	element.component = std::make_unique<std::variant<std::monostate, Image, Button, Slider, VerticalLayoutGroup>>(std::move(slider));
}

void UILayoutImporter::ProcessVerticalLayout(const Json& json, UIElement& element)
{
	float spacing = json.value("spacing", 0.0f);

	const Json& rectOffsetJson = json.value("rectOffset", Json::object());
	RectOffset rectOffset;
	ProcessRectOffset(rectOffsetJson, rectOffset);

	TextAnchor childAlignment = TextAnchor::UpperLeft;
	if (json.contains("textAnchor"))
	{
		std::string textAnchorStr = json.value("textAnchor", "UpperLeft");
		
		auto it = TextAnchorMap.find(textAnchorStr);
		if (it != TextAnchorMap.end())
		{
			childAlignment = it->second;
		}
	}
	
	VerticalLayoutGroup vlg;
	vlg.spacing = spacing;
	vlg.padding = rectOffset;
	vlg.childAlignment = childAlignment;

	element.component = std::make_unique<std::variant<std::monostate, Image, Button, Slider, VerticalLayoutGroup>>(std::move(vlg));
}

Sprite* UILayoutImporter::ProcessSprite(const Json& json)
{
	// スプライトのパスを取得
	std::string source = json.value("source", "");
	if (source == "")
	{
		// パスが設定されていなければ白い画像を設定する
		source = "Assets/white.png";
	}

	// テクスチャを取得
	Texture2D* texture = AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, UTF8toUTF16LE::Convert(source));
	if (!texture)
	{
		// テクスチャが取得できなければ白い画像を設定する
		AssetManager::GetInstance()->GetAsset<Texture2D>(AssetType::Texture, UTF8toUTF16LE::Convert("Assets/white.png"));
	}

	// Rectの作成
	Rect rect;
	std::vector<float> rectValues = json.value("rect", std::vector<float>{ 0.0f, 0.0f, (float)texture->GetWidth(), (float)texture->GetHeight() });
	rect = Rect(rectValues[0], rectValues[1], rectValues[2], rectValues[3]);

	// pivotの作成
	Vector2 pivot;
	std::vector<float> pivotValues = json.value("pivot", std::vector<float>{ 0.5f, 0.5f });
	pivot = Vector2(pivotValues[0], pivotValues[1]);

	// PixcelsPerUnitを設定
	Vector2 pixelsPerUnit;
	std::vector<float> pixelsPerUnitValues = json.value("pixelsPerUnit", std::vector<float>{ 1.0f, 1.0f });
	pixelsPerUnit = Vector2(pixelsPerUnitValues[0], pixelsPerUnitValues[1]);

	return Sprite::Create(texture, rect, pivot, pixelsPerUnit.x, pixelsPerUnit.y);
}

void UILayoutImporter::ProcessRectTransform(const Json& json, RectTransform& rect)
{
	std::vector<float> anchoredPosition = json.value("pos", std::vector<float>{ 0.0f, 0.0f });
	std::vector<float> sizeDelta = json.value("size", std::vector<float>{ 100.0f, 100.0f });
	std::vector<float> anchorMin = json.value("anchorMin", std::vector<float>{ 0.5f, 0.5f });
	std::vector<float> anchorMax = json.value("anchorMax", std::vector<float>{ 0.5f, 0.5f });
	std::vector<float> pivot = json.value("pivot", std::vector<float>{ 0.5f, 0.5f });

	rect.anchoredPosition = Vector2(anchoredPosition[0], anchoredPosition[1]);
	rect.sizeDelta = Vector2(sizeDelta[0], sizeDelta[1]);
	rect.anchorMin = Vector2(anchorMin[0], anchorMin[1]);
	rect.anchorMax = Vector2(anchorMax[0], anchorMax[1]);
	rect.pivot = Vector2(pivot[0], pivot[1]);
}

void UILayoutImporter::ProcessRectOffset(const Json& json, RectOffset& rectOffset)
{
	float left = json.value("left", 0.0f);
	float right = json.value("right", 0.0f);
	float top = json.value("top", 0.0f);
	float bottom = json.value("bottom", 0.0f);

	rectOffset.left = left;
	rectOffset.right = right;
	rectOffset.top = top;
	rectOffset.bottom = bottom;
}

void UILayoutImporter::ProcessUIGraphic(const Json& json, UIGraphic& graphic)
{
	std::vector<float> color = json.value("color", std::vector<float>{ 1.0f, 1.0f, 1.0f, 1.0f });
	graphic.color = Color(color[0], color[1], color[2], color[3]);
	graphic.isEnabled = json.value("isEnabled", true);
	graphic.depth = json.value("depth", 0);
	graphic.raycastTarget = json.value("raycastTarget", false);
}
