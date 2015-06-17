//A very basic pixel shader that shades a position with a texture

cbuffer ColorBuffer : register(b0)
{
	float4 Color;
}

Texture2D<uint> Texture : register(t0);

//This takes in absolute texture coordinates (where Y is flipped)
float4 main(float4 Position : SV_POSITION, float2 TexCoord : TEXCOORD) : SV_TARGET
{
	float c = Texture.Load(float3(TexCoord.x, TexCoord.y, 0));
	return float4(Color.rgb, Color.a * c);
}