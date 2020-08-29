#include "pch.h"
#include "MainScreenIconsVertexBuffer.h"

#include "../../../Common/DeviceResources.h"

vbo::MainScreenIconsVertexBuffer::MainScreenIconsVertexBuffer()
{
}

bool vbo::MainScreenIconsVertexBuffer::IsSizeDependent()
{
	return true;
}

void vbo::MainScreenIconsVertexBuffer::Initialise(DX::DeviceResources* resources)
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

	const float hIcon1Left = -1.0f;
	const float hIcon2Left = -0.5f;
	const float hIcon3Left = 0.0f;
	const float hIcon4Left = 0.5f;
	const float hIcon4Right = 1.0f;

	const float hIconBottom = 0.7f;
	const float hIconLabelBottom = 0.5;
	const float hIconTop = 1.0f;
	const float hLowerIconsLabelTop = h2 + 0.25f * (h3 - h2);

	// Load mesh vertices. Each vertex has a position and a texture coordinate, plus a 4-byte padding.
	structures::VertexTexCoord sceneVertices[36];

	putSquareCentredInside(sceneVertices, 0, hIcon1Left, hIconBottom, hIcon2Left, hIconTop, 0.0f, 0.5f, 0.25f, 0.0f, size);
	putSquareCentredInside(sceneVertices, 6, hIcon2Left, hIconBottom, hIcon3Left, hIconTop, 0.25f, 0.5f, 0.5f, 0.0f, size);
	putSquareCentredInside(sceneVertices, 12, hIcon3Left, hIconBottom, hIcon4Left, hIconTop, 0.5f, 0.5f, 0.75f, 0.0f, size);
	putSquareCentredInside(sceneVertices, 18, hIcon4Left, hIconBottom, hIcon4Right, hIconTop, 0.75f, 0.5f, 1.0f, 0.0f, size);
	putSquareCentredInside(sceneVertices, 24, w2, hLowerIconsLabelTop, w3, h3, 0.0f, 1.0f, 0.25f, 0.5f, size);
	putSquareCentredInside(sceneVertices, 30, w8, hLowerIconsLabelTop, w9, h3, 0.25f, 1.0f, 0.5f, 0.5f, size);

	m_subBufferVertexIndices = { 0, 36 };

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
