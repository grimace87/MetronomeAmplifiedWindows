#include "pch.h"
#include "BackgroundVertexBuffer.h"

#include "../../../Common/DeviceResources.h"

vbo::BackgroundVertexBuffer::BackgroundVertexBuffer()
{
}

bool vbo::BackgroundVertexBuffer::IsSizeDependent()
{
	return true;
}

void vbo::BackgroundVertexBuffer::Initialise(DX::DeviceResources* resources)
{
	// Load mesh vertices. Each vertex has a position and a texture coordinate, plus a 4-byte padding.
	structures::VertexTexCoord sceneVertices[6];
	putSquare(sceneVertices, 0, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

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
