#include "Inputs.hlsli"

cbuffer WorldBuffer : register(b0)
{
	matrix World;
	matrix InverseTransposeWorld;
	matrix WorldViewProjection;
}

LitPixel main(StaticVertex Input)
{
	LitPixel output;
	float4 position = float4(Input.position, 1.0f);

	//Transform the vertex position into projected space

	output.worldPosition = mul(position, World);
	output.position = mul(position, WorldViewProjection);

	//Pass the color through without modification
	output.color = Input.color;
	output.texCoord = Input.texCoord;
	output.normal = mul(Input.normal, (float3x3)InverseTransposeWorld);

	return output;
}