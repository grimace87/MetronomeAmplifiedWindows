#pragma once

#include "../BaseShader.h"
#include "../../ShaderStructures.h"

namespace shader
{
    class AlphaTextureTransformShader : public BaseShader {
    public:
        AlphaTextureTransformShader();
        void SetTransform(DirectX::XMMATRIX& transformMatrixRowMajor);
    protected:
        std::vector<D3D11_INPUT_ELEMENT_DESC> makeInputDescription() override;
        bool VertexShaderUsesConstantBuffer() override;
        bool PixelShaderUsesConstantBuffer() override;
        UINT GetConstantBufferSize() override;
        void* GetConstantBufferData() override;
    private:
        structures::TransformConstantBuffer m_constantBufferData;
    };
}
