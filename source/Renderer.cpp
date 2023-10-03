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



using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	float fov{ std::tanf(camera.fovAngle * PI / 180.f / 2.f) };
	camera.CalculateCameraToWorld();
	float ar{ static_cast<float>(m_Width) / static_cast<float>(m_Height) };

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{	
			float cameraX{ (2.f * ((px + 0.5f) / m_Width) - 1.f) * ar * fov };
			float cameraY{ (1.f - (2.f * (py + 0.5f)/ m_Height)) * fov };
			
			Vector3 rayDir{ (camera.forward + (camera.right * cameraX) + (camera.up * cameraY)) };
			rayDir.Normalize();

			Ray viewRay{ camera.origin,rayDir };
			ColorRGB finalColor{};
			HitRecord closestHit{};
			
			pScene->GetClosestHit(viewRay, closestHit);

			if (closestHit.didHit) {
				finalColor = materials[closestHit.materialIndex]->Shade();
					
				for (int lightIndex{0}; lightIndex < pScene->GetLights().size(); ++lightIndex)
				{
					Vector3 dirToLight{ LightUtils::GetDirectionToLight(pScene->GetLights()[lightIndex], closestHit.origin) };
					float lightDist{ dirToLight.Normalize() };
					Ray ray{ closestHit.origin + closestHit.normal * 0.001f,dirToLight,0.f,lightDist};
					

					if (pScene->DoesHit(ray))
					{
						finalColor *= 0.5f;
					}
				}
			}

			//Update Color in Buffer
			finalColor.MaxToOne();
			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

//bool dae::Renderer::LightHitCheck(const Ray& ray) const
//{
//	return false;
//}
