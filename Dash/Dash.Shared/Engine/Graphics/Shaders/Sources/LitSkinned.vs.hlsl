#include "Inputs.hlsli"
#include "Skinning.hlsli"

cbuffer WorldBuffer : register(b0)
{
	matrix World;
	matrix InverseTransposeWorld;
	matrix WorldViewProjection;
	matrix joints[MAX_JOINTS];
}

//Simple shader to do vertex processing on the GPU
LitPixel main(SkinnedVertex Input)
{
	LitPixel output;
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