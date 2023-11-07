#pragma once

#include <cstdint>
#include "Matrix.h"
#include <vector>



struct SDL_Window;
struct SDL_Surface;



namespace dae
{
	class Scene;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene) const;
		
		void RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float ar, const Matrix cameraToWorld, const Vector3 cameraOrigin) const;

		bool SaveBufferToImage() const;
		void CycleLightingMode();
		void ToggleShadows() { m_ShadowsEnabled = !m_ShadowsEnabled; }
		void ToggleToneMap() { m_ToneMapEnabled = !m_ToneMapEnabled; }


	private:
		enum class LightingMode
		{
			ObservedArea,
			Radiance,
			BRDF,
			Combined
		};

		LightingMode m_LightingMode{ LightingMode::Combined };

		bool m_ShadowsEnabled{ true };
		bool m_ToneMapEnabled{ false };

		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};
		std::vector<int> m_PixelIndices{};

		int m_Width{};
		int m_Height{};
	};
}
