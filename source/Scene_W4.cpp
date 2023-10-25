#include "Scene_W4.h"
#include "Material.h"
#include "utils.h"


Scene_W4::~Scene_W4()
{
	//do not delete pointers
	//it is evil :3
}

void Scene_W4::Initialize()
{
	
	m_Camera.origin = { 0.f,1.f,-5.f };
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

	//Triangle (Temp)
	//===============
	//auto triangle = dae::Triangle{ {-.75f,.5f,.0f},{-.75f,2.f, .0f}, {.75f,.5f,0.f} };
	//triangle.cullMode = dae::TriangleCullMode::NoCulling;
	//triangle.materialIndex = matLambert_White;
	//
	//m_TriangleGeometries.emplace_back(triangle);

	//Triangle Mesh
	//=============
	//m_MeshPtr = AddTriangleMesh(dae::TriangleCullMode::NoCulling, matLambert_White);
	//m_MeshPtr->positions = {
	//	{-.75f,-1.f, 0.f},   //V0
	//	{-.75f, 1.f, 0.f},   //V1
	//	{ .75f, 1.f, 1.f },  //V2 
	//	{ .75f,-1.f, 0.f} }; //V3
	//m_MeshPtr->indices = {
	//	0,1,2, //Triangle 1
	//	0,2,3  //Triangle 2
	//};
	//
	//m_MeshPtr->CalculateNormals();
	//
	//m_MeshPtr->Translate({ 0.f,1.5f,0.f });
	//m_MeshPtr->UpdateTransforms();

	////OBJ
	////===
	m_MeshPtr = AddTriangleMesh(dae::TriangleCullMode::BackFaceCulling, matLambert_White);
	//dae::Utils::ParseOBJ("Resources/simple_cube.obj",
	dae::Utils::ParseOBJ("Resources/lowpoly_bunny2.obj",
		m_MeshPtr->positions, 
		m_MeshPtr->normals, 
		m_MeshPtr->indices);

	//No need to Calculate the normals, these are calculated inside the ParseOBJ function

	m_MeshPtr->Scale({ 1.5f,1.5f,1.5f });
	m_MeshPtr->Translate({ .0f,.5f,0.f });

	m_MeshPtr->UpdateTransforms();

	//Light
	AddPointLight(dae::Vector3{ 0.f, 5.f, 5.f }, 150.f, dae::ColorRGB{ 1.f, .61f, .45f }); //Backlight
	AddPointLight(dae::Vector3{ -2.5f, 5.f, -5.f }, 170.f, dae::ColorRGB{ 1.f, .8f, .45f }); //Front Light Left
	AddPointLight(dae::Vector3{ 2.5f, 2.5f, -5.f }, 150.f, dae::ColorRGB{ .34f, .47f, .68f });
}