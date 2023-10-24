#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle):
			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		Vector3 origin{};

		float fovAngle{90.f};
		const float speed{10.f};
		const float mouseSpeed{.1f};

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch {0.f};

		float totalYaw {0.f};


		Matrix cameraToWorld{};


		Matrix CalculateCameraToWorld()
		{
			
			Matrix finalRotation = Matrix::CreateRotation(totalPitch, totalYaw, 0);
			forward = finalRotation.TransformVector(Vector3::UnitZ);
			forward.Normalize();

			right = Vector3::Cross(Vector3::UnitY,forward).Normalized();
			up = Vector3::Cross(forward,right).Normalized();
			
			cameraToWorld = { right,up,forward,origin };

			return cameraToWorld;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
			//wasd controlls
			if (pKeyboardState[SDL_SCANCODE_W]) 
			{
				origin += speed * deltaTime * forward;
			}
			if (pKeyboardState[SDL_SCANCODE_S]) 
			{
				origin -= speed * deltaTime * forward;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += speed * deltaTime * right;
			}
			if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin -= speed * deltaTime * right;
			}
			//mouse controlls
			bool leftMouse{ (mouseState & SDL_BUTTON(1)) != 0 };
			bool rightMouse{ (mouseState & SDL_BUTTON(3)) != 0 };
			
			if (rightMouse && !leftMouse) 
			{
				totalPitch += deltaTime * -mouseY * mouseSpeed;
				totalYaw += deltaTime * mouseX * mouseSpeed;
			}
			
			if (leftMouse && !rightMouse) 
			{
				if (mouseY > 0) 
				{
					origin -= speed * deltaTime * forward;
				}
				else if (mouseY < 0) 
				{
					origin += speed * deltaTime * forward;

				}
				totalYaw += deltaTime * mouseX;
			}

			if (leftMouse && rightMouse)
			{
				if (mouseY > 0) 
				{
					origin -= speed * deltaTime * up;
				}
				else if (mouseY < 0) 
				{
					origin += speed * deltaTime * up;
				}
			}


		}
	};
}
