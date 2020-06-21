#include "pch.h"
#include "VertexBuffers.h"

#include "Common/DirectXHelper.h"
#include "Common/DeviceResources.h"

vbo::BaseVertexBuffer::BaseVertexBuffer() : m_vertexCount(0), m_isValid(false)
{
}

// Puts the vertex data for a square into a float array, assuming each vertex fills 6 floats [x, y, z, s, t, unused]
void vbo::BaseVertexBuffer::putSquare(structures::VertexTexCoord buffer[], int index, float x1, float y1, float x2, float y2, float s1, float t1, float s2, float t2)
{
	using namespace DirectX;

	structures::VertexTexCoord squareVertices[] =
	{
		{XMFLOAT3(x1, y1, 0.0f), XMFLOAT3(s1, t1, 0.0f)},
		{XMFLOAT3(x1, y2, 0.0f), XMFLOAT3(s1, t2, 0.0f)},
		{XMFLOAT3(x2, y2, 0.0f), XMFLOAT3(s2, t2, 0.0f)},
		{XMFLOAT3(x2, y2, 0.0f), XMFLOAT3(s2, t2, 0.0f)},
		{XMFLOAT3(x2, y1, 0.0f), XMFLOAT3(s2, t1, 0.0f)},
		{XMFLOAT3(x1, y1, 0.0f), XMFLOAT3(s1, t1, 0.0f)}
	};
	std::copy(squareVertices, squareVertices + 6, &buffer[index]);
}

vbo::BaseVertexBuffer* vbo::BaseVertexBuffer::NewFromClassId(ClassId id)
{
	switch (id) {
	case ClassId::MAIN_SCREEN_BG:
		return new MainScreenBgVertexBuffer();
	default:
		throw std::exception("Requested VBO class does not exist");
	}
}

void vbo::BaseVertexBuffer::Activate(ID3D11DeviceContext3* context)
{
	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(structures::VertexTexCoord);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void vbo::BaseVertexBuffer::Reset()
{
	m_isValid = false;
	m_vertexBuffer.Reset();
}

vbo::MainScreenBgVertexBuffer::MainScreenBgVertexBuffer()
{
}

bool vbo::MainScreenBgVertexBuffer::IsSizeDependent()
{
	return true;
}

Concurrency::task<void> vbo::MainScreenBgVertexBuffer::MakeInitTask(DX::DeviceResources* resources)
{
	// Coordinates used in the vertex buffer depend on the window size
	return Concurrency::create_task([this, resources] {

		// Get necessary coordinates to draw the overlay, in normalised coordinates (range of -1 to 1)
		Windows::Foundation::Size size = resources->GetOutputSize();
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
		structures::VertexTexCoord sceneVertices[114];

		putSquare(sceneVertices, 0, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		putSquare(sceneVertices, 6, w1, h1, w2, h2, 0.0f, 0.0f, 0.5f, 0.5f);
		putSquare(sceneVertices, 12, w2, h1, w3, h2, 0.5f, 0.0f, 1.0f, 0.5f);
		putSquare(sceneVertices, 18, w3, h1, w4, h2, 0.5f, 0.0f, 0.0f, 0.5f);

		putSquare(sceneVertices, 24, w7, h1, w8, h2, 0.0f, 0.0f, 0.5f, 0.5f);
		putSquare(sceneVertices, 30, w8, h1, w9, h2, 0.5f, 0.0f, 1.0f, 0.5f);
		putSquare(sceneVertices, 36, w9, h1, w10, h2, 0.5f, 0.0f, 0.0f, 0.5f);

		putSquare(sceneVertices, 42, w1, h2, w2, h4, 0.5f, 0.0f, 1.0f, 0.5f);
		putSquare(sceneVertices, 48, w2, h3, w3, h4, 0.5f, 0.0f, 1.0f, 0.5f);
		putSquare(sceneVertices, 54, w3, h2, w4, h4, 0.5f, 0.0f, 1.0f, 0.5f);

		putSquare(sceneVertices, 60, w7, h2, w8, h4, 0.5f, 0.0f, 1.0f, 0.5f);
		putSquare(sceneVertices, 66, w8, h3, w9, h4, 0.5f, 0.0f, 1.0f, 0.5f);
		putSquare(sceneVertices, 72, w9, h2, w10, h4, 0.5f, 0.0f, 1.0f, 0.5f);

		putSquare(sceneVertices, 78, w4, h3, w5, h4, 0.5f, 1.0f, 0.0f, 0.5f);
		putSquare(sceneVertices, 84, w6, h3, w7, h4, 0.0f, 1.0f, 0.5f, 0.5f);

		putSquare(sceneVertices, 90, w1, h4, w10, h5, 0.5f, 0.0f, 1.0f, 0.5f);

		putSquare(sceneVertices, 96, w1, h5, w2, h6, 0.0f, 0.5f, 0.5f, 0.0f);
		putSquare(sceneVertices, 102, w2, h5, w9, h6, 0.5f, 0.5f, 1.0f, 0.0f);
		putSquare(sceneVertices, 108, w9, h5, w10, h6, 0.5f, 0.5f, 0.0f, 0.0f);

		m_vertexCount = 114;

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = sceneVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(sceneVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			resources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
			)
		);

		m_isValid = true;
		});
}
