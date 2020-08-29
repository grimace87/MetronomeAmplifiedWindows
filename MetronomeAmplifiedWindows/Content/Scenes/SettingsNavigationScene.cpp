#include "pch.h"
#include "SettingsNavigationScene.h"

#include "../Common/DirectXHelper.h"
#include "../Components/Shaders/FontShader.h"

using namespace MetronomeAmplifiedWindows;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
SettingsNavigationScene::SettingsNavigationScene(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{
}

std::vector<shader::ClassId> SettingsNavigationScene::GetRequiredShaders()
{
	return { shader::ClassId::ALPHA_TEXTURE, shader::ClassId::FONT };
}

std::vector<texture::ClassId> SettingsNavigationScene::GetRequiredSizeIndependentTextures()
{
	return { texture::ClassId::WOOD_TEXTURE, texture::ClassId::FONT_TEXTURE, texture::ClassId::ICONS_TEXTURE, texture::ClassId::SAMPLE_IMAGE };
}

std::vector<texture::ClassId> SettingsNavigationScene::GetRequiredSizeDependentTextures()
{
	return { texture::ClassId::OVERLAY_TEXTURE };
}

std::vector<vbo::ClassId> SettingsNavigationScene::GetRequiredSizeIndependentVertexBuffers()
{
	return {};
}

std::vector<vbo::ClassId> SettingsNavigationScene::GetRequiredSizeDependentVertexBuffers()
{
	return { vbo::ClassId::BG, vbo::ClassId::HELP_DETAILS_OVERLAY, vbo::ClassId::HELP_DETAILS_ICONS, vbo::ClassId::HELP_NAVIGATING_TEXTS, vbo::ClassId::HELP_NAVIGATING_IMAGES };
}

// Called once per frame, updates the cbuffer struct as needed.
void SettingsNavigationScene::Update(DX::StepTimer const& timer)
{
}

// Renders one frame using the vertex and pixel shaders.
void SettingsNavigationScene::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_deviceResources->AreShadersFulfilled() || !m_deviceResources->AreTexturesFulfilled() || !m_deviceResources->AreVertexBuffersFulfilled())
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Set main shader
	auto mainShader = m_deviceResources->GetShader(shader::ClassId::ALPHA_TEXTURE);
	mainShader->Activate(context);

	// Set the blend state
	m_deviceResources->ActivateBlendState();

	// Set the texture for the first few vertices
	auto woodenTexture = m_deviceResources->GetTexture(texture::ClassId::WOOD_TEXTURE);
	woodenTexture->Activate(context);

	// Set the sampler state
	m_deviceResources->ActivateLinearSamplerState();

	// Draw background
	auto backgroundVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::BG);
	backgroundVertexBuffer->Activate(context);
	context->Draw(
		backgroundVertexBuffer->VerticesInSubBuffer(0),
		0
	);

	// Set the texture for the translucent overlay vertices
	auto overlayTexture = m_deviceResources->GetTexture(texture::ClassId::OVERLAY_TEXTURE);
	overlayTexture->Activate(context);

	// Set the sampler state
	m_deviceResources->ActivatePointSamplerState();

	// Draw overlay
	auto overlayVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::HELP_DETAILS_OVERLAY);
	overlayVertexBuffer->Activate(context);
	context->Draw(
		overlayVertexBuffer->VerticesInSubBuffer(0),
		0
	);

	// Set the texture for the UI icon vertices
	auto iconsTexture = m_deviceResources->GetTexture(texture::ClassId::ICONS_TEXTURE);
	iconsTexture->Activate(context);

	// Set the sampler state
	m_deviceResources->ActivateLinearSamplerState();

	// Draw icons
	auto iconsVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::HELP_DETAILS_ICONS);
	iconsVertexBuffer->Activate(context);
	context->Draw(
		iconsVertexBuffer->VerticesInSubBuffer(0),
		0
	);

	// Set the texture for the screenshots
	auto screenshotsTexture = m_deviceResources->GetTexture(texture::ClassId::SAMPLE_IMAGE);
	screenshotsTexture->Activate(context);

	// Draw screenshot
	auto screenshotsVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::HELP_NAVIGATING_IMAGES);
	screenshotsVertexBuffer->Activate(context);
	context->Draw(
		screenshotsVertexBuffer->VerticesInSubBuffer(0),
		0
	);

	// Set font shader
	shader::FontShader* fontShader = dynamic_cast<shader::FontShader*>(m_deviceResources->GetShader(shader::ClassId::FONT));
	fontShader->SetPaintColor(1.0f, 1.0f, 1.0f, 1.0f);
	fontShader->Activate(context);

	// Set the font texture and VBO
	auto fontTexture = m_deviceResources->GetTexture(texture::ClassId::FONT_TEXTURE);
	fontTexture->Activate(context);
	auto fontVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::HELP_NAVIGATING_TEXTS);
	fontVertexBuffer->Activate(context);

	// Draw heading
	context->Draw(
		fontVertexBuffer->VerticesInSubBuffer(0),
		fontVertexBuffer->IndexOfSubBuffer(0)
	);

	// Change font colour
	fontShader->SetPaintColor(0.0f, 0.0f, 0.0f, 1.0f);
	fontShader->Activate(context);

	// Draw text
	context->Draw(
		fontVertexBuffer->VerticesInSubBuffer(1),
		fontVertexBuffer->IndexOfSubBuffer(1)
	);
}

void SettingsNavigationScene::OnPointerPressed(StackHost* stackHost, float normalisedX, float normalisedY)
{
	
}
