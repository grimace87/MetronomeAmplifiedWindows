#include "pch.h"
#include "FontTexture.h"

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
