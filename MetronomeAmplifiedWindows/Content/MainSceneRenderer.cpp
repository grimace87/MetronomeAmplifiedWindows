#include "pch.h"
#include "MainSceneRenderer.h"

#include "..\Common\DirectXHelper.h"
#include "..\Common\WICTextureLoader.h"

using namespace MetronomeAmplifiedWindows;

using namespace DirectX;
using namespace Windows::Foundation;

// Puts the vertex data for a square into a float array, assuming each vertex fills 6 floats [x, y, z, s, t, unused]
void MainSceneRenderer::putSquare(VertexTexCoord buffer[], int index, float x1, float y1, float x2, float y2, float s1, float t1, float s2, float t2)
{
	VertexTexCoord squareVertices[] =
	{
		{XMFLOAT3(x1, y1, 0.0f), XMFLOAT3(s1, t1, 0.0f)},
		{XMFLOAT3(x2, y1, 0.0f), XMFLOAT3(s2, t1, 0.0f)},
		{XMFLOAT3(x2, y2, 0.0f), XMFLOAT3(s2, t2, 0.0f)},
		{XMFLOAT3(x2, y2, 0.0f), XMFLOAT3(s2, t2, 0.0f)},
		{XMFLOAT3(x1, y2, 0.0f), XMFLOAT3(s1, t2, 0.0f)},
		{XMFLOAT3(x1, y1, 0.0f), XMFLOAT3(s1, t1, 0.0f)}
	};
	std::copy(squareVertices, squareVertices + 6, &buffer[index]);
}

std::vector<byte> MainSceneRenderer::makeOverlayTextureData(_Out_ int* width, _Out_ int* height)
{
	// Determine sizes and create the data array
	const float screenDpi = m_deviceResources->GetDpi();
	const float cornerRadiusLogicalInches = 0.15f;
	const byte alphaLevel = 0x80;
	const int cornerRadiusPixels = (int)(cornerRadiusLogicalInches * screenDpi);
	const int rowStrideBytes = 4 * 2 * cornerRadiusPixels;
	const int sectionOffsetBytes = 4 * cornerRadiusPixels;
	std::vector<byte> textureData;
	textureData.resize(rowStrideBytes * cornerRadiusPixels);
	std::fill(textureData.begin(), textureData.end(), 0xff);

	// Output dimensions
	*width = 2 * cornerRadiusPixels;
	*height = cornerRadiusPixels;

	// Generate the left section (rounded corner)
	for (int j = 0; j < cornerRadiusPixels; j++) {
		int index = j * rowStrideBytes;
		const int transparentPixels = (int)(cornerRadiusPixels - sqrt(max(0.0, 2.0 * j * cornerRadiusPixels - j * j)));
		for (int i = 0; i < cornerRadiusPixels; i++) {
			const byte pixelAlpha = i <= transparentPixels ? 0 : alphaLevel;
			textureData[index + 3] = pixelAlpha;
			index += 4;
		}
	}

	// Generate the right section (solid colour)
	for (int j = 0; j < cornerRadiusPixels; j++) {
		int index = j * rowStrideBytes + sectionOffsetBytes;
		for (int i = 0; i < cornerRadiusPixels; i++) {
			textureData[index + 3] = alphaLevel;
			index += 4;
		}
	}

	// Return the buffer
	return textureData;
}

// Function that creates a texture from raw pixel data (assumes RGBA format)
void MainSceneRenderer::CreateTextureFromRawPixelData(std::vector<byte>& pixelData, int width, int height, ID3D11Resource** texture, ID3D11ShaderResourceView** textureView)
{
	// Describe texture
	D3D11_SUBRESOURCE_DATA subData = { (const void*)pixelData.data(), width * 4 * sizeof(byte), 0 };
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = 0;

	// Create resource
	ID3D11Texture2D* tex;
	DX::ThrowIfFailed(
	m_deviceResources->GetD3DDevice()->CreateTexture2D(
		&desc,
		&subData,
		&tex)
	);

	// Describe texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipLevels = 1;
	viewDesc.Texture2D.MostDetailedMip = 0;

	// Create texture view
	ID3D11ShaderResourceView* texView;
	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateShaderResourceView(
			tex,
			&viewDesc,
			&texView)
	);

	// Assign output parameters
	*texture = tex;
	*textureView = texView;
}

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
MainSceneRenderer::MainSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceDependentLoadingComplete(false),
	m_windowDependentLoadingComplete(false),
	m_backgroundVertexCount(0),
	m_overlayVertexCount(0),
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// Called once per frame, updates the cbuffer struct as needed.
void MainSceneRenderer::Update(DX::StepTimer const& timer)
{
}

// Renders one frame using the vertex and pixel shaders.
void MainSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_deviceDependentLoadingComplete || !m_windowDependentLoadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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

	// Set the blend state
	UINT sampleMask = 0xffffffff;
	context->OMSetBlendState(m_blendState.Get(), NULL, sampleMask);

	// Set the texture in the pixel shader
	context->PSSetShaderResources(0, 1, m_woodenTextureView.GetAddressOf());

	// Set the sampler state
	context->PSSetSamplers(0, 1, m_samplerStateLinear.GetAddressOf());

	// Draw the objects.
	context->Draw(
		m_backgroundVertexCount,
		0
	);

	// Set the texture in the pixel shader
	context->PSSetShaderResources(0, 1, m_overlayTextureView.GetAddressOf());

	// Set the sampler state
	context->PSSetSamplers(0, 1, m_samplerStatePoint.GetAddressOf());

	// Draw the objects.
	context->Draw(
		m_overlayVertexCount,
		m_backgroundVertexCount
	);
}

void MainSceneRenderer::CreateDeviceDependentResources()
{
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"AlphaTextureVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"AlphaTexturePixelShader.cso");

	// Load an image file asynchronously
	auto loadTextureImageTask = DX::ReadDataAsync(L"Assets\\Textures\\wood_bg_texture.jpg");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
			)
		);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
			)
		);
		});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
			)
		);
		});

	// After the image file is loaded, decode it and create a texture
	auto createTextureTask = loadTextureImageTask.then([this](const std::vector<byte>& fileData) {

		// Decode the image data and create the Direct3D texture and texture view
		DX::ThrowIfFailed(
			CreateWICTextureFromMemory(
				m_deviceResources->GetD3DDevice(),
				m_deviceResources->GetD3DDeviceContext(),
				fileData.data(),
				fileData.size(),
				&m_woodenTexture,
				&m_woodenTextureView)
		);

		// Create the linear sampler state
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateSamplerState(
				&samplerDesc,
				&m_samplerStateLinear)
		);

		// Create the point sampler state, as a slight variation of the previous
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateSamplerState(
				&samplerDesc,
				&m_samplerStatePoint)
		);

		// Create the blend state
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBlendState(
				&blendDesc,
				&m_blendState)
		);
		});

	// Check if exceptions occurred, if not then signal this stuff loaded okay.
	(createVSTask && createPSTask && createTextureTask).then([this](Concurrency::task<void> t) {
		try {
			t.get();
		}
		catch (Platform::COMException^ e) {
			OutputDebugString(L"Oi mate whats this");
			throw e;
		}

		m_deviceDependentLoadingComplete = true;
		});
}

// Initializes view parameters when the window size changes.
void MainSceneRenderer::CreateWindowSizeDependentResources()
{
	using namespace Concurrency;

	// Coordinates used in the vertex buffer depend on the window size
	auto createVertexBufferTask = create_task([this] {

		// Get necessary coordinates to draw the overlay, in normalised coordinates (range of -1 to 1)
		Size size = m_deviceResources->GetOutputSize();
		const float marginLogicalInches = 0.15f;
		const float dpi = m_deviceResources->GetDpi();
		const float marginUnitsW = 2.0f * (marginLogicalInches * dpi) / size.Width;
		const float marginUnitsH = 2.0f * (marginLogicalInches * dpi) / size.Height;

		const float w1 = -1.0f + marginUnitsW;
		const float w2 = w1 + marginUnitsW;
		const float w4 = 1.0f - marginUnitsW;
		const float w3 = w4 - marginUnitsW;

		const float h1 = -1.0f + marginUnitsH;
		const float h2 = h1 + marginUnitsH;
		const float h4 = 1.0f - marginUnitsH;
		const float h3 = h4 - marginUnitsH;

		// Load mesh vertices. Each vertex has a position and a texture coordinate, plus a 4-byte padding.
		VertexTexCoord sceneVertices[48];
		putSquare(sceneVertices, 0, -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
		putSquare(sceneVertices, 6, w1, h2, w2, h1, 0.0f, 1.0f, 0.5f, 0.0f);
		putSquare(sceneVertices, 12, w2, h2, w3, h1, 0.5f, 1.0f, 1.0f, 0.0f);
		putSquare(sceneVertices, 18, w3, h2, w4, h1, 0.5f, 1.0f, 0.0f, 0.0f);
		putSquare(sceneVertices, 24, w1, h3, w4, h2, 0.5f, 0.0f, 1.0f, 1.0f);
		putSquare(sceneVertices, 30, w1, h4, w2, h3, 0.0f, 0.0f, 0.5f, 1.0f);
		putSquare(sceneVertices, 36, w2, h4, w3, h3, 0.5f, 0.0f, 1.0f, 1.0f);
		putSquare(sceneVertices, 42, w3, h4, w4, h3, 0.5f, 0.0f, 0.0f, 1.0f);

		m_backgroundVertexCount = 6;
		m_overlayVertexCount = 42;

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = sceneVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(sceneVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
			)
		);
		});

	// Create the dpi-dependent texture
	auto createTextureTask = create_task([this] {

		int width;
		int height;
		std::vector<byte> overlayData = makeOverlayTextureData(&width, &height);
		CreateTextureFromRawPixelData(overlayData, width, height, &m_overlayTexture, &m_overlayTextureView);
		});


	// Check if exceptions occurred, if not then signal this stuff loaded okay.
	(createVertexBufferTask && createTextureTask).then([this](Concurrency::task<void> t) {
		try {
			t.get();
		}
		catch (Platform::COMException^ e) {
			OutputDebugString(L"Oi mate whats this");
			throw e;
		}

		m_windowDependentLoadingComplete = true;
		});
}

void MainSceneRenderer::ReleaseDeviceDependentResources()
{
	m_deviceDependentLoadingComplete = false;
	m_windowDependentLoadingComplete = false;
	m_samplerStateLinear.Reset();
	m_samplerStatePoint.Reset();
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_vertexBuffer.Reset();
	m_woodenTexture.Reset();
	m_woodenTextureView.Reset();
	m_overlayTexture.Reset();
	m_overlayTextureView.Reset();
}