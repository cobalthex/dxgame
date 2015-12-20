#include "Lighting.hlsli"

struct LitPixel
{
	float4 position : SV_POSITION; //position in screen coordinates
	float4 worldPosition : POSITION; //position in world coordinates
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
};

//Texture2D Texture : register(t0);
//SamplerState Sampler : register(s0);

//A pass-through function for the (interpolated) color data
float4 main(LitPixel Input) : SV_TARGET
{
	return float4(0, 0, 0, 1);

	LightCalc lit = ComputeLighting(Input.worldPosition, normalize(Input.normal));

	float4 emissive = Emissive;
	float4 ambient = Ambient * GlobalAmbience;
	float4 diffuse = Diffuse * lit.diffuse;
	float4 specular = Specular * lit.specular;

	float4 texColor = { 1, 1, 1, 1 };
	//if (UseTexture)
	//texColor = Texture.Sample(Sampler, Input.texCoord);

	return (emissive + ambient + diffuse + specular) * texColor;
}