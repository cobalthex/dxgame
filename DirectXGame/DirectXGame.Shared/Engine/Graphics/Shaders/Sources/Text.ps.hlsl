//A very basic pixel shader that shades a position with a texture

cbuffer ColorBuffer : register(b0)
{
	float4 Color;
}

Texture2D<uint> Texture : register(t0); //only r component is used

float4 main(float4 Position : SV_POSITION, float2 TexCoord : TEXCOORD) : SV_TARGET
{
	return float4(1, 0, 0, 1);
	//return float4(Color.rgb, Color.a * Texture.Load(float3(TexCoord, 1)));
}