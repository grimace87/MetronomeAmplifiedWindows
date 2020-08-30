#include "pch.h"
#include "MainSceneRenderer.h"

#include "../Common/DirectXHelper.h"
#include "../Components/Shaders/FontShader.h"
#include "SettingsHubScene.h"

using namespace MetronomeAmplifiedWindows;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
MainSceneRenderer::MainSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{
}

std::vector<shader::ClassId> MainSceneRenderer::GetRequiredShaders()
{
	return { shader::ClassId::ALPHA_TEXTURE, shader::ClassId::FONT };
}

std::vector<texture::ClassId> MainSceneRenderer::GetRequiredSizeIndependentTextures()
{
	return { texture::ClassId::WOOD_TEXTURE, texture::ClassId::FONT_TEXTURE, texture::ClassId::ICONS_TEXTURE };
}

std::vector<texture::ClassId> MainSceneRenderer::GetRequiredSizeDependentTextures()
{
	return { texture::ClassId::OVERLAY_TEXTURE };
}

std::vector<vbo::ClassId> MainSceneRenderer::GetRequiredSizeIndependentVertexBuffers()
{
	return {};
}

std::vector<vbo::ClassId> MainSceneRenderer::GetRequiredSizeDependentVertexBuffers()
{
	return { vbo::ClassId::BG, vbo::ClassId::MAIN_SCREEN_TRANSLUCENT_OVERLAY, vbo::ClassId::MAIN_SCREEN_ICONS, vbo::ClassId::MAIN_SCREEN_ICON_LABELS };
}

// Called once per frame, updates the cbuffer struct as needed.
void MainSceneRenderer::Update(double timeDiffSeconds)
{
}

// Renders one frame using the vertex and pixel shaders.
void MainSceneRenderer::Render()
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
	auto overlayVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::MAIN_SCREEN_TRANSLUCENT_OVERLAY);
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
	auto iconsVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::MAIN_SCREEN_ICONS);
	iconsVertexBuffer->Activate(context);
	context->Draw(
		iconsVertexBuffer->VerticesInSubBuffer(0),
		0
	);

	// Set font shader
	shader::FontShader* fontShader = dynamic_cast<shader::FontShader*>(m_deviceResources->GetShader(shader::ClassId::FONT));
	fontShader->SetPaintColor(0.96f, 0.87f, 0.70f, 1.0f);
	fontShader->Activate(context);

	// Set the font texture
	auto fontTexture = m_deviceResources->GetTexture(texture::ClassId::FONT_TEXTURE);
	fontTexture->Activate(context);

	// Draw icon labels
	auto fontVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::MAIN_SCREEN_ICON_LABELS);
	fontVertexBuffer->Activate(context);
	context->Draw(
		fontVertexBuffer->VerticesInSubBuffer(0),
		0
	);
}

void MainSceneRenderer::OnPointerPressed(StackHost* stackHost, float normalisedX, float normalisedY)
{
	stackHost->pushScene(new SettingsHubScene(m_deviceResources));
}
