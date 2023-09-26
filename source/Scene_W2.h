#pragma once
#include "Scene.h"
class Scene_W2 : public dae::Scene
{
public:
	Scene_W2() = default;
	~Scene_W2() override = default;

	Scene_W2(const Scene_W2&) = delete;
	Scene_W2(Scene_W2&&) noexcept = delete;
	Scene_W2& operator=(const Scene_W2&) = delete;
	Scene_W2& operator=(Scene_W2&&) noexcept = delete;

	void Initialize() override;
};

