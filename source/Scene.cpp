#include "Scene.h"
#include "Utils.h"
#include "Material.h"
#include <iostream>

namespace dae {

#pragma region Base Scene
	//Initialize Scene with Default Solid Color Material (RED)
	Scene::Scene():
		m_Materials({ new Material_SolidColor({1,0,0})})
	{
		m_SphereGeometries.reserve(32);
		m_PlaneGeometries.reserve(32);
		m_TriangleMeshGeometries.reserve(32);
		m_Lights.reserve(32);
	}

	Scene::~Scene()
	{
		for(auto& pMaterial : m_Materials)
		{
			delete pMaterial;
			pMaterial = nullptr;
		}

		m_Materials.clear();
	}

	void dae::Scene::GetClosestHit(const Ray& ray, HitRecord& closestHit) const
	{

		for (int i{}; i < m_SphereGeometries.size(); ++i) 
		{
			HitRecord tempHitRecord;
			GeometryUtils::HitTest_Sphere(m_SphereGeometries[i], ray, tempHitRecord);
			if (tempHitRecord.t < closestHit.t) {
				closestHit = tempHitRecord;
			}
		}
		for (int i{}; i < m_PlaneGeometries.size(); ++i) 
		{
			HitRecord tempHitRecord;
			GeometryUtils::HitTest_Plane(m_PlaneGeometries[i], ray, tempHitRecord);
			if (tempHitRecord.t < closestHit.t) {
				closestHit = tempHitRecord;
			}
		}


	}

	bool Scene::DoesHit(const Ray& ray) const
	{
		
		for (size_t i{}; i < m_SphereGeometries.size(); ++i)
		{
			if (GeometryUtils::DoesHit_Sphere(m_SphereGeometries[i], ray))
			{
				return true;
			}
		}

		for (size_t i{}; i < m_PlaneGeometries.size(); ++i)
		{
			if (GeometryUtils::DoesHit_Plane(m_PlaneGeometries[i], ray))
			{
				return true;
			}
		}

		return false;
		
	}

#pragma region Scene Helpers
	Sphere* Scene::AddSphere(const Vector3& origin, float radius, unsigned char materialIndex)
	{
		Sphere s;
		s.origin = origin;
		s.radius = radius;
		s.materialIndex = materialIndex;

		m_SphereGeometries.emplace_back(s);
		return &m_SphereGeometries.back();
	}

	Plane* Scene::AddPlane(const Vector3& origin, const Vector3& normal, unsigned char materialIndex)
	{
		Plane p;
		p.origin = origin;
		p.normal = normal;
		p.materialIndex = materialIndex;

		m_PlaneGeometries.emplace_back(p);
		return &m_PlaneGeometries.back();
	}

	TriangleMesh* Scene::AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex)
	{
		TriangleMesh m{};
		m.cullMode = cullMode;
		m.materialIndex = materialIndex;

		m_TriangleMeshGeometries.emplace_back(m);
		return &m_TriangleMeshGeometries.back();
	}

	Light* Scene::AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color)
	{
		Light l;
		l.origin = origin;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Point;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	Light* Scene::AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color)
	{
		Light l;
		l.direction = direction;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Directional;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	unsigned char Scene::AddMaterial(Material* pMaterial)
	{
		m_Materials.push_back(pMaterial);
		return static_cast<unsigned char>(m_Materials.size() - 1);
	}
#pragma endregion
#pragma endregion

}
