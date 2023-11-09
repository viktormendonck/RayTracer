#pragma once
#include "Scene.h"
class SceneBUNNY : public dae::Scene
{
public:
	SceneBUNNY() = default;
	~SceneBUNNY() override;

	SceneBUNNY(const SceneBUNNY&) = delete;
	SceneBUNNY(SceneBUNNY&&) noexcept = delete;
	SceneBUNNY& operator=(const SceneBUNNY&) = delete;
	SceneBUNNY& operator=(SceneBUNNY&&) noexcept = delete;

	void Initialize() override;
	void Update(dae::Timer* pTimer) override;
private:
	dae::TriangleMesh* m_MeshPtr{};
	const float m_MeshRotationSpeed{ 0.75f };
};

