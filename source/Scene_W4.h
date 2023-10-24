#pragma once
#include "Scene.h"
class Scene_W4 : public dae::Scene
{
public:
	Scene_W4() = default;
	~Scene_W4() override;

	Scene_W4(const Scene_W4&) = delete;
	Scene_W4(Scene_W4&&) noexcept = delete;
	Scene_W4& operator=(const Scene_W4&) = delete;
	Scene_W4& operator=(Scene_W4&&) noexcept = delete;

	void Initialize() override;
private:
	dae::TriangleMesh* m_MeshPtr{};
};
