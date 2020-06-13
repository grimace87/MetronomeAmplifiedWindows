#pragma once

#include <string>

namespace shader {

	enum class ClassId {
		ALPHA_TEXTURE
	};

	class BaseShader {
	private:
        Microsoft::WRL::ComPtr<ID3D11VertexShader>   m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>    m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11InputLayout>	 m_inputLayout;

        void CompileVertexShader(ID3D11Device3* device, const std::vector<byte>& fileData);
        void CompilePixelShader(ID3D11Device3* device, const std::vector<byte>& fileData);

	protected:
		BaseShader(const wchar_t* vertexShaderFile, const wchar_t* pixelShaderFile);
		virtual std::vector<D3D11_INPUT_ELEMENT_DESC> makeInputDescription() = 0;
		virtual void initialiseSubclass() = 0;

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
		virtual std::vector<D3D11_INPUT_ELEMENT_DESC> makeInputDescription() override;
		virtual void initialiseSubclass() override;
	};
}
