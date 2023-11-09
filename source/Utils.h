#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"
//use moeller or not
#define moeller

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			const Vector3 rayOriginToSphereOrigin{ sphere.origin - ray.origin };
			const float hypotenuseSquared{ rayOriginToSphereOrigin.SqrMagnitude() };
			const float side1{ Vector3::Dot(rayOriginToSphereOrigin, ray.direction) };
			
			const float distanceToRaySquared = hypotenuseSquared - side1 * side1;
			
			//if the distance to the ray is larger than the radius there will be no results
			//    also if equal because that is the exact border of the circle
			if (distanceToRaySquared >= sphere.radius * sphere.radius) {
				return false;
			}
			
			const float distanceRaypointToIntersect = sqrt(sphere.radius * sphere.radius - distanceToRaySquared);
			const float t = side1 - distanceRaypointToIntersect;
			
			if (t < ray.min || t > ray.max) {
				return false;
			}
			if (ignoreHitRecord) return true;
			
			hitRecord.didHit = true;
			hitRecord.materialIndex = sphere.materialIndex;
			hitRecord.t = t;
			hitRecord.origin = ray.origin + t * ray.direction;
			hitRecord.normal = Vector3(sphere.origin, hitRecord.origin) / sphere.radius;
			return true;
		}
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray) 
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}

#pragma endregion

#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			const float denom = Vector3::Dot(ray.direction,plane.normal);
			const float t = Vector3::Dot((plane.origin - ray.origin), plane.normal) / denom;
			if (t <= ray.min || t > ray.max) // your favorite epsilon
			{
				return false;
			}  
			if (ignoreHitRecord) return true;

			hitRecord.didHit = true;
			hitRecord.t = t;
 			hitRecord.origin = ray.origin + t * ray.direction;
			hitRecord.normal = plane.normal;
			hitRecord.materialIndex = plane.materialIndex;
			
			return true;
		}
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray) 
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
		
		
#pragma endregion
#pragma region Triangle HitTest
		inline bool HitTest_RightSideOfLine(const Vector3& p, const Vector3& v0, const Vector3& v1, const Vector3& n)
		{
			bool b{};
			const Vector3 e{ v1 - v0 };
			const Vector3 cross{ Vector3::Cross(e, p - v0) };
			return Vector3::Dot(cross, n) <= 0;
		}
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			const float dot{ Vector3::Dot(triangle.normal, ray.direction) };

			if (AreEqual(dot, 0)) return false;
			
			const Vector3 l(triangle.v0 - ray.origin);

			const float t { (Vector3::Dot(l,triangle.normal) / Vector3::Dot(ray.direction,triangle.normal)) };

			if (t < ray.min || t > ray.max) 
			{
				return false;
			}
			if (triangle.cullMode == TriangleCullMode::FrontFaceCulling) 
			{
				if ((!ignoreHitRecord && dot < 0.f )|| (ignoreHitRecord && dot > 0.f))return false;
			}
			if (triangle.cullMode == TriangleCullMode::BackFaceCulling) 
			{
				if ((!ignoreHitRecord && dot > 0.f) || (ignoreHitRecord && dot < 0.f))return false;
				return false;
			}

			const Vector3 p{ ray.origin + ray.direction * t };
			
			if (HitTest_RightSideOfLine(p, triangle.v0, triangle.v1, triangle.normal) || 
				HitTest_RightSideOfLine(p, triangle.v1, triangle.v2, triangle.normal) ||
				HitTest_RightSideOfLine(p, triangle.v2, triangle.v0, triangle.normal))
			{
				return false;
			}
			if (ignoreHitRecord) return true;

			hitRecord.didHit = true;
			hitRecord.origin = p;
			hitRecord.materialIndex = triangle.materialIndex;
			hitRecord.t = t;
			hitRecord.normal = triangle.normal;
			return true;

			
		}

		

		inline bool HitTest_TriangleMoeller(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			const float dot{ Vector3::Dot(triangle.normal, ray.direction) };
			
			if (AreEqual(dot, 0)) { return false; }
			
			if (triangle.cullMode == TriangleCullMode::FrontFaceCulling && dot < 0.f) { return false; }
			if (triangle.cullMode == TriangleCullMode::BackFaceCulling && dot > 0.f) { return false; }
			
			const Vector3 edge1{ triangle.v1 - triangle.v0 };
			const Vector3 edge2{ triangle.v2 - triangle.v0 };
			const Vector3 p{ Vector3::Cross(ray.direction,edge2) };
			
			const float f{ Vector3::Dot(p,edge1) };
			if (AreEqual(f, 0)) { return false; }
			
			const float invF{ 1.f / f };
			const Vector3 tVec{ ray.origin - triangle.v0 };
			const float u{ invF * Vector3::Dot(p,tVec) };
			if (u < 0.f || u>1.f) { return false; }
			
			const Vector3 q{ Vector3::Cross(tVec, edge1) };
			const float v{ invF * Vector3::Dot(q ,ray.direction) };
			
			if (v < 0.f || u + v >1.f) { return false; }
			
			const float t{ invF * Vector3::Dot(q,edge2) };
			
			if (t < ray.min || t > ray.max || t > hitRecord.t) { return false; }
			if (ignoreHitRecord) return true;
			
			
			hitRecord.t = t;
			hitRecord.didHit = true;
			hitRecord.origin = ray.origin + ray.direction * t;
			hitRecord.normal = triangle.normal;
			hitRecord.materialIndex = triangle.materialIndex;
			return true;
			

		}
		

#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool  SlabTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			float tx1 = (mesh.transformedMinAABB.x - ray.origin.x) / ray.direction.x;
			float tx2 = (mesh.transformedMaxAABB.x - ray.origin.x) / ray.direction.x;

			float tmin = std::min(tx1, tx2);
			float tmax = std::max(tx1, tx2);

			float ty1 = (mesh.transformedMinAABB.y - ray.origin.y) / ray.direction.y;
			float ty2 = (mesh.transformedMaxAABB.y - ray.origin.y) / ray.direction.y;

			tmin = std::max(tmin, std::min(ty1, ty2));
			tmax = std::min(tmax, std::max(ty1, ty2));

			float tz1 = (mesh.transformedMinAABB.z - ray.origin.z) / ray.direction.z;
			float tz2 = (mesh.transformedMaxAABB.z - ray.origin.z) / ray.direction.z;

			tmin = std::max(tmin, std::min(tz1, tz2));
			tmax = std::min(tmax, std::max(tz1, tz2));

			return tmax > 0 && tmax >= tmin;
		} 

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			if (!SlabTest_TriangleMesh(mesh, ray)) { return false; }
			
			dae::Triangle tri{};
			tri.cullMode = mesh.cullMode;
			tri.materialIndex = mesh.materialIndex;
			

			for (int i = 0; i < mesh.normals.size(); i++)
			{
				tri.v0 = mesh.transformedPositions[mesh.indices[i * 3 + 0]];
				tri.v1 = mesh.transformedPositions[mesh.indices[i * 3 + 1]];
				tri.v2 = mesh.transformedPositions[mesh.indices[i * 3 + 2]];
				tri.normal = mesh.transformedNormals[i];
				
				if (ignoreHitRecord)
				{
					if (HitTest_TriangleMoeller(tri, ray, hitRecord, ignoreHitRecord)) return true;
				}
				else
				{
					HitTest_TriangleMoeller(tri, ray, hitRecord, ignoreHitRecord);
				}
				
			}
			if (ignoreHitRecord) 
			{
				return false;
			}
			else 
			{
				return hitRecord.didHit;
			}
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray) {
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
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
			const float distace = GetDirectionToLight(light, target).Magnitude();
			const float radiance = light.intensity / (distace * distace);
			
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
				const uint32_t i0 = indices[index];
				const uint32_t i1 = indices[index + 1];
				const uint32_t i2 = indices[index + 2];
				
				const Vector3 edgeV0V1 = positions[i1] - positions[i0];
				const Vector3 edgeV0V2 = positions[i2] - positions[i0];
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