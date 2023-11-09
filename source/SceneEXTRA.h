#pragma once
#include "Scene.h"
class SceneEXTRA : public dae::Scene
{
public:
	SceneEXTRA() = default;
	~SceneEXTRA() override;

	SceneEXTRA(const SceneEXTRA&) = delete;
	SceneEXTRA(SceneEXTRA&&) noexcept = delete;
	SceneEXTRA& operator=(const SceneEXTRA&) = delete;
	SceneEXTRA& operator=(SceneEXTRA&&) noexcept = delete;

	void Initialize() override;
	void Update(dae::Timer* pTimer) override;
private:
	//std::vector<dae::TriangleMesh*> m_Meshes{ 2 };
	dae::TriangleMesh* m_MeshPtr{};
	dae::TriangleMesh* m_Mesh2Ptr{};
	const float m_MeshRotationSpeed{ 0.75f };
};


