#pragma once

#include "../ShaderStructures.h"

#include <string>

namespace DX {
	class DeviceResources;
}

namespace vbo {

	enum class ClassId {
		BG,
		MAIN_SCREEN_TRANSLUCENT_OVERLAY,
		MAIN_SCREEN_ICONS,
		MAIN_SCREEN_ICON_LABELS,
		SETTINGS_HUB_LABELS,
		HELP_DETAILS_OVERLAY,
		HELP_DETAILS_ICONS,
		HELP_NAVIGATING_TEXTS,
		HELP_NAVIGATING_IMAGES
	};

	class BaseVertexBuffer {
	protected:
		bool m_isValid;
		winrt::com_ptr<ID3D11Buffer> m_vertexBuffer;
		std::vector<unsigned int> m_subBufferVertexIndices;

		BaseVertexBuffer();
		void putSquare(structures::VertexTexCoord buffer[], int index, float x1, float y1, float x2, float y2, float s1, float t1, float s2, float t2);
		void putSquareCentredInside(structures::VertexTexCoord buffer[], int index, float x1, float y1, float x2, float y2, float s1, float t1, float s2, float t2, winrt::Windows::Foundation::Size size);

	public:
		static BaseVertexBuffer* NewFromClassId(ClassId id);
		virtual bool IsSizeDependent() = 0;
		virtual void Initialise(DX::DeviceResources* resources) = 0;
		void Activate(ID3D11DeviceContext3* context);
		void Reset();

		inline bool IsValid() { return m_isValid; }
		inline unsigned int IndexOfSubBuffer(int index) { return m_subBufferVertexIndices[index]; }
		inline unsigned int VerticesInSubBuffer(int index) { return m_subBufferVertexIndices[index + 1] - m_subBufferVertexIndices[index]; }
	};
}
