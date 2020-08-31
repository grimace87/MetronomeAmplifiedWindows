// Single (model) transformation matrix, stored column-major, plus print colour
cbuffer TransformPaintColorConstantBuffer : register(b0)
{
	matrix transform;
	float4 paintColor;
};


struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 tex : TEXCOORD0;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 tex: TEXCOORD0;
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;

	float4 pos = float4(input.pos, 1.0f);

	output.pos = mul(pos, transform);
	output.tex = input.tex.xy;

	return output;
}
