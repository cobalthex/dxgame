#include "Consts.hlsli"

cbuffer WorldBuffer : register(b0)
{
	matrix World;
	matrix WorldViewProjection;
	matrix LightWvp;
	float4 LightPosition;
}

struct VertexInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
	float4 lightViewPos : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};

PixelInput main(VertexInput Input)
{
	PixelInput output;

	float4 worldPosition;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(float4(Input.position, 1), WorldViewProjection);
	// Store the texture coordinates for the pixel shader.
	output.texCoord = Input.texCoord;
	// Calculate the normal vector against the world matrix only.
	output.normal = normalize(mul(Input.normal, (float3x3)World));
	// Calculate the position of the vertex in the world.
	worldPosition = mul(Input.position, World);

	// Calculate the position of the vertice as viewed by the light source.
	output.lightViewPos = mul(Input.position, LightWvp);

	// Determine the light position based on the position of the light and the position of the vertex in the world.
	output.lightPos = normalize(LightPosition.xyz - worldPosition.xyz);

	return output;
}