#pragma once

namespace structures
{
	// Constant buffer used to send a single (model) matrix to the vertex shader.
	struct TransformConstantBuffer
	{
		DirectX::XMMATRIX transform;
	};

	// Constant buffer for RGBA paint colour
	struct PaintColorConstantBuffer
	{
		DirectX::XMFLOAT4 color;
	};

	// Constant buffer for RGBA paint colour and transformation matrix
	struct TransformPaintColorConstantBuffer
	{
        DirectX::XMMATRIX transform;
		DirectX::XMFLOAT4 color;
	};

	// Used to send position and texture coordinate per-vertex data to the vertex shader
	struct VertexTexCoord
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 tex;
	};
}