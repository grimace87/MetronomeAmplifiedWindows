// A constant buffer that stores a single transformation matrix (model matrix; view and projection are identities)
cbuffer TransformConstantBuffer : register(b0)
{
	matrix transform;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 tex : TEXCOORD0;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 tex: TEXCOORD0;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;

	// Extend position data with w component
	float4 pos = float4(input.pos, 1.0f);

	// Positions transformed, texture coordinates unmodified
	output.pos = mul(pos, transform);
	output.tex = input.tex.xy;

	return output;
}
