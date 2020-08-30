#include "pch.h"
#include "SettingsNavigationScene.h"

#include "../Common/DirectXHelper.h"
#include "../Components/Shaders/AlphaTextureTransformShader.h"
#include "../Components/Shaders/FontTransformShader.h"

using namespace MetronomeAmplifiedWindows;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
SettingsNavigationScene::SettingsNavigationScene(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources),
	m_identityMatrix{ DirectX::XMMatrixIdentity() },
	m_transformLeftMatrix{ DirectX::XMMatrixIdentity() },
	m_transformRightMatrix{ DirectX::XMMatrixIdentity() },
	m_isAnimating(false),
	m_focusCard(0),
	m_animateToTheRight(false),
	m_animationProgress(0.0f)
{
}

std::vector<shader::ClassId> SettingsNavigationScene::GetRequiredShaders()
{
	return { shader::ClassId::ALPHA_TRANSFORM_TEXTURE, shader::ClassId::FONT_TRANSFORM };
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
void SettingsNavigationScene::Update(double timeDiffSeconds)
{
	UpdateMatrices(timeDiffSeconds);
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
	auto mainShader = dynamic_cast<shader::AlphaTextureTransformShader*>(m_deviceResources->GetShader(shader::ClassId::ALPHA_TRANSFORM_TEXTURE));
	mainShader->SetTransform(m_identityMatrix);
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
	shader::FontTransformShader* fontShader = dynamic_cast<shader::FontTransformShader*>(m_deviceResources->GetShader(shader::ClassId::FONT_TRANSFORM));
	fontShader->SetPaintColor(1.0f, 1.0f, 1.0f, 1.0f);
	fontShader->SetTransform(m_identityMatrix);
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

void SettingsNavigationScene::MoveToNext()
{
	if (m_isAnimating || (m_focusCard >= 7)) {
		return;
	}
	m_focusCard++;
	m_isAnimating = true;
	m_animateToTheRight = false;
	m_animationProgress = 0.0f;
}

void SettingsNavigationScene::MoveToPrevious()
{
	if (m_isAnimating || (m_focusCard == 0)) {
		return;
	}
	m_focusCard--;
	m_isAnimating = true;
	m_animateToTheRight = true;
	m_animationProgress = 0.0f;
}

void SettingsNavigationScene::UpdateMatrices(double timeDeltaSeconds)
{
	const float animationDuration = 0.3f;
	if (m_isAnimating) {
		m_animationProgress += (float)(timeDeltaSeconds / animationDuration);
		if (m_animationProgress >= 1.0f) {
			m_transformRightMatrix = DirectX::XMMatrixIdentity();
			m_animationProgress = 1.0f;
			m_isAnimating = false;
			return;
		}
		const float scaleOut = 0.66666667f + animationDuration / (9.0f * m_animationProgress + 3.0f * animationDuration);
		const float scaleIn = 0.66666667f + animationDuration / (9.0f * (1.0f - m_animationProgress) + 3.0f * animationDuration);

		if (m_animateToTheRight) {
			m_transformLeftMatrix = DirectX::XMMatrixMultiply(
				DirectX::XMMatrixTranslation(2.0f * (-1.0f + m_animationProgress) / scaleIn, 0.0f, 0.0f),
				DirectX::XMMatrixScaling(scaleIn, scaleIn, 1.0f));
			m_transformRightMatrix = DirectX::XMMatrixMultiply(
				DirectX::XMMatrixTranslation(2.0f * m_animationProgress / scaleOut, 0.0f, 0.0f),
				DirectX::XMMatrixScaling(scaleOut, scaleOut, 1.0f));
		} else {
			m_transformLeftMatrix = DirectX::XMMatrixMultiply(
				DirectX::XMMatrixTranslation(-2.0f * m_animationProgress / scaleOut, 0.0f, 0.0f),
				DirectX::XMMatrixScaling(scaleOut, scaleOut, 1.0f));
			m_transformRightMatrix = DirectX::XMMatrixMultiply(
				DirectX::XMMatrixTranslation(2.0f * (1.0f - m_animationProgress) / scaleIn, 0.0f, 0.0f),
				DirectX::XMMatrixScaling(scaleIn, scaleIn, 1.0f));
		}
	} else {
		m_transformLeftMatrix = DirectX::XMMatrixIdentity();
	}
}
