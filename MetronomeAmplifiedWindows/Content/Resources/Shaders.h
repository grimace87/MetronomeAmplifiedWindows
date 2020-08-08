#pragma once

#include <string>

#include "../ShaderStructures.h"

namespace shader {

	enum class ClassId {
		ALPHA_TEXTURE,
		FONT
	};

	class BaseShader {
	private:
        winrt::com_ptr<ID3D11VertexShader>   m_vertexShader;
		winrt::com_ptr<ID3D11PixelShader>    m_pixelShader;
		winrt::com_ptr<ID3D11InputLayout>	 m_inputLayout;
		winrt::com_ptr<ID3D11Buffer>		 m_constantBuffer;

        void CompileVertexShader(ID3D11Device3* device, const std::vector<byte>& fileData);
        void CompilePixelShader(ID3D11Device3* device, const std::vector<byte>& fileData);

	protected:
		BaseShader(const wchar_t* vertexShaderFile, const wchar_t* pixelShaderFile);
		virtual std::vector<D3D11_INPUT_ELEMENT_DESC> makeInputDescription() = 0;
		virtual UINT GetConstantBufferSize() = 0;
		virtual bool HasConstantBuffer() = 0;
		virtual void* GetConstantBufferData() = 0;

	public:
		std::wstring vertexShaderAssetName;
		std::wstring pixelShaderAssetName;

		static BaseShader* NewFromClassId(ClassId id);
		Concurrency::task<void> MakeCompileTask(ID3D11Device3* device);
		void Activate(ID3D11DeviceContext3* context);
		void Reset();
	};

	class AlphaTexture : public BaseShader {
	public:
		AlphaTexture();
	protected:
		std::vector<D3D11_INPUT_ELEMENT_DESC> makeInputDescription() override;
		bool HasConstantBuffer() override;
		UINT GetConstantBufferSize() override;
		void* GetConstantBufferData() override;
	};

	class FontShader : public BaseShader {
	public:
		FontShader();
		void SetPaintColor(float r, float g, float b, float a);
	protected:
		std::vector<D3D11_INPUT_ELEMENT_DESC> makeInputDescription() override;
		bool HasConstantBuffer() override;
		UINT GetConstantBufferSize() override;
		void* GetConstantBufferData() override;
	private:
		structures::PaintColorConstantBuffer m_paintColorData;
	};
}
