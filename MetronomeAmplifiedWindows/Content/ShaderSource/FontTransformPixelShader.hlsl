// Globals
Texture2D shaderTexture;
SamplerState sampleType;

cbuffer TransformPaintColorConstantBuffer : register(b0)
{
	matrix transform;
	float4 paintColor;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 sampleColor;
	sampleColor = shaderTexture.Sample(sampleType, input.tex);

	return float4(paintColor.xyz, sampleColor.w * paintColor.w);
}
