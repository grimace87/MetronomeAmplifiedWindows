#include "pch.h"
#include "SettingsHubLabelsVertexBuffer.h"

#include "../../../Common/DeviceResources.h"

vbo::SettingsHubLabelsVertexBuffer::SettingsHubLabelsVertexBuffer()
{
}

bool vbo::SettingsHubLabelsVertexBuffer::IsSizeDependent()
{
	return true;
}

void vbo::SettingsHubLabelsVertexBuffer::Initialise(DX::DeviceResources* resources)
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
	const float leftX = -1.0f + marginUnitsW;
	const float rightX = 1.0f - marginUnitsW;
	const float t1 = 1.0f - 2.0f * marginUnitsH;
	const float t2 = t1 - 2.0f * marginUnitsH;
	const float t3 = t2 - 2.0f * marginUnitsH;
	const float t4 = t3 - 2.0f * marginUnitsH;
	const float t5 = t4 - 2.0f * marginUnitsH;
	const float t6 = t5 - 2.0f * marginUnitsH;

	std::vector<structures::VertexTexCoord> vboData;
	std::vector<std::string> labels = {
		"Help Sections",
		"Navigating the App",
		"Controlling the Experience",
		"Crafting Your Song",
		"Managing Song Files"
	};
	int totalStructCount = 0;
	for (auto& label : labels) {
		totalStructCount += 6 * label.length();
	}
	vboData.resize(totalStructCount);

	int bufferIndex = 0;
	m_subBufferVertexIndices.resize(3);
	m_subBufferVertexIndices[0] = 0;
	const float maxTextHeightPixels = 1.2f * marginLogicalInches * dpi;
	orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[0], leftX, t1, rightX - leftX, t1 - t2, maxTextHeightPixels, size, font::Gravity::START, font::Gravity::CENTER);
	bufferIndex += 6 * labels[0].length();
	m_subBufferVertexIndices[1] = bufferIndex;
	orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[1], leftX, t2, rightX - leftX, t2 - t3, maxTextHeightPixels, size, font::Gravity::START, font::Gravity::CENTER);
	bufferIndex += 6 * labels[1].length();
	orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[2], leftX, t3, rightX - leftX, t3 - t4, maxTextHeightPixels, size, font::Gravity::START, font::Gravity::CENTER);
	bufferIndex += 6 * labels[2].length();
	orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[3], leftX, t4, rightX - leftX, t4 - t5, maxTextHeightPixels, size, font::Gravity::START, font::Gravity::CENTER);
	bufferIndex += 6 * labels[3].length();
	orkney->PrintTextIntoVbo(vboData, bufferIndex, labels[4], leftX, t5, rightX - leftX, t5 - t6, maxTextHeightPixels, size, font::Gravity::START, font::Gravity::CENTER);
	bufferIndex += 6 * labels[4].length();
	m_subBufferVertexIndices[2] = bufferIndex;

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
