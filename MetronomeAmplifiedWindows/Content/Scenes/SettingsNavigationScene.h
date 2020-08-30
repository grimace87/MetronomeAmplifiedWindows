#pragma once

#include "../Common/DeviceResources.h"
#include "../Common/StepTimer.h"
#include "../Traits.h"

namespace MetronomeAmplifiedWindows
{
	class SettingsNavigationScene : public Scene
	{
	public:
		SettingsNavigationScene(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void ReleaseDeviceDependentResources();
		virtual void Update(double timeDiffSeconds) override;

		// Renderable
		virtual void Render() override;

		// UsesCachedResources
		virtual std::vector<shader::ClassId> GetRequiredShaders() override;
		virtual std::vector<texture::ClassId> GetRequiredSizeIndependentTextures() override;
		virtual std::vector<texture::ClassId> GetRequiredSizeDependentTextures() override;
		virtual std::vector<vbo::ClassId> GetRequiredSizeIndependentVertexBuffers() override;
		virtual std::vector<vbo::ClassId> GetRequiredSizeDependentVertexBuffers() override;

		// Scene
		virtual void OnPointerPressed(StackHost* stackHost, float normalisedX, float normalisedY) override;

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Matrices for shuffling cards around
		DirectX::XMMATRIX m_identityMatrix;
		DirectX::XMMATRIX m_transformLeftMatrix;
		DirectX::XMMATRIX m_transformRightMatrix;

		// State for animating
		bool m_isAnimating;
		int m_focusCard;
		bool m_animateToTheRight;
		float m_animationProgress;

		void MoveToNext();
		void MoveToPrevious();
		void UpdateMatrices(double timeDeltaSeconds);
	};
}

