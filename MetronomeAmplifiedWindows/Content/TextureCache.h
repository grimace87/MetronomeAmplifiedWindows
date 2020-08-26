#pragma once

#include "Components/BaseTexture.h"

#include <map>

namespace cache {

	class TextureCache {
	private:
		std::map<texture::ClassId, texture::BaseTexture*> m_textures;
		bool m_sizeIndependentTexturesAreFulfilled;
		bool m_sizeDependentTexturesAreFulfilled;
		bool m_samplerAndBlendStateFulfilled;

		winrt::com_ptr<ID3D11SamplerState>        m_samplerStateLinear;
		winrt::com_ptr<ID3D11SamplerState>        m_samplerStatePoint;
		winrt::com_ptr<ID3D11BlendState>          m_blendState;

		void RequireSamplerAndBlendState(DX::DeviceResources* resources);

	public:
		TextureCache();
		bool ContainsAll(std::vector<texture::ClassId>& textureClasses);
		void RequireSizeIndependentTextures(DX::DeviceResources* resources, std::vector<texture::ClassId>& textureClasses);
		void RequireSizeDependentTextures(DX::DeviceResources* resources, std::vector<texture::ClassId>& textureClasses);
		inline bool AreTexturesFulfilled() { return m_sizeIndependentTexturesAreFulfilled && m_sizeDependentTexturesAreFulfilled; }
		texture::BaseTexture* GetTexture(texture::ClassId textureClass);
		void Clear();
		void InvalidateSizeDependentTextures();

		void ActivateBlendState(ID3D11DeviceContext* context);
		void ActivateLinearSamplerState(ID3D11DeviceContext* context);
		void ActivatePointSamplerState(ID3D11DeviceContext* context);
	};
}
