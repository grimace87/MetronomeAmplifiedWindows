#pragma once

#include "Resources/VertexBuffers.h"

#include <map>

namespace cache {

	class VertexBufferCache {
	private:
		std::map<vbo::ClassId, vbo::BaseVertexBuffer*> m_vertexBuffers;
		bool m_sizeIndependentBuffersAreFulfilled;
		bool m_sizeDependentBuffersAreFulfilled;

	public:
		VertexBufferCache();
		bool ContainsAll(std::vector<vbo::ClassId>& vertexBufferClasses);
		void RequireSizeIndependentVertexBuffers(DX::DeviceResources* resources, std::vector<vbo::ClassId>& vertexBufferClasses);
		void RequireSizeDependentVertexBuffers(DX::DeviceResources* resources, std::vector<vbo::ClassId>& vertexBufferClasses);
		inline bool AreVertexBuffersFulfilled() { return m_sizeIndependentBuffersAreFulfilled && m_sizeDependentBuffersAreFulfilled; }
		vbo::BaseVertexBuffer* GetVertexBuffer(vbo::ClassId vertexBufferClass);
		void Clear();
	};
}
