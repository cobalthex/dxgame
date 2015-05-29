#include "Pch.hpp"
#include "BitmapTextMesh.hpp"
#include "Engine/Graphics/Shaders/TextShader.hpp"

BitmapTextMesh::BitmapTextMesh(const DeviceResourcesPtr& DeviceResources, const std::string& Text, const stbtt_fontinfo* Font, float FontSize)
	: deviceResources(DeviceResources), text(Text), font(Font), fontSize(FontSize)
{
	Refresh();
}

struct TexRegion
{
	int x1 = 0, y1 = 0;
	int x2 = 0, y2 = 0;
};

void BitmapTextMesh::Refresh()
{
	unsigned width = 0;
	unsigned height = 0;
	float scale = stbtt_ScaleForPixelHeight(font, fontSize);
	int ascent, descent, lineGap;
	stbtt_GetFontVMetrics(font, &ascent, &descent, &lineGap);

	unsigned texWidth = 0;
	unsigned texHeight = 0;
	int x = 0, y = 0;
	std::map<int, TexRegion> uniques;

	//calculate the size of characters
	for (unsigned i = 0; i < text.length(); i++)
	{
		//texture only stores uniques
		if (uniques.find(text[i]) != uniques.end())
			continue;

		TexRegion rgn;
		stbtt_GetCodepointBitmapBox(font, text[i], scale, scale, &rgn.x1, &rgn.y1, &rgn.x2, &rgn.y2);
		unsigned w = rgn.x2 - rgn.x1;
		unsigned h = rgn.y2 - rgn.y1;
		
		rgn.x1 += x;
		rgn.x2 += x;
		rgn.y1 += y;
		rgn.y2 += y;

		int ax;
		stbtt_GetCodepointHMetrics(font, text[i], &ax, 0);
		x += (int)(ax * scale);

		uniques[text[i]] = rgn;

		texHeight = max(texHeight, h);
		texWidth += x;
	}

	//render texture
	byte* tex = new byte[texWidth * texHeight];
	for (auto& c : uniques)
	{
		auto& v = c.second;

		//compute y (different characters have different heights
		int _y = ascent + v.y1;

		//render character (stride and offset is important here)
		int byteOffset = v.x1 + (_y  * texWidth);
		stbtt_MakeCodepointBitmap(font, tex + byteOffset, v.x2 - v.x1, v.y2 - v.y1, texWidth, scale, scale, c.first);
	}
	if (!texture.IsValid() || texture.Width() != texWidth || texture.Height() != texHeight)
		texture = Texture2D(deviceResources, texWidth, texHeight, DXGI_FORMAT_R8_UINT, 1, true);
	texture.SetData(tex, 0, 0, texWidth, texHeight);
	/*
	//create mesh
	x = 0;
	y = 0;
	std::vector<Shaders::TextShader::Vertex> verts;
	for (unsigned i = 0; i < text.length(); i++)
	{


		//how wide is this character
		int ax;
		stbtt_GetCodepointHMetrics(font, text[i], &ax, 0);
		x += (int)(ax * scale);

		if (i < text.length() - 1)
		{
			int kern;
			kern = stbtt_GetCodepointKernAdvance(font, text[i], text[i + 1]);
			x += (int)(kern * scale);
		}
	}*/
}

void BitmapTextMesh::Draw()
{
	if (texture.IsValid())
		texture.Apply();
}




/*
//get bounding box for character (may be offset to account for chars that dip above or below the line
int c_x1, c_y1, c_x2, c_y2;
stbtt_GetCodepointBitmapBox(&fontInfo, text[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

//compute y (different characters have different heights
int y = ascent + c_y1;

//render character (stride and offset is important here)
int byteOffset = x + (y  * width);
stbtt_MakeCodepointBitmap(&fontInfo, textBuffer + byteOffset, c_x2 - c_x1, c_y2 - c_y1, width, scale, scale, text[i]);

//how wide is this character
int ax;
stbtt_GetCodepointHMetrics(&fontInfo, text[i], &ax, 0);
x += (int)(ax * scale);

//add kerning
int kern;
kern = stbtt_GetCodepointKernAdvance(&fontInfo, text[i], text[i + 1]);
x += (int)(kern * scale);
*/