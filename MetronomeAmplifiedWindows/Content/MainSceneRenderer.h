#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"

namespace MetronomeAmplifiedWindows
{
	class MainSceneRenderer
	{
	public:
		MainSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
	};
}

