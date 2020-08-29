#include "pch.h"
#include "SettingsDetailsIconsVertexBuffer.h"

#include "../../../Common/DeviceResources.h"

vbo::SettingsDetailsIconsVertexBuffer::SettingsDetailsIconsVertexBuffer()
{
}

bool vbo::SettingsDetailsIconsVertexBuffer::IsSizeDependent()
{
	return true;
}

void vbo::SettingsDetailsIconsVertexBuffer::Initialise(DX::DeviceResources* resources)
{
	// Coordinates used in the vertex buffer depend on the window size
	// Get necessary coordinates to draw the overlay, in normalised coordinates (range of -1 to 1)
	winrt::Windows::Foundation::Size size = resources->GetOutputSize();
	const float marginLogicalInches = 0.25f;
	const float dpi = resources->GetDpi();
	const float marginUnitsW = 2.0f * (marginLogicalInches * dpi) / size.Width;
	const float marginUnitsH = 2.0f * (marginLogicalInches * dpi) / size.Height;

	const float aspect = size.Width / size.Height;
	const float iconHeightUnits = 0.25f;
	const float iconWidthUnits = iconHeightUnits * 0.5f / aspect;

	const float w1 = -1.0f + marginUnitsW;
	const float w2 = w1 + iconWidthUnits;
	const float w4 = 1.0f - marginUnitsW;
	const float w3 = w4 - iconWidthUnits;

	const float h1 = -0.5f * iconHeightUnits;
	const float h2 = 0.5f * iconHeightUnits;

	// Load mesh vertices. Each vertex has a position and a texture coordinate, plus a 4-byte padding.
	structures::VertexTexCoord sceneVertices[12];

	putSquare(sceneVertices, 0, w1, h1, w2, h2, 0.875f, 0.5f, 1.0f, 1.0f);
	putSquare(sceneVertices, 6, w3, h1, w4, h2, 1.0f, 0.5f, 0.875f, 1.0f);

	m_subBufferVertexIndices = { 0, 12 };

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
