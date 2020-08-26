#include "pch.h"
#include "OverlayTexture.h"

#include "../../../Common/DeviceResources.h"

texture::OverlayTexture::OverlayTexture() : BaseTexture()
{
}

bool texture::OverlayTexture::IsSizeDependent()
{
	return true;
}

Concurrency::task<void> texture::OverlayTexture::MakeInitTask(DX::DeviceResources* resources)
{
	return Concurrency::create_task([this, resources]() -> void {

		// Determine sizes and create the data array
		const float screenDpi = resources->GetDpi();
		const float cornerRadiusLogicalInches = 0.25f;
		const byte alphaLevel = 0x80;
		const int cornerRadiusPixels = (int)(cornerRadiusLogicalInches * screenDpi);
		const int rowStrideBytes = 4 * 2 * cornerRadiusPixels;
		const int sectionOffsetBytes = 4 * cornerRadiusPixels;
		std::vector<byte> textureData;
		textureData.resize(rowStrideBytes * cornerRadiusPixels * 2);
		std::fill(textureData.begin(), textureData.end(), 0xff);

		// Output dimensions
		int width = 2 * cornerRadiusPixels;
		int height = 2 * cornerRadiusPixels;

		// Generate the top-left section (rounded corner)
		for (int j = 0; j < cornerRadiusPixels; j++) {
			int index = j * rowStrideBytes;
			const int transparentPixels = (int)(cornerRadiusPixels - sqrt(max(0.0, 2.0 * j * cornerRadiusPixels - j * j)));
			for (int i = 0; i < cornerRadiusPixels; i++) {
				const byte pixelAlpha = i <= transparentPixels ? 0 : alphaLevel;
				textureData[index + 3] = pixelAlpha;
				index += 4;
			}
		}

		// Generate the top-right section (solid colour)
		for (int j = 0; j < cornerRadiusPixels; j++) {
			int index = j * rowStrideBytes + sectionOffsetBytes;
			for (int i = 0; i < cornerRadiusPixels; i++) {
				textureData[index + 3] = alphaLevel;
				index += 4;
			}
		}

		// Generate the bottom-left section (inner corner)
		for (int j = 0; j < cornerRadiusPixels; j++) {
			int index = (cornerRadiusPixels + j) * rowStrideBytes;
			const int transparentPixels = (int)sqrt(max(0.0, 2.0 * j * cornerRadiusPixels - j * j));
			for (int i = 0; i < cornerRadiusPixels; i++) {
				const int pixelAlpha = i <= transparentPixels ? 0 : alphaLevel;
				textureData[index + 3] = pixelAlpha;
				index += 4;
			}
		}

		// Generate the right section (fully transparent)
		for (int j = 0; j < cornerRadiusPixels; j++) {
			int index = (cornerRadiusPixels + j) * rowStrideBytes + sectionOffsetBytes;
			for (int i = 0; i < cornerRadiusPixels; i++) {
				textureData[index + 3] = 0;
				index += 4;
			}
		}

		// Create texture resources (kept in base class)
		MakeTextureFromMemory(resources, textureData, width, height);
		});
}
