#include "pch.h"
#include "MainScreenTranslucentOverlayVertexBuffer.h"

#include "../../../Common/DeviceResources.h"

vbo::MainScreenTranslucentOverlayVertexBuffer::MainScreenTranslucentOverlayVertexBuffer()
{
}

bool vbo::MainScreenTranslucentOverlayVertexBuffer::IsSizeDependent()
{
	return true;
}

void vbo::MainScreenTranslucentOverlayVertexBuffer::Initialise(DX::DeviceResources* resources)
{
	// Coordinates used in the vertex buffer depend on the window size
	// Get necessary coordinates to draw the overlay, in normalised coordinates (range of -1 to 1)
	winrt::Windows::Foundation::Size size = resources->GetOutputSize();
	const float marginLogicalInches = 0.25f;
	const float dpi = resources->GetDpi();
	const float marginUnitsW = 2.0f * (marginLogicalInches * dpi) / size.Width;
	const float marginUnitsH = 2.0f * (marginLogicalInches * dpi) / size.Height;

	const float w1 = -1.0f + marginUnitsW;
	const float w2 = w1 + marginUnitsW;
	const float w4 = -1.0f + (2.0f - 2.0f * marginUnitsW) / 3.0f;
	const float w3 = w4 - marginUnitsW;
	const float w5 = w4 + marginUnitsW;
	const float w7 = 1.0f - (2.0f - 2.0f * marginUnitsW) / 3.0f;
	const float w6 = w7 - marginUnitsW;
	const float w8 = w7 + marginUnitsW;
	const float w10 = 1.0f - marginUnitsW;
	const float w9 = w10 - marginUnitsW;

	const float h1 = -1.0f + marginUnitsH;
	const float h2 = h1 + marginUnitsH;
	const float h4 = -1.0f + (2.0f - marginUnitsH) / 4.0f;
	const float h3 = h4 - marginUnitsH;
	const float h6 = 0.0f - marginUnitsH;
	const float h5 = h6 - marginUnitsH;

	// Load mesh vertices. Each vertex has a position and a texture coordinate, plus a 4-byte padding.
	structures::VertexTexCoord sceneVertices[108];

	putSquare(sceneVertices, 0, w1, h1, w2, h2, 0.0f, 0.0f, 0.5f, 0.5f);
	putSquare(sceneVertices, 6, w2, h1, w3, h2, 0.5f, 0.0f, 1.0f, 0.5f);
	putSquare(sceneVertices, 12, w3, h1, w4, h2, 0.5f, 0.0f, 0.0f, 0.5f);

	putSquare(sceneVertices, 18, w7, h1, w8, h2, 0.0f, 0.0f, 0.5f, 0.5f);
	putSquare(sceneVertices, 24, w8, h1, w9, h2, 0.5f, 0.0f, 1.0f, 0.5f);
	putSquare(sceneVertices, 30, w9, h1, w10, h2, 0.5f, 0.0f, 0.0f, 0.5f);

	putSquare(sceneVertices, 36, w1, h2, w2, h4, 0.5f, 0.0f, 1.0f, 0.5f);
	putSquare(sceneVertices, 42, w2, h3, w3, h4, 0.5f, 0.0f, 1.0f, 0.5f);
	putSquare(sceneVertices, 48, w3, h2, w4, h4, 0.5f, 0.0f, 1.0f, 0.5f);

	putSquare(sceneVertices, 54, w7, h2, w8, h4, 0.5f, 0.0f, 1.0f, 0.5f);
	putSquare(sceneVertices, 60, w8, h3, w9, h4, 0.5f, 0.0f, 1.0f, 0.5f);
	putSquare(sceneVertices, 66, w9, h2, w10, h4, 0.5f, 0.0f, 1.0f, 0.5f);

	putSquare(sceneVertices, 72, w4, h3, w5, h4, 0.5f, 1.0f, 0.0f, 0.5f);
	putSquare(sceneVertices, 78, w6, h3, w7, h4, 0.0f, 1.0f, 0.5f, 0.5f);

	putSquare(sceneVertices, 84, w1, h4, w10, h5, 0.5f, 0.0f, 1.0f, 0.5f);

	putSquare(sceneVertices, 90, w1, h5, w2, h6, 0.0f, 0.5f, 0.5f, 0.0f);
	putSquare(sceneVertices, 96, w2, h5, w9, h6, 0.5f, 0.5f, 1.0f, 0.0f);
	putSquare(sceneVertices, 102, w9, h5, w10, h6, 0.5f, 0.5f, 0.0f, 0.0f);

	m_subBufferVertexIndices = { 0, 108 };

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
}
