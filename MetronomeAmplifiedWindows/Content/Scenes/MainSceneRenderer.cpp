#include "pch.h"
#include "MainSceneRenderer.h"

#include "..\Common\DirectXHelper.h"

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
	return { vbo::ClassId::MAIN_SCREEN_BG, vbo::ClassId::ICON_LABELS };
}

// Called once per frame, updates the cbuffer struct as needed.
void MainSceneRenderer::Update(DX::StepTimer const& timer)
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

	// Get and activate the vertex buffer
	auto backgroundVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::MAIN_SCREEN_BG);
	backgroundVertexBuffer->Activate(context);

	// Draw the objects.
	UINT vertexTotal = backgroundVertexBuffer->GetVertexCount();
	context->Draw(
		6,
		0
	);

	// Set the texture for the translucent overlay vertices
	auto overlayTexture = m_deviceResources->GetTexture(texture::ClassId::OVERLAY_TEXTURE);
	overlayTexture->Activate(context);

	// Set the sampler state
	m_deviceResources->ActivatePointSamplerState();

	// Draw the objects.
	context->Draw(
		vertexTotal - 42,
		6
	);

	// Set the texture for the UI icon vertices
	auto iconsTexture = m_deviceResources->GetTexture(texture::ClassId::ICONS_TEXTURE);
	iconsTexture->Activate(context);

	// Set the sampler state
	m_deviceResources->ActivateLinearSamplerState();

	// Draw the objects.
	context->Draw(
		36,
		vertexTotal - 36
	);

	// Set font shader
	shader::FontShader* fontShader = dynamic_cast<shader::FontShader*>(m_deviceResources->GetShader(shader::ClassId::FONT));
	fontShader->SetPaintColor(0.96f, 0.87f, 0.70f, 1.0f);
	fontShader->Activate(context);

	// Set the font texture
	auto fontTexture = m_deviceResources->GetTexture(texture::ClassId::FONT_TEXTURE);
	fontTexture->Activate(context);

	// Set text VBO
	auto fontVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::ICON_LABELS);
	fontVertexBuffer->Activate(context);
	auto fontVertexCount = fontVertexBuffer->GetVertexCount();

	// Draw the objects.
	context->Draw(
		fontVertexCount,
		0
	);
}
