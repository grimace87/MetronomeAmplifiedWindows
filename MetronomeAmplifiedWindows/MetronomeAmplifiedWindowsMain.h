#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
//#include "Content\Sample3DSceneRenderer.h"
//#include "Content\SampleFpsTextRenderer.h"
#include "Content\MainSceneRenderer.h"

// Renders Direct2D and 3D content on the screen.
namespace MetronomeAmplifiedWindows
{
	class MetronomeAmplifiedWindowsMain : public DX::IDeviceNotify
	{
	public:
		MetronomeAmplifiedWindowsMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~MetronomeAmplifiedWindowsMain();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// TODO: Replace with your own content renderers.
		// std::unique_ptr<Sample3DSceneRenderer> m_sceneRenderer;
		// std::unique_ptr<SampleFpsTextRenderer> m_fpsTextRenderer;
		std::unique_ptr<MainSceneRenderer> m_mainScene;

		// Rendering loop timer.
		DX::StepTimer m_timer;
	};
}