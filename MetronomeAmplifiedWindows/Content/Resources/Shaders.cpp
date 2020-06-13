#include "pch.h"
#include "Shaders.h"

#include "../../Common/DirectXHelper.h"

shader::BaseShader::BaseShader(const wchar_t* vertexShaderFile, const wchar_t* pixelShaderFile) :
	vertexShaderAssetName(vertexShaderFile),
	pixelShaderAssetName(pixelShaderFile) {}

void shader::BaseShader::CompileVertexShader(ID3D11Device3* device, const std::vector<byte>& fileData)
{
	// Construct the input description
	auto inputDescription = makeInputDescription();
	
	DX::ThrowIfFailed(
		device->CreateVertexShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&m_vertexShader
		)
	);

	DX::ThrowIfFailed(
		device->CreateInputLayout(
			inputDescription.data(),
			inputDescription.size(),
			&fileData[0],
			fileData.size(),
			&m_inputLayout
		)
	);
}

void shader::BaseShader::CompilePixelShader(ID3D11Device3* device, const std::vector<byte>& fileData)
{
	DX::ThrowIfFailed(
		device->CreatePixelShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			&m_pixelShader
		)
	);
}

Concurrency::task<void> shader::BaseShader::MakeCompileTask(ID3D11Device3* device)
{
    // Load shaders asynchronously.
    auto loadVSTask = DX::ReadDataAsync(vertexShaderAssetName);
    auto loadPSTask = DX::ReadDataAsync(pixelShaderAssetName);

    // After the vertex shader file is loaded, create the shader and input layout.
    auto createVSTask = loadVSTask.then([this, device](const std::vector<byte>& fileData) -> void {
        this->CompileVertexShader(device, fileData);
    });

    // After the pixel shader file is loaded, create the shader and constant buffer.
    auto createPSTask = loadPSTask.then([this, device](const std::vector<byte>& fileData) -> void {
        this->CompilePixelShader(device, fileData);
    });

	// Return task waiting on both shaders
	return createVSTask && createPSTask;
}

void shader::BaseShader::Activate(ID3D11DeviceContext3* context)
{
	// Set the vertex shader input layout
	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);
}

void shader::BaseShader::Reset()
{
    m_vertexShader.Reset();
    m_inputLayout.Reset();
    m_pixelShader.Reset();
}

shader::BaseShader* shader::BaseShader::NewFromClassId(ClassId id)
{
	switch (id) {
	case ClassId::ALPHA_TEXTURE:
		return new AlphaTexture();
	default:
		throw std::exception("Requested shader class does not exist");
	}
}

shader::AlphaTexture::AlphaTexture() : BaseShader(L"AlphaTextureVertexShader.cso", L"AlphaTexturePixelShader.cso") {

}

std::vector<D3D11_INPUT_ELEMENT_DESC> shader::AlphaTexture::makeInputDescription() {
	return {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
}

void shader::AlphaTexture::initialiseSubclass() {

}
