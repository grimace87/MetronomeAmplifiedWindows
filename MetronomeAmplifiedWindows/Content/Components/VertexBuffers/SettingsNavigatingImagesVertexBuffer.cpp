#include "pch.h"
#include "SettingsNavigatingImagesVertexBuffer.h"

#include "../../../Common/DeviceResources.h"

vbo::SettingsNavigatingImagesVertexBuffer::SettingsNavigatingImagesVertexBuffer()
{
}

bool vbo::SettingsNavigatingImagesVertexBuffer::IsSizeDependent()
{
	return true;
}

void vbo::SettingsNavigatingImagesVertexBuffer::Initialise(DX::DeviceResources* resources)
{
	// Coordinates used in the vertex buffer depend on the window size
	// Get basic margins and the like
	winrt::Windows::Foundation::Size size = resources->GetOutputSize();
	const float marginLogicalInches = 0.25f;
	const float dpi = resources->GetDpi();
	const float marginUnitsW = 2.0f * (marginLogicalInches * dpi) / size.Width;
	const float marginUnitsH = 2.0f * (marginLogicalInches * dpi) / size.Height;

	const float screenAspect = size.Width / size.Height;
	const float imageAspect = 720.0f / 1280.0f;

	const float h1 = -0.125f - marginUnitsH;
	const float h2 = 1.0f - 5.0f * marginUnitsH;
	const float widthUnits = (h2 - h1) * imageAspect / screenAspect;
	const float w1 = -0.5f * widthUnits;
	const float w2 = 0.5f * widthUnits;

	// Load mesh vertices. Each vertex has a position and a texture coordinate, plus a 4-byte padding.
	structures::VertexTexCoord sceneVertices[6];
	putSquare(sceneVertices, 0, w1, h1, w2, h2, 0.0f, 1.0f, 1.0f, 0.0f);

	m_subBufferVertexIndices = { 0, 6 };
	m_regionsOfInterest = {};

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
