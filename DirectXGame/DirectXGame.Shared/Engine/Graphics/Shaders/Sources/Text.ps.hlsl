//A very basic pixel shader that shades a position with a texture

cbuffer ColorBuffer : register(b0)
{
	float4 Color;
}

Texture2D<uint> Texture : register(t0); //only r component is used

float4 main(float4 Position : SV_POSITION, float2 TexCoord : TEXCOORD) : SV_TARGET
{
	uint w, h;
	Texture.GetDimensions(w, h);
	float c = Texture.Load(float3(TexCoord.x * w, (1 - TexCoord.y) * h, 0));
	return float4(Color.rgb, Color.a * c);
}