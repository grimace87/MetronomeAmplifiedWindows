#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
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
		// Utility functions
		void putSquare(VertexTexCoord buffer[], int index, float x1, float y1, float x2, float y2, float s1, float t1, float s2, float t2);
		std::vector<byte> MainSceneRenderer::makeOverlayTextureData(_Out_ int* width, _Out_ int* height);
		void CreateTextureFromRawPixelData(std::vector<byte>& pixelData, int width, int height, ID3D11Resource** texture, ID3D11ShaderResourceView** textureView);

		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11Buffer>		      m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Resource>            m_woodenTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_woodenTextureView;
		Microsoft::WRL::ComPtr<ID3D11Resource>            m_overlayTexture;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_overlayTextureView;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>        m_samplerStateLinear;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>        m_samplerStatePoint;
		Microsoft::WRL::ComPtr<ID3D11BlendState>          m_blendState;

		// System resources for geometry (e.g. shader structure for cbuffer if using one).
		uint32 m_backgroundVertexCount;
		uint32 m_overlayVertexCount;

		// Variables used with the rendering loop.
		bool m_deviceDependentLoadingComplete;
		bool m_windowDependentLoadingComplete;
	};
}

