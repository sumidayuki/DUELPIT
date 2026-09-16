#pragma once

class MainScene : public Scene
{
public:
	MainScene();
	bool Load() override;
	void Start() override;
	void Update() override;
	void Draw() override;
};