#pragma once

#include "../BaseShader.h"

namespace shader
{
	class AlphaTexture : public BaseShader {
	public:
		AlphaTexture();
	protected:
		std::vector<D3D11_INPUT_ELEMENT_DESC> makeInputDescription() override;
		bool VertexShaderUsesConstantBuffer() override;
		bool PixelShaderUsesConstantBuffer() override;
		UINT GetConstantBufferSize() override;
		void* GetConstantBufferData() override;
	};
}
