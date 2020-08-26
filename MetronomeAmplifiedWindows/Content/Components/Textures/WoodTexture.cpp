#include "pch.h"
#include "WoodTexture.h"

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
