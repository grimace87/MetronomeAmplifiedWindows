#include "pch.h"
#include "BaseVertexBuffer.h"

#include "Common/DeviceResources.h"
#include "VertexBuffers/BackgroundVertexBuffer.h"
#include "VertexBuffers/MainScreenTranslucentOverlayVertexBuffer.h"
#include "VertexBuffers/MainScreenIconsVertexBuffer.h"
#include "VertexBuffers/MainScreenIconLabelsVertexBuffer.h"
#include "VertexBuffers/SettingsHubLabelsVertexBuffer.h"
#include "VertexBuffers/SettingsDetailsTranslucentOverlayVertexBuffer.h"
#include "VertexBuffers/SettingsDetailsIconsVertexBuffer.h"
#include "VertexBuffers/SettingsNavigatingImagesVertexBuffer.h"
#include "VertexBuffers/SettingsNavigatingTextsVertexBuffer.h"

vbo::BaseVertexBuffer::BaseVertexBuffer() : m_subBufferVertexIndices{}, m_isValid(false)
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

int vbo::BaseVertexBuffer::RegionOfInterestAt(float xNormalised, float yNormalised)
{
	for (int i = 0; i < m_regionsOfInterest.size(); i++) {
		auto& region = m_regionsOfInterest[i];
		if (xNormalised > region.X && xNormalised < region.X + region.Width) {
			if (yNormalised > region.Y && yNormalised < region.Y + region.Height) {
				return i;
			}
		}
	}
	return -1;
}

vbo::BaseVertexBuffer* vbo::BaseVertexBuffer::NewFromClassId(ClassId id)
{
	switch (id) {
	case ClassId::BG:
		return new BackgroundVertexBuffer();
	case ClassId::MAIN_SCREEN_TRANSLUCENT_OVERLAY:
		return new MainScreenTranslucentOverlayVertexBuffer();
	case ClassId::MAIN_SCREEN_ICONS:
		return new MainScreenIconsVertexBuffer();
	case ClassId::MAIN_SCREEN_ICON_LABELS:
		return new MainScreenIconLabelsVertexBuffer();
	case ClassId::SETTINGS_HUB_LABELS:
		return new SettingsHubLabelsVertexBuffer();
	case ClassId::HELP_DETAILS_OVERLAY:
		return new SettingsDetailsTranslucentOverlayVertexBuffer();
	case ClassId::HELP_DETAILS_ICONS:
		return new SettingsDetailsIconsVertexBuffer();
	case ClassId::HELP_NAVIGATING_IMAGES:
		return new SettingsNavigatingImagesVertexBuffer();
	case ClassId::HELP_NAVIGATING_TEXTS:
		return new SettingsNavigatingTextsVertexBuffer();
	default:
		throw std::exception("Requested VBO class does not exist");
	}
}

void vbo::BaseVertexBuffer::Activate(ID3D11DeviceContext3* context)
{
	// Each vertex is one instance of the VertexTexCoord struct.
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
