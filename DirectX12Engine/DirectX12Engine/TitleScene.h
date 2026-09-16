#pragma once

class TitleScene : public Scene
{
private:
	float m_time;
	SpriteRenderer* m_titleText;

public:
	TitleScene();
	bool Load() override;
	void Start() override;
	void Update() override;
	void Draw() override;
};