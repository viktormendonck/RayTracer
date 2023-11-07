//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"
#include <iostream>
#include "execution"
#define PARALLEL_EXECUTION

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
#ifdef PARALLEL_EXECUTION
	int pixelCount{ m_Width * m_Height };
	m_PixelIndices.reserve(pixelCount);
	for (int i{ 0 }; i < pixelCount; ++i)
	{
		m_PixelIndices.emplace_back(i);
	}
#endif

}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	camera.CalculateCameraToWorld();
	float fov{ std::tanf(camera.fovAngle * PI / 180.f / 2.f) };
	float ar{ static_cast<float>(m_Width) / static_cast<float>(m_Height) };

//multithread
#ifdef PARALLEL_EXECUTION
	std::for_each(std::execution::par, m_PixelIndices.begin(), m_PixelIndices.end(), [&](int index)
{
			RenderPixel(pScene, index, fov, ar, camera.cameraToWorld, camera.origin);
});
#else
	int pixelCount{ m_Width * m_Height };
	for (int i{ 0 }; i < pixelCount; ++i)
	{
		RenderPixel(pScene, i, fov, ar, camera.cameraToWorld, camera.origin);
	}
#endif
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float ar, const Matrix cameraToWorld, const Vector3 cameraOrigin) const
{
	Camera& camera = pScene->GetCamera();
	auto materials{ pScene->GetMaterials() };
	auto& lights = pScene->GetLights();

	const uint32_t px{ pixelIndex % m_Width }, py{ pixelIndex / m_Width };

	float rx{ static_cast<float>(px) + 0.5f }, ry{ static_cast<float>(py) + 0.5f };
	float cx{ (2 * (rx / static_cast<float>(m_Width)) - 1) };
	float cy{ (1 - (2 * (ry / static_cast<float>(m_Height)))) * fov };


	float cameraX{ (2.f * ((px + 0.5f) / m_Width) - 1.f) * ar * fov };
	float cameraY{ (1.f - (2.f * (py + 0.5f) / m_Height)) * fov };

	Vector3 rayDir{ (camera.forward + (camera.right * cameraX) + (camera.up * cameraY)) };
	const float magnitude = rayDir.Normalize();

	Ray viewRay{ camera.origin,rayDir };
	ColorRGB finalColor{};
	HitRecord closestHit{};

	pScene->GetClosestHit(viewRay, closestHit);

	if (closestHit.didHit)
	{

		for (int lightIndex{ 0 }; lightIndex < lights.size(); ++lightIndex)
		{
			Vector3 dirToLight{ LightUtils::GetDirectionToLight(lights[lightIndex], closestHit.origin) };
			float lightDist{ dirToLight.Normalize() };
			float lambert = Vector3::Dot(closestHit.normal, dirToLight);


			if (m_ShadowsEnabled)
			{
				Ray shadowRay{ closestHit.origin + closestHit.normal * 0.001f,dirToLight,0.f,lightDist };
				if (pScene->DoesHit(shadowRay))
				{
					continue;
				}
			}

			if (lambert <= 0)
			{
				continue;
			}

			ColorRGB radiance{};
			ColorRGB shade{};

			switch (m_LightingMode) {
			case LightingMode::ObservedArea:
				finalColor += ColorRGB{ 1,1,1 } *lambert;
				break;
			case LightingMode::Radiance:
				finalColor += LightUtils::GetRadiance(lights[lightIndex], closestHit.origin);
				break;
			case LightingMode::BRDF:
				shade = materials[closestHit.materialIndex]->Shade(closestHit, dirToLight, -rayDir);
				finalColor += shade;
				break;
			case LightingMode::Combined:
				radiance = LightUtils::GetRadiance(lights[lightIndex], closestHit.origin);
				shade = materials[closestHit.materialIndex]->Shade(closestHit, dirToLight, -rayDir);
				finalColor += (radiance * shade * lambert);
				break;
			}
		}
	}
	//Update Color in Buffer
			//Tone Mapping :)
	if (m_ToneMapEnabled)
	{
		finalColor.Hable();
	}
	else
	{
		finalColor.MaxToOne();
	}

	m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
		static_cast<uint8_t>(finalColor.r * 255),
		static_cast<uint8_t>(finalColor.g * 255),
		static_cast<uint8_t>(finalColor.b * 255));
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void dae::Renderer::CycleLightingMode()
{
	m_LightingMode = static_cast<LightingMode>((static_cast<int>(m_LightingMode) + 1) % 4);
}


//bool dae::Renderer::LightHitCheck(const Ray& ray) const
//{
//	return false;
//}
