#include "pch.h"
#include "SettingsHubScene.h"

#include "../Common/DirectXHelper.h"
#include "../Components/Shaders/FontShader.h"
#include "SettingsNavigationScene.h"

using namespace MetronomeAmplifiedWindows;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
SettingsHubScene::SettingsHubScene(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{
}

std::vector<shader::ClassId> SettingsHubScene::GetRequiredShaders()
{
	return { shader::ClassId::ALPHA_TEXTURE, shader::ClassId::FONT };
}

std::vector<texture::ClassId> SettingsHubScene::GetRequiredSizeIndependentTextures()
{
	return { texture::ClassId::WOOD_TEXTURE, texture::ClassId::FONT_TEXTURE };
}

std::vector<texture::ClassId> SettingsHubScene::GetRequiredSizeDependentTextures()
{
	return {};
}

std::vector<vbo::ClassId> SettingsHubScene::GetRequiredSizeIndependentVertexBuffers()
{
	return {};
}

std::vector<vbo::ClassId> SettingsHubScene::GetRequiredSizeDependentVertexBuffers()
{
	return { vbo::ClassId::BG, vbo::ClassId::SETTINGS_HUB_LABELS };
}

// Called once per frame, updates the cbuffer struct as needed.
void SettingsHubScene::Update(double timeDiffSeconds)
{
}

// Renders one frame using the vertex and pixel shaders.
void SettingsHubScene::Render()
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
	auto backgroundVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::BG);
	backgroundVertexBuffer->Activate(context);

	// Draw the objects.
	context->Draw(
		backgroundVertexBuffer->VerticesInSubBuffer(0),
		0
	);

	// Set font shader
	shader::FontShader* fontShader = dynamic_cast<shader::FontShader*>(m_deviceResources->GetShader(shader::ClassId::FONT));
	fontShader->SetPaintColor(1.0f, 1.0f, 1.0f, 1.0f);
	fontShader->Activate(context);

	// Set the font texture
	auto fontTexture = m_deviceResources->GetTexture(texture::ClassId::FONT_TEXTURE);
	fontTexture->Activate(context);

	// Set text VBO
	auto fontVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::SETTINGS_HUB_LABELS);
	fontVertexBuffer->Activate(context);

	// Draw the first line of text
	context->Draw(
		fontVertexBuffer->VerticesInSubBuffer(0),
		fontVertexBuffer->IndexOfSubBuffer(0)
	);

	// Update the paint colour and draw the rest
	fontShader->SetPaintColor(0.96f, 0.87f, 0.70f, 1.0f);
	fontShader->Activate(context);
	context->Draw(
		fontVertexBuffer->VerticesInSubBuffer(1),
		fontVertexBuffer->IndexOfSubBuffer(1)
	);
}

void SettingsHubScene::OnPointerPressed(StackHost* stackHost, float normalisedX, float normalisedY)
{
	// Make sure VBOs are initialised
	if (!m_deviceResources->AreShadersFulfilled() || !m_deviceResources->AreTexturesFulfilled() || !m_deviceResources->AreVertexBuffersFulfilled())
	{
		return;
	}

	// Check for region 0 in text labels VBO
	auto textsVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::SETTINGS_HUB_LABELS);
	int vboRegion = textsVertexBuffer->RegionOfInterestAt(normalisedX, normalisedY);
	if (vboRegion == 0) {
		stackHost->pushScene(new SettingsNavigationScene(m_deviceResources));
	}
}
