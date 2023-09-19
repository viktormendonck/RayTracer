#pragma once
#include "Scene.h"


class Scene_W1 : public dae::Scene
{
public:
	Scene_W1() = default;
	~Scene_W1() override = default;

	Scene_W1(const Scene_W1&) = delete;
	Scene_W1(Scene_W1&&) noexcept = delete;
	Scene_W1& operator=(const Scene_W1&) = delete;
	Scene_W1& operator=(Scene_W1&&) noexcept = delete;

	void Initialize() override;
};