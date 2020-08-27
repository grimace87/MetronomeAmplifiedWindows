#include "pch.h"
#include "SampleImageTexture.h"

texture::SampleImageTexture::SampleImageTexture() : BaseTexture()
{
}

Concurrency::task<void> texture::SampleImageTexture::MakeInitTask(DX::DeviceResources* resources)
{
	return MakeTextureFromFileTask(resources, L"Assets\\Textures\\sample_screenshot.png");
}

bool texture::SampleImageTexture::IsSizeDependent()
{
	return false;
}
