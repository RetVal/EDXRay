#pragma once

#include "EDXPrerequisites.h"
#include "Core/Memory.h"
#include "Math/BoundingBox.h"
#include "SIMD/SSE.h"
#include "Windows/Threading.h"
#include "../ForwardDecl.h"

#include <atomic>

namespace EDX
{
	namespace RayTracer
	{
		struct BuildTriangle
		{
			uint idx1, idx2, idx3;
			uint meshIdx, triIdx;
			bool hasAlpha;

			BuildTriangle(uint i1 = 0, uint i2 = 0, uint i3 = 0, uint m = 0, uint t = 0, bool alpha = false)
				: idx1(i1)
				, idx2(i2)
				, idx3(i3)
				, meshIdx(m)
				, triIdx(t)
				, hasAlpha(alpha)
			{
			}
		};

		struct BuildVertex
		{
			Vector3 pos;
			int pad;

			BuildVertex(float x = 0, float y = 0, float z = 0)
				: pos(x, y, z)
			{
			}
		};

		struct TriangleInfo
		{
			uint idx;
			Vector3 centroid;
			BoundingBox bbox;

			TriangleInfo(uint _idx = 0, const BoundingBox& box = BoundingBox())
				: idx(_idx)
				, bbox(box)
			{
				centroid = bbox.Centroid();
			}
		};

		class BVH2
		{
		public:
			struct BuildNode
			{
				Triangle4* pTriangles;
				uint primCount;

				BoundingBox leftBounds;
				BoundingBox rightBounds;
				BuildNode* pChildren[2];

				BuildNode()
					: primCount(0)
					, pTriangles(nullptr)
				{
					pChildren[0] = pChildren[1] = nullptr;
				}

				void InitLeaf(Triangle4* pTris, uint count)
				{
					pTriangles = pTris;
					primCount = count;
					pChildren[0] = pChildren[1] = nullptr;
				}
				void InitInterior(const BoundingBox& lBounds, const BoundingBox& rBounds, BuildNode* pLeft, BuildNode* pRight)
				{
					pTriangles = nullptr;
					primCount = 0;
					leftBounds = lBounds;
					rightBounds = rBounds;
					pChildren[0] = pLeft;
					pChildren[1] = pRight;
				}
			};

			struct Node
			{
				uint triangleCount;
				uint secondChildOffset;

				FloatSSE minMaxBoundsX;
				FloatSSE minMaxBoundsY;
				FloatSSE minMaxBoundsZ;
			};

		private:
			Node*			mpRoot;
			std::atomic_int	mTreeBufSize;
			BuildVertex*	mpBuildVertices;
			BuildTriangle*	mpBuildIndices;
			uint mBuildVertexCount;
			uint mBuildTriangleCount;
			Array<Primitive*> mRefPrims;

			BoundingBox mBounds;

			const uint MaxDepth;

			Array<UniquePtr<QueuedBuildTask>> mBuildTasks;
			CriticalSection mMemLock;
			CriticalSection mTaskLock;

		public:
			BVH2()
				: mpBuildVertices(nullptr)
				, mpBuildIndices(nullptr)
				, mBuildVertexCount(0)
				, mBuildTriangleCount(0)
				, MaxDepth(128)
			{
			}
			~BVH2()
			{
				Destroy();
			}

			void Construct(const Array<Primitive*>& prims);
			void RecursiveBuildNode(BuildNode* pBuildNode,
				Array<TriangleInfo>& buildInfo,
				const int startIdx,
				const int endIdx,
				const int depth,
				MemoryPool& memory);

			bool Intersect(const Ray& ray, Intersection* pIsect) const;
			bool Occluded(const Ray& ray) const;
			BoundingBox WorldBounds() const
			{
				return mBounds;
			}

		private:

			void ExtractGeometry(const Array<Primitive*>& prims);
			uint LinearizeNodes(const BuildNode* pNode, uint* piOffset);

			void Destroy()
			{
				Memory::SafeDeleteArray(mpBuildVertices);
				Memory::SafeDeleteArray(mpBuildIndices);
				Memory::Free(mpRoot);
			}
		};
	}
}