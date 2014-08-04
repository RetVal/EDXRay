#pragma once

#include "../ForwardDecl.h"
#include "Math/Vector.h"

namespace EDX
{
	namespace RayTracer
	{
		class TriangleMesh
		{
		protected:
			Vector3*	mpPositionBuffer;
			Vector3*	mpNormalBuffer;
			Vector2*	mpTexcoordBuffer;
			uint*		mpIndexBuffer;

			uint		mVertexCount;
			uint		mTriangleCount;

		public:
			TriangleMesh()
				: mpPositionBuffer(nullptr)
				, mpNormalBuffer(nullptr)
				, mpTexcoordBuffer(nullptr)
				, mpIndexBuffer(nullptr)
				, mVertexCount(0)
				, mTriangleCount(0)
			{
			}
			~TriangleMesh()
			{
				Release();
			}

			void LoadMesh(const char* path,
				const Vector3& pos = Vector3::ZERO,
				const Vector3& scl = Vector3::UNIT_SCALE,
				const Vector3& rot = Vector3::ZERO);

			void LoadSphere(const float radius,
				const Vector3& pos = Vector3::ZERO,
				const Vector3& scl = Vector3::UNIT_SCALE,
				const Vector3& rot = Vector3::ZERO);

			Vector3 GetPositionAt(uint idx) const;
			Vector3 GetNormalAt(uint idx) const;
			Vector2 GetTexcoordAt(uint idx) const;

			uint GetTriangleCount() const
			{
				return mTriangleCount;
			}
			uint GetVertexCount() const
			{
				return mVertexCount;
			}

			void Release();
		};
	}
}