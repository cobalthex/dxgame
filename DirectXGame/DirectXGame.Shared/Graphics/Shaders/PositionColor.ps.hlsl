//A very basic pixel shader that shades a position and color

float4 main(float4 Position : SV_POSITION, float4 Color : COLOR) : SV_TARGET
{
	return Color;
}