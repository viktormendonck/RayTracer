#include "SceneEXTRA.h"
#include "Material.h"
#include "utils.h"


SceneEXTRA::~SceneEXTRA()
{
	//do not delete pointers
	//it is evil :3
}

void SceneEXTRA::Initialize()
{

	m_Camera.origin = { 0.f,3.f,-9.f };
	m_Camera.fovAngle = 45.f;

	//Materials
	const auto matLambert_GrayBlue = AddMaterial(new dae::Material_Lambert({ .3f, 0.3f, 0.57f },1.f));
	const auto matLambert_White = AddMaterial(new dae::Material_Lambert({.5f,.2f,.2f}, .75f));
	const auto matLambert_White2 = AddMaterial(new dae::Material_Lambert({.5f,.1f,.25f}, .75f));
	

	//Planes
	AddPlane(dae::Vector3{ 0.f, 0.f, 10.f }, dae::Vector3{ 0.f, 0.f, -1.f }, matLambert_GrayBlue); //BACK
	AddPlane(dae::Vector3{ 0.f, 0.f, 0.f }, dae::Vector3{ 0.f, 1.f, 0.f }, matLambert_GrayBlue); //BOTTOM
	AddPlane(dae::Vector3{ 0.f, 10.f, 0.f }, dae::Vector3{ 0.f, -1.f, 0.f }, matLambert_GrayBlue); //TOP
	AddPlane(dae::Vector3{ 5.f, 0.f, 0.f }, dae::Vector3{ -1.f, 0.f, 0.f }, matLambert_GrayBlue); //RIGHT
	AddPlane(dae::Vector3{ -5.f, 0.f, 0.f }, dae::Vector3{ 1.f, 0.f, 0.f }, matLambert_GrayBlue); //LEFT

	
	m_MeshPtr = AddTriangleMesh(dae::TriangleCullMode::BackFaceCulling, matLambert_White);
	dae::Utils::ParseOBJ("Resources/apex_logo.obj",
		m_MeshPtr->positions,
		m_MeshPtr->normals,
		m_MeshPtr->indices);
	
	
	m_MeshPtr->Scale({ .04f,.04f,.04f });
	m_MeshPtr->Translate({ -2.5f,2.5f,0.f });
	m_MeshPtr->RotateY((M_PI / 3) * 2.5f);
	
	m_MeshPtr->UpdateAABB();
	m_MeshPtr->UpdateTransforms();

	m_Mesh2Ptr = AddTriangleMesh(dae::TriangleCullMode::BackFaceCulling, matLambert_White2);
	dae::Utils::ParseOBJ("Resources/apex_logo.obj",
		m_Mesh2Ptr->positions,
		m_Mesh2Ptr->normals,
		m_Mesh2Ptr->indices);


	m_Mesh2Ptr->Scale({ .04f,.04f,.04f });
	m_Mesh2Ptr->Translate({ 2.5,2.5f,0.f });
	m_Mesh2Ptr->RotateY((- M_PI / 3) * 2.5f);
		  
	m_Mesh2Ptr->UpdateAABB();
	m_Mesh2Ptr->UpdateTransforms();

	


	//Light
	AddPointLight(dae::Vector3{ 0.f, 5.f, 5.f }, 50.f, dae::ColorRGB{ .4f, .2f, .2f }); //Backlight
	AddPointLight(dae::Vector3{ -2.5f, 5.f, -5.f }, 70.f, dae::ColorRGB{ 1.f, .2f, .2f }); //Front Light Left
	AddPointLight(dae::Vector3{ 2.5f, 1.f, -5.f }, 80.f, dae::ColorRGB{ .5f, .08f, .5f });
	// 
	// white light for color checks
	//AddPointLight(dae::Vector3{ 0.f, 5.f, 5.f }, 50.f, dae::ColorRGB{ .8f, .8f, .8f }); //Backlight
	//AddPointLight(dae::Vector3{ -2.5f, 5.f, -5.f }, 70.f, dae::ColorRGB{ .8f, .8f, .8f }); //Front Light Left
	//AddPointLight(dae::Vector3{ 2.5f, 1.f, -5.f }, 80.f, dae::ColorRGB{ .8f, .8f, .8f });
}

void SceneEXTRA::Update(dae::Timer* pTimer)
{
	Scene::Update(pTimer);

	//m_MeshPtr->RotateY(std::sinf(pTimer->GetTotal() * m_MeshRotationSpeed) * 2 * dae::PI);
	//m_MeshPtr->UpdateTransforms();
}

