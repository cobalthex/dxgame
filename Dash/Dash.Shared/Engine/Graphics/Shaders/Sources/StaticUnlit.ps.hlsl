#include "Inputs.hlsli"

Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);

//A pass-through function for the (interpolated) color data
float4 main(LitPixel Input) : SV_TARGET
{
	return Texture.Sample(Sampler, Input.texCoord);
}