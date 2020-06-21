#pragma once

#include "Resources/Textures.h"

#include <map>

namespace cache {

	class TextureCache {
	private:
		std::map<texture::ClassId, texture::BaseTexture*> m_textures;
		bool m_sizeIndependentTexturesAreFulfilled;
		bool m_sizeDependentTexturesAreFulfilled;
		bool m_samplerAndBlendStateFulfilled;

		Microsoft::WRL::ComPtr<ID3D11SamplerState>        m_samplerStateLinear;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>        m_samplerStatePoint;
		Microsoft::WRL::ComPtr<ID3D11BlendState>          m_blendState;

		void RequireSamplerAndBlendState(DX::DeviceResources* resources);

	public:
		TextureCache();
		bool ContainsAll(std::vector<texture::ClassId>& textureClasses);
		void RequireSizeIndependentTextures(DX::DeviceResources* resources, std::vector<texture::ClassId>& textureClasses);
		void RequireSizeDependentTextures(DX::DeviceResources* resources, std::vector<texture::ClassId>& textureClasses);
		inline bool AreTexturesFulfilled() { return m_sizeIndependentTexturesAreFulfilled && m_sizeDependentTexturesAreFulfilled; }
		texture::BaseTexture* GetTexture(texture::ClassId textureClass);
		void Clear();
		inline ID3D11SamplerState* const* GetLinearSamplerState() { return m_samplerStateLinear.GetAddressOf(); }
		inline ID3D11SamplerState* const* GetPointSamplerState() { return m_samplerStatePoint.GetAddressOf(); }
		inline ID3D11BlendState* GetBlendState() { return m_blendState.Get(); }
	};
}
