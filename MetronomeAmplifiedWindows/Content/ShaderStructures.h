﻿#pragma once

namespace structures
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	// Used to send position and color per-vertex data to the vertex shader.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};

	// Used to send position and texture coordinate per-vertex data to the vertex shader
	struct VertexTexCoord
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 tex;
	};
}