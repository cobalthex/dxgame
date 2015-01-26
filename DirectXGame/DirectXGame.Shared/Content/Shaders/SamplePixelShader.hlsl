// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

Texture2D tex;
SamplerState ss;

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	return input.color * tex.Sample(ss, input.texcoord);
}
