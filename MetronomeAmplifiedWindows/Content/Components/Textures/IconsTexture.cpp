#include "pch.h"
#include "IconsTexture.h"

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
