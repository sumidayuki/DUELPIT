#pragma once

class MainScene : public Scene
{
public:
	MainScene();
	bool Load() override;
	void Unload() override;
	void Start() override;
	void Update() override;
	void Draw() override;
};