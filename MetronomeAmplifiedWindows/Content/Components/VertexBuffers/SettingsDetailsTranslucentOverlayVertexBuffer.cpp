#include "pch.h"
#include "SettingsDetailsTranslucentOverlayVertexBuffer.h"

#include "../../../Common/DeviceResources.h"

vbo::SettingsDetailsTranslucentOverlayVertexBuffer::SettingsDetailsTranslucentOverlayVertexBuffer()
{
}

bool vbo::SettingsDetailsTranslucentOverlayVertexBuffer::IsSizeDependent()
{
	return true;
}

Concurrency::task<void> vbo::SettingsDetailsTranslucentOverlayVertexBuffer::MakeInitTask(DX::DeviceResources* resources)
{
	// Coordinates used in the vertex buffer depend on the window size
	return Concurrency::create_task([this, resources] {

		// Get necessary coordinates to draw the overlay, in normalised coordinates (range of -1 to 1)
		winrt::Windows::Foundation::Size size = resources->GetOutputSize();
		const float marginLogicalInches = 0.25f;
		const float dpi = resources->GetDpi();
		const float marginUnitsW = 2.0f * (marginLogicalInches * dpi) / size.Width;
		const float marginUnitsH = 2.0f * (marginLogicalInches * dpi) / size.Height;

		const float w1 = -1.0f + marginUnitsW;
		const float w2 = w1 + marginUnitsW;
		const float w4 = 1.0f - marginUnitsW;
		const float w3 = w4 - marginUnitsW;

		const float h1 = -1.0f + marginUnitsH;
		const float h2 = h1 + marginUnitsH;
		const float h4 = 1.0f - 4.0f * marginUnitsH;
		const float h3 = h4 - marginUnitsH;

		// Load mesh vertices. Each vertex has a position and a texture coordinate, plus a 4-byte padding.
		structures::VertexTexCoord sceneVertices[42];
		
		putSquare(sceneVertices, 0, w1, h1, w2, h2, 0.0f, 0.0f, 0.5f, 0.5f);
		putSquare(sceneVertices, 6, w2, h1, w3, h2, 0.5f, 0.0f, 1.0f, 0.5f);
		putSquare(sceneVertices, 12, w3, h1, w4, h2, 0.5f, 0.0f, 0.0f, 0.5f);

		putSquare(sceneVertices, 18, w1, h2, w4, h3, 0.5f, 0.0f, 1.0f, 0.5f);

		putSquare(sceneVertices, 24, w1, h3, w2, h4, 0.0f, 0.5f, 0.5f, 0.0f);
		putSquare(sceneVertices, 30, w2, h3, w3, h4, 0.5f, 0.5f, 1.0f, 0.0f);
		putSquare(sceneVertices, 36, w3, h3, w4, h4, 0.5f, 0.5f, 0.0f, 0.0f);

		m_subBufferVertexIndices = { 0, 42 };

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = sceneVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(sceneVertices), D3D11_BIND_VERTEX_BUFFER);
		winrt::check_hresult(
			resources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				m_vertexBuffer.put()
			)
		);

		m_isValid = true;
		});
}
