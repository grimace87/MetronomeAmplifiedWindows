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

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
MainSceneRenderer::MainSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_vertexCount(0),
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// Initializes view parameters when the window size changes.
void MainSceneRenderer::CreateWindowSizeDependentResources()
{
}

// Called once per frame, updates the cbuffer struct as needed.
void MainSceneRenderer::Update(DX::StepTimer const& timer)
{
}

// Renders one frame using the vertex and pixel shaders.
void MainSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
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

	// Set the texture in the pixel shader
	context->PSSetShaderResources(0, 1, m_woodenTextureView.GetAddressOf());

	// Set the sampler state
	context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

	// Draw the objects.
	context->Draw(
		m_vertexCount,
		0
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

	// Once both shaders are loaded, create the mesh.
	auto createVertexBufferTask = (createPSTask && createVSTask).then([this]() {

		// Load mesh vertices. Each vertex has a position and a texture coordinate, plus a 4-byte padding.
		VertexTexCoord sceneVertices[6];
		putSquare(sceneVertices, 0, -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		m_vertexCount = 6;

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

		// Create the sampler state
		D3D11_SAMPLER_DESC desc;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.BorderColor[0] = 0;
		desc.BorderColor[1] = 0;
		desc.BorderColor[2] = 0;
		desc.BorderColor[3] = 0;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateSamplerState(
				&desc,
				&m_samplerState)
		);
		});

	// Once the cube is loaded, the object is ready to be rendered.
	// Check if exceptions occurred first.
	(createVertexBufferTask && createTextureTask).then([this](Concurrency::task<void> t) {
		try {
			t.get();
		}
		catch (Platform::COMException^ e) {
			OutputDebugString(L"Oi mate whats this");
			throw e;
		}
		m_loadingComplete = true;
		});
}

void MainSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_samplerState.Reset();
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_vertexBuffer.Reset();
	m_woodenTexture.Reset();
	m_woodenTextureView.Reset();
}