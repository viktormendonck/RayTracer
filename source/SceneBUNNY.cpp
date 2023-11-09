#include "SceneBUNNY.h"
#include "Material.h"
#include "utils.h"

SceneBUNNY::~SceneBUNNY()
{
}

void SceneBUNNY::Initialize()
{

	m_Camera.origin = { 0.f,3.f,-9.f };
	m_Camera.fovAngle = 45.f;

	//Materials
	const auto matLambert_GrayBlue = AddMaterial(new dae::Material_Lambert({ .49f, 0.57f, 0.57f }, 1.f));
	const auto matLambert_White = AddMaterial(new dae::Material_Lambert(dae::colors::White, 1.f));

	//Planes
	AddPlane(dae::Vector3{ 0.f, 0.f, 10.f }, dae::Vector3{ 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
	AddPlane(dae::Vector3{ 0.f, 0.f, 0.f }, dae::Vector3{ 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
	AddPlane(dae::Vector3{ 0.f, 10.f, 0.f }, dae::Vector3{ 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
	AddPlane(dae::Vector3{ 5.f, 0.f, 0.f }, dae::Vector3{ -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
	AddPlane(dae::Vector3{ -5.f, 0.f, 0.f }, dae::Vector3{ 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT



	////OBJ
	////===
	m_MeshPtr = AddTriangleMesh(dae::TriangleCullMode::BackFaceCulling, matLambert_White);
	//dae::Utils::ParseOBJ("Resources/simple_cube.obj",
	dae::Utils::ParseOBJ("Resources/lowpoly_bunny2.obj",
		m_MeshPtr->positions,
		m_MeshPtr->normals,
		m_MeshPtr->indices);


	m_MeshPtr->Scale({ 2.f,2.f,2.f });
	m_MeshPtr->Translate({ .0f,.0f,0.f });

	m_MeshPtr->UpdateAABB();
	m_MeshPtr->UpdateTransforms();

	//Light
	AddPointLight(dae::Vector3{ 0.f, 5.f, 5.f }, 50.f, dae::ColorRGB{ 1.f, .61f, .45f }); //Backlight
	AddPointLight(dae::Vector3{ -2.5f, 5.f, -5.f }, 70.f, dae::ColorRGB{ 1.f, .8f, .45f }); //Front Light Left
	AddPointLight(dae::Vector3{ 2.5f, 2.5f, -5.f }, 50.f, dae::ColorRGB{ .34f, .47f, .68f });
}
void SceneBUNNY::Update(dae::Timer* pTimer)
{
	Scene::Update(pTimer);

	m_MeshPtr->RotateY(std::sinf(pTimer->GetTotal() * m_MeshRotationSpeed) * 2 * dae::PI);
	m_MeshPtr->UpdateTransforms();
}