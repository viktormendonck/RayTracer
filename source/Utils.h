#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord)
		{
			//todo W1
			float a{ Vector3::Dot(ray.direction, ray.direction) };
			Vector3 sphereToRay{ ray.origin - sphere.origin };
			float b{ Vector3::Dot(2 * ray.direction, sphereToRay) };
			float c{ Vector3::Dot(sphereToRay, sphereToRay) - (sphere.radius * sphere.radius) };
			float discriminant{ b * b - 4 * a * c };

			if (discriminant <= 0)
			{
				return false;
			}

			float t = (-b - sqrtf(discriminant)) / (2 * a);

			if (t <= ray.min || t > ray.max)
			{
				return false;
			}

			hitRecord.t = t;
			hitRecord.didHit = true;
			hitRecord.origin = ray.origin + hitRecord.t * ray.direction;
			hitRecord.normal = (hitRecord.origin - sphere.origin).Normalized();
			hitRecord.materialIndex = sphere.materialIndex;


			return hitRecord.didHit;
		}

		inline bool DoesHit_Sphere(const Sphere& sphere, const Ray& ray)
		{
			float a{ Vector3::Dot(ray.direction, ray.direction) };
			Vector3 sphereToRay{ ray.origin - sphere.origin };
			float b{ Vector3::Dot(2 * ray.direction, sphereToRay) };
			float c{ Vector3::Dot(sphereToRay, sphereToRay) - (sphere.radius * sphere.radius) };
			float discriminant{ b * b - 4 * a * c };

			if (discriminant <= 0)
			{
				return false;
			}

			float t = (-b - sqrtf(discriminant)) / (2 * a);

			if (t <= ray.min || t > ray.max)
			{
				return false;
			}
			return true;
		}

#pragma endregion

#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord)
		{
			float denom = Vector3::Dot(ray.direction,plane.normal);
			float t = Vector3::Dot((plane.origin - ray.origin), plane.normal) / denom;
			if (t <= ray.min || t > ray.max) // your favorite epsilon
			{
				return false;
			}  
			
			hitRecord.didHit = true;
			hitRecord.t = t;
 			hitRecord.origin = ray.origin + t * ray.direction;
			hitRecord.normal = plane.normal;
			hitRecord.materialIndex = plane.materialIndex;
			
			return true;
		}

		inline bool DoesHit_Plane(const Plane& plane, const Ray& ray)
		{
			float denom = Vector3::Dot(ray.direction, plane.normal);
			float t = Vector3::Dot((plane.origin - ray.origin), plane.normal) / denom;
			if (t <= ray.min || t > ray.max) 
			{
				return false;
			}
			return true;
		}
		
#pragma endregion
#pragma region Triangle HitTest
		inline bool HitTest_RightSideOfLine(const Vector3& p, const Vector3& v0, const Vector3& v1, const Vector3& n)
		{
			Vector3 e{ v1 - v0 };
			Vector3 cross{ Vector3::Cross(e, p - v0) };
			return Vector3::Dot(cross, n) <= 0;
		}
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord)
		{
			const float dot{ Vector3::Dot(triangle.normal, ray.direction) };

			if (AreEqual(dot, 0)) return false;
			
			Vector3 l(triangle.v0 - ray.origin);

			float t { (Vector3::Dot(l,triangle.normal) / Vector3::Dot(ray.direction,triangle.normal)) };

			if (t < ray.min || t > ray.max) 
			{
				return false;
			}
			if (triangle.cullMode == TriangleCullMode::FrontFaceCulling && dot < 0.f) 
			{
				return false;
			}
			if (triangle.cullMode == TriangleCullMode::BackFaceCulling && dot > 0.f) 
			{
				return false;
			}

			Vector3 p{ ray.origin + ray.direction * t };
			
			if (HitTest_RightSideOfLine(p, triangle.v0, triangle.v1, triangle.normal) || 
				HitTest_RightSideOfLine(p, triangle.v1, triangle.v2, triangle.normal) ||
				HitTest_RightSideOfLine(p, triangle.v2, triangle.v0, triangle.normal))
			{
				return false;
			}

			hitRecord.didHit = true;
			hitRecord.origin = p;
			hitRecord.materialIndex = triangle.materialIndex;
			hitRecord.t = t;
			hitRecord.normal = triangle.normal;
			return true;

			
		}

		inline bool DoesHit_Triangle (const Triangle& triangle, const Ray& ray)
		{

			const float dot{ Vector3::Dot(triangle.normal, ray.direction) };

			if (AreEqual(dot, 0)) return false;

			Vector3 l(triangle.v0 - ray.origin);

			float t{ (Vector3::Dot(l,triangle.normal) / Vector3::Dot(ray.direction,triangle.normal)) };

			if (t < ray.min || t > ray.max)
			{
				return false;
			}
			if (triangle.cullMode == TriangleCullMode::FrontFaceCulling && dot > 0.f)
			{
				return false;
			}
			if (triangle.cullMode == TriangleCullMode::BackFaceCulling && dot < 0.f)
			{
				return false;
			}

			Vector3 p{ ray.origin + ray.direction * t };

			if (HitTest_RightSideOfLine(p, triangle.v0, triangle.v1, triangle.normal))
			{
				return false;
			}
			if (HitTest_RightSideOfLine(p, triangle.v1, triangle.v2, triangle.normal))
			{
				return false;
			}
			if (HitTest_RightSideOfLine(p, triangle.v2, triangle.v0, triangle.normal))
			{
				return false;
			}
			return true;
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord)
		{
			HitRecord temp{};
			hitRecord.t = INFINITY;
			for (int i = 0; i < mesh.indices.size() / 3; i++)
			{
				const Vector3& v0 = mesh.transformedPositions[mesh.indices[i * 3]];
				const Vector3& v1 = mesh.transformedPositions[mesh.indices[i * 3 + 1]];
				const Vector3& v2 = mesh.transformedPositions[mesh.indices[i * 3 + 2]];

				auto triangle = Triangle(v0, v1, v2, mesh.transformedNormals[i]);
				triangle.cullMode = mesh.cullMode;
				triangle.materialIndex = mesh.materialIndex;
				if (HitTest_Triangle(triangle, ray, temp))
				{
					if (hitRecord.t > temp.t) 
					{
						hitRecord = temp;
					}
				}
			}
			return hitRecord.didHit;
			
		}

		inline bool DoesHit_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			for (int i = 0; i < mesh.indices.size() / 3; i++)
			{
				const Vector3& v0 = mesh.transformedPositions[mesh.indices[i * 3]];
				const Vector3& v1 = mesh.transformedPositions[mesh.indices[i * 3 + 1]];
				const Vector3& v2 = mesh.transformedPositions[mesh.indices[i * 3 + 2]];

				auto triangle = Triangle(v0, v1, v2, mesh.transformedNormals[i]);
				triangle.cullMode = mesh.cullMode;
				triangle.materialIndex = mesh.materialIndex;
				if (DoesHit_Triangle(triangle, ray))
				{
					return true;
				}
			}
			return false;
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			return { light.origin - origin };
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			float distace = GetDirectionToLight(light, target).Magnitude();
			float radiance = light.intensity / (distace * distace);
			
			return { light.color * radiance};
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}