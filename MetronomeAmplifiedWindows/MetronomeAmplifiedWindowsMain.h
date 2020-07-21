#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\Traits.h"

// Renders Direct2D and 3D content on the screen.
namespace MetronomeAmplifiedWindows
{
	class MetronomeAmplifiedWindowsMain : public DX::IDeviceNotify
	{
	public:
		MetronomeAmplifiedWindowsMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~MetronomeAmplifiedWindowsMain();
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();
		void OnPointerPressed(float normalisedX, float normalisedY);

		// IDeviceNotify
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Stack of renderable scenes
		std::stack<Scene*> m_sceneStack;
		Scene* GetTopScene();

		// Rendering loop timer.
		DX::StepTimer m_timer;
	};
}