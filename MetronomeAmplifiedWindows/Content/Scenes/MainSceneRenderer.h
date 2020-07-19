#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "..\Traits.h"

namespace MetronomeAmplifiedWindows
{
	class MainSceneRenderer : public Scene
	{
	public:
		MainSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);

		// Renderable
		virtual void Render() override;

		// UsesCachedResources
		virtual std::vector<shader::ClassId> GetRequiredShaders() override;
		virtual std::vector<texture::ClassId> GetRequiredSizeIndependentTextures() override;
		virtual std::vector<texture::ClassId> GetRequiredSizeDependentTextures() override;
		virtual std::vector<vbo::ClassId> GetRequiredSizeIndependentVertexBuffers() override;
		virtual std::vector<vbo::ClassId> GetRequiredSizeDependentVertexBuffers() override;

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
	};
}

