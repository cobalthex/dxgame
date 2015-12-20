// A constant buffer that stores the three basic column-major matrices for composing geometry.

#define MAX_JOINTS 128

cbuffer WVP : register(b0)
{
	matrix World;
	matrix InverseTransposeWorld;
	matrix WorldViewProjection;
	matrix Joints[MAX_JOINTS];
};

// Per-vertex data used as input to the vertex shader.
struct SkinnedVertex
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 texCoord : TEXCOORD;
	uint4 blendIndices : BLENDINDICES;
	float4 blendWeights : BLENDWEIGHTS;
};

// Per-pixel color data passed through the pixel shader.
struct LitPixel
{
	float4 position : SV_POSITION; //position in screen coordinates
	float4 worldPosition : POSITION; //position in world coordinates
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
};

float4x3 Skin(matrix Joints[MAX_JOINTS], float4 Weights, uint4 Indices)
{
	float4x3 result = Weights.x * (float4x3)Joints[Indices.x];
	result = result + Weights.y * (float4x3)Joints[Indices.y];
	result = result + Weights.z * (float4x3)Joints[Indices.z];
	result = result + Weights.w * (float4x3)Joints[Indices.w];
	return result;
}

// Simple shader to do vertex processing on the GPU.
LitPixel main(SkinnedVertex Input)
{
	LitPixel output;
	float4 position = float4(Input.position, 1.0f);
	float4x3 skin = Skin(Joints, Input.blendWeights, Input.blendIndices);

	//position.xyz = mul(position, skin);
	//Input.normal.xyz = mul(Input.normal.xyz, (float3x3)skin);
	//Input.tangent.xyz = mul(Input.tangent.xyz, (float3x3)skin);

	//Transform the vertex position into projected space

	output.worldPosition = mul(position, World);
	output.position = mul(position, WorldViewProjection);

	//Pass the color through without modification
	output.texCoord = Input.texCoord;
	output.normal = mul(Input.normal, (float3x3)InverseTransposeWorld);

	return output;
}
