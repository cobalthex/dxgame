#include "Inputs.hlsli"
#include "Lighting.hlsli"

Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);

//A pass-through function for the (interpolated) color data
float4 main(LitPixel Input) : SV_TARGET
{
	LightCalc lit = ComputeLighting(Input.worldPosition, normalize(Input.normal));

	float4 emissive = Emissive;
	float4 ambient = Ambient * GlobalAmbience;
	float4 diffuse = Diffuse * lit.diffuse;
	float4 specular = Specular * lit.specular;

	float4 texColor = { 1, 1, 1, 1 };
	if (UseTexture)
		texColor = Texture.Sample(Sampler, Input.texCoord);

	return (emissive + ambient + diffuse + specular) * texColor;
}