#include "Skinning.hlsli"

cbuffer WorldBuffer : register(b0)
{
	matrix World;
	matrix InverseTransposeWorld;
	matrix WorldViewProjection;
	matrix joints[MAX_JOINTS];
}

//Per-vertex data used as input to the vertex shader
struct VertexInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 texCoord : TEXCOORD;
	float4 color : COLOR;
	uint4 blendIndices : BLENDINDICES;
	float4 blendWeights : BLENDWEIGHT;
};

//Per-pixel color data passed through the pixel shader
struct PixelInput
{
	float4 position : SV_POSITION; //position in screen coordinates
	float4 worldPosition : POSITION; //position in world coordinates
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
	float4 color : COLOR;
};

//Simple shader to do vertex processing on the GPU
PixelInput main(VertexInput Input)
{
	PixelInput output;
	float4 position = float4(Input.position, 1.0f);
	float4x3 skin = Skin(joints, Input.blendWeights, Input.blendIndices);

	position.xyz = mul(position, skin);
	Input.normal.xyz = mul(Input.normal.xyz, (float3x3)skin);
	Input.tangent.xyz = mul(Input.tangent.xyz, (float3x3)skin);

	//Transform the vertex position into projected space

	output.worldPosition = mul(position, World);
	output.position = mul(position, WorldViewProjection);

	//Pass the color through without modification
	output.color = Input.color;
	output.texCoord = Input.texCoord;
	output.normal = mul(Input.normal, (float3x3)InverseTransposeWorld);

	return output;
}