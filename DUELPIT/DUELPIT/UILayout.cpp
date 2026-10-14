#include "UILayout.h"

UILayout::UILayout(CanvasData&& canvasData)
	: m_canvasData(std::make_unique<CanvasData>(std::move(canvasData)))
{
}

UILayout* UILayout::Create(CanvasData&& canvasData)
{
	return new UILayout(std::move(canvasData));
}
