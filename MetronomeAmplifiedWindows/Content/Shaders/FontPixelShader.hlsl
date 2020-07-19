// Globals
Texture2D shaderTexture;
SamplerState sampleType;

// A constant buffer for paint color.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	float4 paintColor;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 tex: TEXCOORD0;
};

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 sampleColor;
	sampleColor = shaderTexture.Sample(sampleType, input.tex);

	return float4(paintColor.xyz, sampleColor.w * paintColor.w);
}
