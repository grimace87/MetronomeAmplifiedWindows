#include "pch.h"
#include "VertexBuffers.h"

#include "Common/DirectXHelper.h"
#include "Common/DeviceResources.h"
#include "../../Common/Font.h"

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

void vbo::BaseVertexBuffer::putSquareCentredInside(structures::VertexTexCoord buffer[], int index, float x1, float y1, float x2, float y2, float s1, float t1, float s2, float t2, winrt::Windows::Foundation::Size size)
{
	// Get units to pixels scaling factor, and use those to determine dimensions of requested rect in pixels
	const float pixelsPerUnitWidth = size.Width / 2.0f;
	const float pixelsPerUnitHeight = size.Height / 2.0f;
	const float rectWidthPixels = abs(x2 - x1) * pixelsPerUnitWidth;
	const float rectHeightPixels = abs(y2 - y1) * pixelsPerUnitHeight;

	// Figure out where the square lies in this rect (squareness is defined within pixel coordinates)
	if (rectWidthPixels > rectHeightPixels) {
		const float direction = x1 > x2 ? -1.0f : 1.0f;
		const float widthMargin = direction * 0.5f * (rectWidthPixels - rectHeightPixels) / pixelsPerUnitWidth;
		putSquare(buffer, index, x1 + widthMargin, y1, x2 - widthMargin, y2, s1, t1, s2, t2);
	} else {
		const float direction = y1 > y2 ? -1.0f : 1.0f;
		const float heightMargin = direction * 0.5f * (rectHeightPixels - rectWidthPixels) / pixelsPerUnitHeight;
		putSquare(buffer, index, x1, y1 + heightMargin, x2, y2 - heightMargin, s1, t1, s2, t2);
	}
}

vbo::BaseVertexBuffer* vbo::BaseVertexBuffer::NewFromClassId(ClassId id)
{
	switch (id) {
	case ClassId::MAIN_SCREEN_BG:
		return new MainScreenBgVertexBuffer();
	case ClassId::ICON_LABELS:
		return new IconLabelsVertexBuffer();
	default:
		throw std::exception("Requested VBO class does not exist");
	}
}

void vbo::BaseVertexBuffer::Activate(ID3D11DeviceContext3* context)
{
	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(structures::VertexTexCoord);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer = m_vertexBuffer.get();
	context->IASetVertexBuffers(
		0,
		1,
		&vertexBuffer,
		&stride,
		&offset
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void vbo::BaseVertexBuffer::Reset()
{
	m_isValid = false;
	m_vertexBuffer = nullptr;
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
		structures::VertexTexCoord sceneVertices[150];

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

		putSquareCentredInside(sceneVertices, 114, hIcon1Left, hIconBottom, hIcon2Left, hIconTop, 0.0f, 0.5f, 0.25f, 0.0f, size);
		putSquareCentredInside(sceneVertices, 120, hIcon2Left, hIconBottom, hIcon3Left, hIconTop, 0.25f, 0.5f, 0.5f, 0.0f, size);
		putSquareCentredInside(sceneVertices, 126, hIcon3Left, hIconBottom, hIcon4Left, hIconTop, 0.5f, 0.5f, 0.75f, 0.0f, size);
		putSquareCentredInside(sceneVertices, 132, hIcon4Left, hIconBottom, hIcon4Right, hIconTop, 0.75f, 0.5f, 1.0f, 0.0f, size);
		putSquareCentredInside(sceneVertices, 138, w2, hLowerIconsLabelTop, w3, h3, 0.0f, 1.0f, 0.25f, 0.5f, size);
		putSquareCentredInside(sceneVertices, 144, w8, hLowerIconsLabelTop, w9, h3, 0.25f, 1.0f, 0.5f, 0.5f, size);

		m_vertexCount = 150;

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

vbo::IconLabelsVertexBuffer::IconLabelsVertexBuffer()
{
}

bool vbo::IconLabelsVertexBuffer::IsSizeDependent()
{
	return true;
}

Concurrency::task<void> vbo::IconLabelsVertexBuffer::MakeInitTask(DX::DeviceResources* resources)
{
	// Coordinates used in the vertex buffer depend on the window size
	return Concurrency::create_task([this, resources]() -> void {

		font::Font* orkney = resources->GetOrkneyFont();
		if (orkney == nullptr) {
			return;
		}

		// Guidelines from MainScreenBackgroundVertexBuffer
		winrt::Windows::Foundation::Size size = resources->GetOutputSize();
		const float marginLogicalInches = 0.25f;
		const float dpi = resources->GetDpi();
		const float marginUnitsW = 2.0f * (marginLogicalInches * dpi) / size.Width;
		const float marginUnitsH = 2.0f * (marginLogicalInches * dpi) / size.Height;
		const float w2 = -1.0f + 2.0f * marginUnitsW;
		const float w3 = -1.0f + (2.0f - 2.0f * marginUnitsW) / 3.0f - marginUnitsW;
		const float w8 = 1.0f - (2.0f - 2.0f * marginUnitsW) / 3.0f + marginUnitsW;
		const float w9 = 1.0f - 2.0f * marginUnitsW;
		const float hIcon1Left = -1.0f;
		const float hIcon2Left = -0.5f;
		const float hIcon3Left = 0.0f;
		const float hIcon4Left = 0.5f;
		const float hIcon4Right = 1.0f;
		const float hIconBottom = 0.7f;
		const float hIconLabelBottom = 0.5f;
		const float h2 = -1.0f + 2.0f * marginUnitsH;
		const float h3 = -1.0f + (2.0f - marginUnitsH) / 4.0f - marginUnitsH;
		const float hLowerIconsLabelTop = h2 + 0.25f * (h3 - h2);

		std::vector<structures::VertexTexCoord> vboData;
		std::vector<std::string> labels = {
			"TONE",
			"SONG",
			"HELP",
			"SETTINGS",
			"TIMER",
			"LIFT"
		};
		int totalStructCount = 0;
		for (auto& label : labels) {
			totalStructCount += 6 * label.length();
		}
		vboData.resize(totalStructCount);

		int bufferIndex = 0;
		const float maxTextHeightPixels = 1.2f * marginLogicalInches * dpi;
		orkney->PrintTextIntoVboCentredInside(vboData, bufferIndex, labels[0], hIcon1Left, hIconBottom, hIcon2Left - hIcon1Left, hIconBottom - hIconLabelBottom, maxTextHeightPixels, size);
		bufferIndex += 6 * labels[0].length();
		orkney->PrintTextIntoVboCentredInside(vboData, bufferIndex, labels[1], hIcon2Left, hIconBottom, hIcon3Left - hIcon2Left, hIconBottom - hIconLabelBottom, maxTextHeightPixels, size);
		bufferIndex += 6 * labels[1].length();
		orkney->PrintTextIntoVboCentredInside(vboData, bufferIndex, labels[2], hIcon3Left, hIconBottom, hIcon4Left - hIcon3Left, hIconBottom - hIconLabelBottom, maxTextHeightPixels, size);
		bufferIndex += 6 * labels[2].length();
		orkney->PrintTextIntoVboCentredInside(vboData, bufferIndex, labels[3], hIcon4Left, hIconBottom, hIcon4Right - hIcon4Left, hIconBottom - hIconLabelBottom, maxTextHeightPixels, size);
		bufferIndex += 6 * labels[3].length();
		orkney->PrintTextIntoVboCentredInside(vboData, bufferIndex, labels[4], w2, hLowerIconsLabelTop, w3 - w2, hLowerIconsLabelTop - h2, maxTextHeightPixels, size);
		bufferIndex += 6 * labels[4].length();
		orkney->PrintTextIntoVboCentredInside(vboData, bufferIndex, labels[5], w8, hLowerIconsLabelTop, w9 - w8, hLowerIconsLabelTop - h2, maxTextHeightPixels, size);
		bufferIndex += 6 * labels[5].length();

		m_vertexCount = vboData.size();

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
		vertexBufferData.pSysMem = vboData.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;

		CD3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(CD3D11_BUFFER_DESC));
		vertexBufferDesc.ByteWidth = vboData.size() * sizeof(structures::VertexTexCoord);
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
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
