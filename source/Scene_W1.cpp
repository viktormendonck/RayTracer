#include "Scene_W1.h"
#include "Material.h"


void Scene_W1::Initialize()
{
	//default: Material id0 >> SolidColor Material (RED)
	constexpr unsigned char matId_Solid_Red = 0;
	const unsigned char matId_Solid_Blue = AddMaterial(new dae::Material_SolidColor{ dae::colors::Blue });

	const unsigned char matId_Solid_Yellow = AddMaterial(new dae::Material_SolidColor{ dae::colors::Yellow });
	const unsigned char matId_Solid_Green = AddMaterial(new dae::Material_SolidColor{ dae::colors::Green });
	const unsigned char matId_Solid_Cyan = AddMaterial(new dae::Material_SolidColor{ dae::colors::Cyan });
	const unsigned char matId_Solid_Magenta = AddMaterial(new dae::Material_SolidColor{ dae::colors::Magenta });
	const unsigned char matId_Solid_White = AddMaterial(new dae::Material_SolidColor{ dae::colors::White });
	const unsigned char matId_Solid_Gray = AddMaterial(new dae::Material_SolidColor{ dae::colors::Gray });
	const unsigned char matId_Solid_Black = AddMaterial(new dae::Material_SolidColor{ dae::colors::Black });

	const float distance{ 0.5f };
	const int ballsAmount{ 200 };

	for (int i{ 0 }; i < ballsAmount / 2; ++i)
	{
		//const unsigned char color { static_cast<unsigned char>(i % 8) };
		const unsigned char color{ matId_Solid_Cyan };
		const float location{ distance * i };
		AddSphere({ -location, location, 100.f }, 50.f, color);
		AddSphere({ location, -location, 100.f }, 50.f, color);

	}


	//Plane
	AddPlane({ -75.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matId_Solid_Green);
	AddPlane({ 75.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matId_Solid_Green);
	AddPlane({ 0.f, -75.f, 0.f }, { 0.f, 1.f,0.f }, matId_Solid_Yellow);
	AddPlane({ 0.f, 75.f, 0.f }, { 0.f, -1.f,0.f }, matId_Solid_Yellow);
	AddPlane({ 0.f, 0.f, 125.f }, { 0.f, 0.f,-1.f }, matId_Solid_Magenta);
}