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

	// Get shaders, textures and VBOs
	auto mainShader = dynamic_cast<shader::AlphaTextureTransformShader*>(m_deviceResources->GetShader(shader::ClassId::ALPHA_TRANSFORM_TEXTURE));
	shader::FontTransformShader* fontShader = dynamic_cast<shader::FontTransformShader*>(m_deviceResources->GetShader(shader::ClassId::FONT_TRANSFORM));
	auto woodenTexture = m_deviceResources->GetTexture(texture::ClassId::WOOD_TEXTURE);
	auto overlayTexture = m_deviceResources->GetTexture(texture::ClassId::OVERLAY_TEXTURE);
	auto iconsTexture = m_deviceResources->GetTexture(texture::ClassId::ICONS_TEXTURE);
	auto screenshotsTexture = m_deviceResources->GetTexture(texture::ClassId::SAMPLE_IMAGE);
	auto fontTexture = m_deviceResources->GetTexture(texture::ClassId::FONT_TEXTURE);
	auto backgroundVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::BG);
	auto overlayVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::HELP_DETAILS_OVERLAY);
	auto iconsVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::HELP_DETAILS_ICONS);
	auto screenshotsVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::HELP_NAVIGATING_IMAGES);
	auto fontVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::HELP_NAVIGATING_TEXTS);

	// Draw the background
	mainShader->SetTransform(m_identityMatrix);
	mainShader->Activate(context);
	woodenTexture->Activate(context);
	m_deviceResources->ActivateBlendState();
	m_deviceResources->ActivateLinearSamplerState();
	backgroundVertexBuffer->Activate(context);
	context->Draw(
		backgroundVertexBuffer->VerticesInSubBuffer(0),
		0
	);
	
	// Draw the overlay and the sample image, once or twice depending on animation state
	m_deviceResources->ActivatePointSamplerState();
	if (!m_isAnimating) {

		overlayTexture->Activate(context);
		overlayVertexBuffer->Activate(context);
		context->Draw(
			overlayVertexBuffer->VerticesInSubBuffer(0),
			0
		);

		screenshotsTexture->Activate(context);
		screenshotsVertexBuffer->Activate(context);
		context->Draw(
			screenshotsVertexBuffer->VerticesInSubBuffer(0),
			0
		);
	}
	else {

		// Draw overlay twice
		overlayTexture->Activate(context);
		overlayVertexBuffer->Activate(context);
		mainShader->SetTransform(m_transformLeftMatrix);
		mainShader->Activate(context);
		context->Draw(
			overlayVertexBuffer->VerticesInSubBuffer(0),
			0
		);
		mainShader->SetTransform(m_transformRightMatrix);
		mainShader->Activate(context);
		context->Draw(
			overlayVertexBuffer->VerticesInSubBuffer(0),
			0
		);

		mainShader->SetTransform(m_identityMatrix);
		mainShader->Activate(context);

		// Draw image twice
		screenshotsTexture->Activate(context);
		screenshotsVertexBuffer->Activate(context);
		mainShader->SetTransform(m_transformLeftMatrix);
		mainShader->Activate(context);
		context->Draw(
			screenshotsVertexBuffer->VerticesInSubBuffer(0),
			0
		);
		mainShader->SetTransform(m_transformRightMatrix);
		mainShader->Activate(context);
		context->Draw(
			screenshotsVertexBuffer->VerticesInSubBuffer(0),
			0
		);
	}

	// Draw icons
	mainShader->SetTransform(m_identityMatrix);
	mainShader->Activate(context);
	iconsTexture->Activate(context);
	m_deviceResources->ActivateLinearSamplerState();
	iconsVertexBuffer->Activate(context);
	context->Draw(
		iconsVertexBuffer->VerticesInSubBuffer(0),
		0
	);
	
	// Draw heading in white
	fontShader->SetPaintColor(1.0f, 1.0f, 1.0f, 1.0f);
	fontShader->SetTransform(m_identityMatrix);
	fontShader->Activate(context);
	fontTexture->Activate(context);
	fontVertexBuffer->Activate(context);
	context->Draw(
		fontVertexBuffer->VerticesInSubBuffer(0),
		fontVertexBuffer->IndexOfSubBuffer(0)
	);
	
	// Change font colour
	fontShader->SetPaintColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Draw one or two contents sections depending on animation state
	if (!m_isAnimating) {
		fontShader->Activate(context);
		context->Draw(
			fontVertexBuffer->VerticesInSubBuffer(m_focusCard + 1),
			fontVertexBuffer->IndexOfSubBuffer(m_focusCard + 1)
		);
	}
	else {
		int leftSide;
		if (m_animateToTheRight) {
			leftSide = m_focusCard;
		}
		else {
			leftSide = m_focusCard - 1;
		}

		fontShader->SetTransform(m_transformLeftMatrix);
		fontShader->Activate(context);
		context->Draw(
			fontVertexBuffer->VerticesInSubBuffer(leftSide + 1),
			fontVertexBuffer->IndexOfSubBuffer(leftSide + 1)
		);

		fontShader->SetTransform(m_transformRightMatrix);
		fontShader->Activate(context);
		context->Draw(
			fontVertexBuffer->VerticesInSubBuffer(leftSide + 2),
			fontVertexBuffer->IndexOfSubBuffer(leftSide + 2)
		);
	}
}

void SettingsNavigationScene::OnPointerPressed(StackHost* stackHost, float normalisedX, float normalisedY)
{
	// Make sure VBOs are initialised
	if (!m_deviceResources->AreShadersFulfilled() || !m_deviceResources->AreTexturesFulfilled() || !m_deviceResources->AreVertexBuffersFulfilled())
	{
		return;
	}

	// Check for regions 0 and 1 in icons VBO
	auto iconsVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::HELP_DETAILS_ICONS);
	int vboRegion = iconsVertexBuffer->RegionOfInterestAt(normalisedX, normalisedY);
	if (vboRegion == 0) {
		MoveToPrevious();
	}
	else if (vboRegion == 1) {
		MoveToNext();
	}
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
			m_transformLeftMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(
				DirectX::XMMatrixTranslation(2.0f * (-1.0f + m_animationProgress) / scaleIn, 0.0f, 0.0f),
				DirectX::XMMatrixScaling(scaleIn, scaleIn, 1.0f)));
			m_transformRightMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(
				DirectX::XMMatrixTranslation(2.0f * m_animationProgress / scaleOut, 0.0f, 0.0f),
				DirectX::XMMatrixScaling(scaleOut, scaleOut, 1.0f)));
		} else {
			m_transformLeftMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(
				DirectX::XMMatrixTranslation(-2.0f * m_animationProgress / scaleOut, 0.0f, 0.0f),
				DirectX::XMMatrixScaling(scaleOut, scaleOut, 1.0f)));
			m_transformRightMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(
				DirectX::XMMatrixTranslation(2.0f * (1.0f - m_animationProgress) / scaleIn, 0.0f, 0.0f),
				DirectX::XMMatrixScaling(scaleIn, scaleIn, 1.0f)));
		}
	} else {
		m_transformLeftMatrix = DirectX::XMMatrixIdentity();
	}
}
