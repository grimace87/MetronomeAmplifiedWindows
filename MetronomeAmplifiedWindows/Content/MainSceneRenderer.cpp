#include "pch.h"
#include "MainSceneRenderer.h"

#include "..\Common\DirectXHelper.h"

using namespace MetronomeAmplifiedWindows;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
MainSceneRenderer::MainSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
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

	auto mainShader = m_deviceResources->GetShader(shader::ClassId::ALPHA_TEXTURE);
	mainShader->Activate(context);

	// Set the blend state
	UINT sampleMask = 0xffffffff;
	context->OMSetBlendState(m_deviceResources->GetBlendState(), NULL, sampleMask);

	// Set the texture for the first few vertices
	auto woodenTexture = m_deviceResources->GetTexture(texture::ClassId::WOOD_TEXTURE);
	woodenTexture->Activate(context);

	// Set the sampler state
	context->PSSetSamplers(0, 1, m_deviceResources->GetLinearSamplerState());

	// Get and activate the vertex buffer
	auto backgroundVertexBuffer = m_deviceResources->GetVertexBuffer(vbo::ClassId::MAIN_SCREEN_BG);
	backgroundVertexBuffer->Activate(context);

	// Draw the objects.
	UINT vertexTotal = backgroundVertexBuffer->GetVertexCount();
	context->Draw(
		6,
		0
	);

	// Set the texture for the remaining vertices
	auto overlayTexture = m_deviceResources->GetTexture(texture::ClassId::OVERLAY_TEXTURE);
	overlayTexture->Activate(context);

	// Set the sampler state
	context->PSSetSamplers(0, 1, m_deviceResources->GetPointSamplerState());

	// Draw the objects.
	context->Draw(
		vertexTotal - 6,
		6
	);
}

void MainSceneRenderer::CreateDeviceDependentResources()
{
	// Load shaders and textures if needed
    m_deviceResources->RequireShaders({ shader::ClassId::ALPHA_TEXTURE });
	m_deviceResources->RequireSizeIndependentTextures({ texture::ClassId::WOOD_TEXTURE });
}

// Initializes view parameters when the window size changes.
void MainSceneRenderer::CreateWindowSizeDependentResources()
{
	// Invalidate size-dependent resources
	m_deviceResources->InvalidateSizeDependentResources();

	// (Re-)create any size-dependent resources
	m_deviceResources->RequireSizeDependentTextures({ texture::ClassId::OVERLAY_TEXTURE });
	m_deviceResources->RequireSizeDependentVertexBuffers({ vbo::ClassId::MAIN_SCREEN_BG });
}

void MainSceneRenderer::ReleaseDeviceDependentResources()
{
    // TODO - Verify cache is not being totally emptied merely from a change of scenes

	m_deviceResources->ClearShaderCache();
	m_deviceResources->ClearTextureCache();
	m_deviceResources->ClearVertexBufferCache();
}