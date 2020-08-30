#include "pch.h"
#include "MetronomeAmplifiedWindowsMain.h"
#include "Common\DirectXHelper.h"

#include "Content/Scenes/MainSceneRenderer.h"

using namespace MetronomeAmplifiedWindows;

// Loads and initializes application assets when the application is loaded.
MetronomeAmplifiedWindowsMain::MetronomeAmplifiedWindowsMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{
	// Register to be notified if the Device is lost or recreated
	m_deviceResources->RegisterDeviceNotify(this);

	// Content initialisation
	Scene* firstScene = new MainSceneRenderer(m_deviceResources);
	m_sceneStack.push(firstScene);

	// Initialise time-related variables
	m_lastTimeUpdate = m_timer.GetElapsedTicks();
}

MetronomeAmplifiedWindowsMain::~MetronomeAmplifiedWindowsMain()
{
	// Deregister device notification
	m_deviceResources->RegisterDeviceNotify(nullptr);
}

// Updates the application state once per frame.
void MetronomeAmplifiedWindowsMain::Update() 
{
	// Update scene objects.
	m_timer.Tick([&]()
	{
		uint64_t currentTicks = m_timer.GetElapsedTicks();
		double timeDiffSeconds = m_timer.TicksToSeconds(currentTicks - m_lastTimeUpdate);
		m_lastTimeUpdate = currentTicks;
		Scene* topScene = GetTopScene();
		if (topScene != nullptr) {
			topScene->Update(timeDiffSeconds);
		}
	});
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool MetronomeAmplifiedWindowsMain::Render()
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0 || !m_deviceResources->AreShadersFulfilled() || !m_deviceResources->AreTexturesFulfilled() || !m_deviceResources->AreVertexBuffersFulfilled())
	{
		return false;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Reset the viewport to target the whole screen.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// Reset render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, nullptr);

	// Clear the back buffer and depth stencil view.
	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::Black);

	// Render the scene objects.
	Scene* topScene = GetTopScene();
	if (topScene != nullptr) {
		topScene->Render();
	}

	return true;
}

void MetronomeAmplifiedWindowsMain::OnPointerPressed(float normalisedX, float normalisedY)
{
	Scene* topScene = GetTopScene();
	if (topScene != nullptr) {
		topScene->OnPointerPressed(this, normalisedX, normalisedY);
	}
}

// Notifies renderers that device resources need to be released.
void MetronomeAmplifiedWindowsMain::OnDeviceLost()
{
	m_deviceResources->ClearShaderCache();
	m_deviceResources->ClearTextureCache();
	m_deviceResources->ClearVertexBufferCache();
}

// Notifies renderers that device resources may now be recreated.
void MetronomeAmplifiedWindowsMain::OnDeviceRestored()
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void MetronomeAmplifiedWindowsMain::CreateDeviceDependentResources()
{
	Scene* topScene = GetTopScene();

	// Load shaders and textures if needed
	if (topScene != nullptr) {
		m_deviceResources->RequireShaders(topScene->GetRequiredShaders());
		m_deviceResources->RequireSizeIndependentTextures(topScene->GetRequiredSizeIndependentTextures());
		m_deviceResources->RequireSizeIndependentVertexBuffers(topScene->GetRequiredSizeIndependentVertexBuffers());
	}
}

// Updates application state when the window size changes (e.g. device orientation change)
void MetronomeAmplifiedWindowsMain::CreateWindowSizeDependentResources()
{
	Scene* topScene = GetTopScene();

	// Invalidate size-dependent resources, then re-create any that are needed now
	if (topScene != nullptr) {
		m_deviceResources->InvalidateSizeDependentResources();
		m_deviceResources->RequireSizeDependentTextures(topScene->GetRequiredSizeDependentTextures());
		m_deviceResources->RequireSizeDependentVertexBuffers(topScene->GetRequiredSizeDependentVertexBuffers());
	}
}

Scene* MetronomeAmplifiedWindowsMain::GetTopScene()
{
	if (m_sceneStack.empty()) {
		return nullptr;
	}
	return m_sceneStack.top();
}

void MetronomeAmplifiedWindowsMain::pushScene(Scene* newScene)
{
	m_sceneStack.push(newScene);
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void MetronomeAmplifiedWindowsMain::popScene()
{
	if (!m_sceneStack.empty()) {
		m_sceneStack.pop();
	}
}
