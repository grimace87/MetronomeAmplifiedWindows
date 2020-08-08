#include "pch.h"
#include "Textures.h"

#include "Common/DeviceResources.h"
#include "..\Common\DirectXHelper.h"
#include "..\Common\WICTextureLoader.h"

texture::BaseTexture::BaseTexture() : m_isValid(false)
{
}

Concurrency::task<void> texture::BaseTexture::MakeTextureFromFileTask(DX::DeviceResources* resources, std::wstring fileName)
{
	// Load an image file asynchronously
	auto loadTextureImageTask = DX::ReadDataAsync(fileName);

	// After the image file is loaded, decode it and create a texture
	return loadTextureImageTask.then([this, resources](const std::vector<byte>& fileData) {
		winrt::check_hresult(
			CreateWICTextureFromMemory(
				resources->GetD3DDevice(),
				resources->GetD3DDeviceContext(),
				fileData.data(),
				fileData.size(),
				m_textureResource.put(),
				m_textureView.put())
		);
		m_isValid = true;
		});
}

void texture::BaseTexture::MakeTextureFromMemory(DX::DeviceResources* resources, std::vector<byte>& pixelData, int width, int height)
{
	// Describe texture
	D3D11_SUBRESOURCE_DATA subData = { (const void*)pixelData.data(), width * 4 * sizeof(byte), 0 };
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;

	// Create resource
	winrt::check_hresult(
		resources->GetD3DDevice()->CreateTexture2D(
			&desc,
			&subData,
			(ID3D11Texture2D**)m_textureResource.put())
	);

	// Describe texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = 1;
	viewDesc.Texture2D.MostDetailedMip = 0;

	// Create texture view
	winrt::check_hresult(
		resources->GetD3DDevice()->CreateShaderResourceView(
			m_textureResource.get(),
			&viewDesc,
			m_textureView.put())
	);

	// Assign output parameters
	m_isValid = true;
}

texture::BaseTexture* texture::BaseTexture::NewFromClassId(texture::ClassId id) {
	switch (id) {
	case ClassId::WOOD_TEXTURE:
		return new WoodTexture();
	case ClassId::OVERLAY_TEXTURE:
		return new OverlayTexture();
	case ClassId::FONT_TEXTURE:
		return new FontTexture();
	case ClassId::ICONS_TEXTURE:
		return new IconsTexture();
	default:
		throw std::exception("Requested texture class does not exist");
	}
}

void texture::BaseTexture::Activate(ID3D11DeviceContext3* context)
{
	ID3D11ShaderResourceView* resourceView = m_textureView.get();
	context->PSSetShaderResources(0, 1, &resourceView);
}

void texture::BaseTexture::Reset()
{
	m_isValid = false;
	m_textureResource = nullptr;
	m_textureView = nullptr;
}

texture::WoodTexture::WoodTexture() : BaseTexture()
{
}

Concurrency::task<void> texture::WoodTexture::MakeInitTask(DX::DeviceResources* resources)
{
	return MakeTextureFromFileTask(resources, L"Assets\\Textures\\wood_bg_texture.jpg");
}

bool texture::WoodTexture::IsSizeDependent()
{
	return false;
}

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

texture::FontTexture::FontTexture() : BaseTexture()
{
}

Concurrency::task<void> texture::FontTexture::MakeInitTask(DX::DeviceResources* resources)
{
	return MakeTextureFromFileTask(resources, L"Assets\\Textures\\Orkney.png");
}

bool texture::FontTexture::IsSizeDependent()
{
	return false;
}

texture::IconsTexture::IconsTexture() : BaseTexture()
{
}

Concurrency::task<void> texture::IconsTexture::MakeInitTask(DX::DeviceResources* resources)
{
	return MakeTextureFromFileTask(resources, L"Assets\\Textures\\icons.png");
}

bool texture::IconsTexture::IsSizeDependent()
{
	return false;
}
