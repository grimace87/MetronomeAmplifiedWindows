#pragma once

#include "../ShaderStructures.h"

#include <string>

namespace DX {
	class DeviceResources;
}

namespace vbo {

	enum class ClassId {
		MAIN_SCREEN_BG,
		ICON_LABELS,
		SETTINGS_HUB_LABELS
	};

	class BaseVertexBuffer {
	protected:
		bool m_isValid;
		UINT m_vertexCount;
		winrt::com_ptr<ID3D11Buffer> m_vertexBuffer;

		BaseVertexBuffer();
		void putSquare(structures::VertexTexCoord buffer[], int index, float x1, float y1, float x2, float y2, float s1, float t1, float s2, float t2);
		void putSquareCentredInside(structures::VertexTexCoord buffer[], int index, float x1, float y1, float x2, float y2, float s1, float t1, float s2, float t2, winrt::Windows::Foundation::Size size);

	public:
		static BaseVertexBuffer* NewFromClassId(ClassId id);
		virtual bool IsSizeDependent() = 0;
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) = 0;
		void Activate(ID3D11DeviceContext3* context);
		void Reset();

		inline bool IsValid() { return m_isValid; }
		inline UINT GetVertexCount() { return m_vertexCount; }
	};

	class MainScreenBgVertexBuffer : public BaseVertexBuffer {
	public:
		MainScreenBgVertexBuffer();
		virtual bool IsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};

	class IconLabelsVertexBuffer : public BaseVertexBuffer {
	public:
		IconLabelsVertexBuffer();
		virtual bool IsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};

	class SettingsHubLabelsVertexBuffer : public BaseVertexBuffer {
	public:
        SettingsHubLabelsVertexBuffer();
		virtual bool IsSizeDependent() override;
	protected:
		virtual Concurrency::task<void> MakeInitTask(DX::DeviceResources* resources) override;
	};
}
