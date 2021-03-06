#include "pch.h"
#include "BaseShader.h"

#include "Shaders/AlphaTextureShader.h"
#include "Shaders/AlphaTextureTransformShader.h"
#include "Shaders/FontShader.h"
#include "Shaders/FontTransformShader.h"
#include "../../Common/DirectXHelper.h"

shader::BaseShader::BaseShader(const wchar_t* vertexShaderFile, const wchar_t* pixelShaderFile) :
	vertexShaderAssetName(vertexShaderFile),
	pixelShaderAssetName(pixelShaderFile) {}

void shader::BaseShader::CompileVertexShader(ID3D11Device3* device, const std::vector<byte>& fileData)
{
	// Construct the input description
	auto inputDescription = makeInputDescription();
	
	winrt::check_hresult(
		device->CreateVertexShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			m_vertexShader.put()
		)
	);

	winrt::check_hresult(
		device->CreateInputLayout(
			inputDescription.data(),
			inputDescription.size(),
			&fileData[0],
			fileData.size(),
			m_inputLayout.put()
		)
	);
}

void shader::BaseShader::CompilePixelShader(ID3D11Device3* device, const std::vector<byte>& fileData)
{
	winrt::check_hresult(
		device->CreatePixelShader(
			&fileData[0],
			fileData.size(),
			nullptr,
			m_pixelShader.put()
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

    // After the pixel shader file is loaded, create the shader and initialise the subclass (e.g. create constant buffer).
    auto createPSTask = loadPSTask.then([this, device](const std::vector<byte>& fileData) -> void {
        this->CompilePixelShader(device, fileData);

		if (HasConstantBuffer()) {
			CD3D11_BUFFER_DESC constantBufferDesc(GetConstantBufferSize(), D3D11_BIND_CONSTANT_BUFFER);
			winrt::check_hresult(
				device->CreateBuffer(
					&constantBufferDesc,
					nullptr,
					m_constantBuffer.put()
				)
			);
		}
    });

	// Return task waiting on both shaders
	return createVSTask && createPSTask;
}

void shader::BaseShader::Activate(ID3D11DeviceContext3* context)
{
	// Set the vertex shader input layout
	context->IASetInputLayout(m_inputLayout.get());

	// Attach our vertex shader.
	context->VSSetShader(
		m_vertexShader.get(),
		nullptr,
		0
	);

	// Attach our pixel shader.
	context->PSSetShader(
		m_pixelShader.get(),
		nullptr,
		0
	);

	// Send the constant buffer to the graphics device.
	if (HasConstantBuffer()) {
		ID3D11Buffer* constantBuffer = m_constantBuffer.get();

		context->UpdateSubresource1(
			constantBuffer,
			0,
			NULL,
			GetConstantBufferData(),
			0,
			0,
			0
		);

		if (VertexShaderUsesConstantBuffer()) {
			context->VSSetConstantBuffers1(
				0,
				1,
				&constantBuffer,
				nullptr,
				nullptr
			);
		}

		if (PixelShaderUsesConstantBuffer()) {
			context->PSSetConstantBuffers1(
				0,
				1,
				&constantBuffer,
				nullptr,
				nullptr
			);
		}
	}
}

void shader::BaseShader::Reset()
{
    m_vertexShader = nullptr;
    m_inputLayout = nullptr;
    m_pixelShader = nullptr;
	if (HasConstantBuffer()) {
		m_constantBuffer = nullptr;
	}
}

shader::BaseShader* shader::BaseShader::NewFromClassId(ClassId id)
{
	switch (id) {
	case ClassId::ALPHA_TEXTURE:
		return new AlphaTexture();
    case ClassId::ALPHA_TRANSFORM_TEXTURE:
        return new AlphaTextureTransformShader();
	case ClassId::FONT:
		return new FontShader();
    case ClassId::FONT_TRANSFORM:
        return new FontTransformShader();
	default:
		throw std::exception("Requested shader class does not exist");
	}
}
