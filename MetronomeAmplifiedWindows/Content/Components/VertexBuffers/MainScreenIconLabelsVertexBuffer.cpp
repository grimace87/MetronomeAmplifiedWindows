#include "pch.h"
#include "MainScreenIconLabelsVertexBuffer.h"

#include "../../../Common/DeviceResources.h"

vbo::MainScreenIconLabelsVertexBuffer::MainScreenIconLabelsVertexBuffer()
{
}

bool vbo::MainScreenIconLabelsVertexBuffer::IsSizeDependent()
{
	return true;
}

void vbo::MainScreenIconLabelsVertexBuffer::Initialise(DX::DeviceResources* resources)
{
	// Coordinates used in the vertex buffer depend on the window size
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
	m_subBufferVertexIndices.resize(2);
	m_subBufferVertexIndices[0] = 0;
	const float maxTextHeightPixels = 1.2f * marginLogicalInches * dpi;
	orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[0], hIcon1Left, hIconBottom, hIcon2Left - hIcon1Left, hIconBottom - hIconLabelBottom, maxTextHeightPixels, size, font::Gravity::CENTER, font::Gravity::CENTER);
	bufferIndex += 6 * labels[0].length();
	orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[1], hIcon2Left, hIconBottom, hIcon3Left - hIcon2Left, hIconBottom - hIconLabelBottom, maxTextHeightPixels, size, font::Gravity::CENTER, font::Gravity::CENTER);
	bufferIndex += 6 * labels[1].length();
	orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[2], hIcon3Left, hIconBottom, hIcon4Left - hIcon3Left, hIconBottom - hIconLabelBottom, maxTextHeightPixels, size, font::Gravity::CENTER, font::Gravity::CENTER);
	bufferIndex += 6 * labels[2].length();
	orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[3], hIcon4Left, hIconBottom, hIcon4Right - hIcon4Left, hIconBottom - hIconLabelBottom, maxTextHeightPixels, size, font::Gravity::CENTER, font::Gravity::CENTER);
	bufferIndex += 6 * labels[3].length();
	orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[4], w2, hLowerIconsLabelTop, w3 - w2, hLowerIconsLabelTop - h2, maxTextHeightPixels, size, font::Gravity::CENTER, font::Gravity::CENTER);
	bufferIndex += 6 * labels[4].length();
	orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[5], w8, hLowerIconsLabelTop, w9 - w8, hLowerIconsLabelTop - h2, maxTextHeightPixels, size, font::Gravity::CENTER, font::Gravity::CENTER);
	bufferIndex += 6 * labels[5].length();
	m_subBufferVertexIndices[1] = bufferIndex;

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
}
