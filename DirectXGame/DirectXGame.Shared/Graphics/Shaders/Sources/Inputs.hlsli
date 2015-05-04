//Single vertex for static meshes
struct StaticVertex
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 texCoord : TEXCOORD;
	float4 color : COLOR;
};

//Single vertex for skinned meshes
struct SkinnedVertex
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 texCoord : TEXCOORD;
	float4 color : COLOR;
	uint4 blendIndices : BLENDINDICES;
	float4 blendWeights : BLENDWEIGHT;
};

//Pixel shader input from shaders using standard lighting
struct LitPixel
{
	float4 position : SV_POSITION; //position in screen coordinates
	float4 worldPosition : POSITION; //position in world coordinates
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
	float4 color : COLOR;
};