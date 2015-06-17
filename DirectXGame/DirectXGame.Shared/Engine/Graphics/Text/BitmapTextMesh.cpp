#include "Pch.hpp"
#include "BitmapTextMesh.hpp"
#include "Engine/Graphics/Shaders/TextShader.hpp"

#include "Engine/Graphics/Textures/Formats/StbImageWrite.hpp"

#include <ppltasks.h>	// For create_task
#include <cwctype>

BitmapTextMesh::BitmapTextMesh(const DeviceResourcesPtr& DeviceResources, const std::string& Text, const stbtt_fontinfo* Font, float FontSize, bool UsePow2Textures)
	: deviceResources(DeviceResources), text(Text), font(Font), fontSize(FontSize), usePow2Textures(UsePow2Textures)
{
	Refresh();
}

struct TexRegion
{
	int x1 = 0, y1 = 0;
	int x2 = 0, y2 = 0;
	bool drawn = false;
};

void BitmapTextMesh::Refresh()
{	
	unsigned width = 0;
	unsigned height = 0;
	float scale = stbtt_ScaleForPixelHeight(font, fontSize);
	int ascent, descent, lineGap;

	stbtt_GetFontVMetrics(font, &ascent, &descent, &lineGap);
	ascent = (int)(ascent * scale);
	descent = (int)(descent * scale);
	lineGap = (int)(lineGap * scale);
	 
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

	texHeight -= descent - 1; //make sure height is actually sufficient

	if (usePow2Textures)
	{
		texWidth = NextPowerOf2(texWidth);
		texHeight = NextPowerOf2(texHeight);
	}

	//render texture and create mesh (texture size must be known from above before it can be made)
	byte* tex = new byte[texWidth * texHeight];
	ZeroMemory(tex, sizeof(byte) * texWidth * texHeight);
	x = 0;
	y = 0;
	std::vector<Shaders::TextShader::Vertex> verts;
	std::vector<unsigned> idx;
	unsigned ix = 0;
	for (unsigned i = 0; i < text.length(); i++)
	{
		auto& ch = uniques[text[i]];

		int w = ch.x2 - ch.x1;
		int h = ch.y2 - ch.y1;

		//compute y (different characters have different heights
		int _y = ascent + ch.y1;
		if (!ch.drawn)
		{

			//render character (stride and offset is important here)
			int byteOffset = ch.x1 + (_y  * texWidth);
			stbtt_MakeCodepointBitmap(font, tex + byteOffset, w, h, texWidth, scale, scale, text[i]);
		}

		//create mesh
		//1 - 2
		//| / |
		//3 - 4
		//TODO: handle line breaks
		Shaders::TextShader::Vertex v;
		v.position = Vector3(x, y, 0);
		v.texCoord = Vector2(ch.x1, _y);
		verts.push_back(v);
		v.position = Vector3(x + w, y, 0);
		v.texCoord = Vector2(ch.x2, _y);
		verts.push_back(v);
		v.position = Vector3(x, y + h, 0);
		v.texCoord = Vector2(ch.x1, _y + h);
		verts.push_back(v);
		v.position = Vector3(x + w, y + h, 0);
		v.texCoord = Vector2(ch.x2, _y + h);
		verts.push_back(v);
		idx.push_back(ix);
		idx.push_back(ix + 1);
		idx.push_back(ix + 2);
		idx.push_back(ix + 1);
		idx.push_back(ix + 3);
		idx.push_back(ix + 2);
		ix += 4;

		//how wide is this character
		int ax;
		stbtt_GetCodepointHMetrics(font, text[i], &ax, nullptr);
		x += (int)(ax * scale); 

		//calculate kern
		if (i < text.length() - 1)
		{
			int kern;
			kern = stbtt_GetCodepointKernAdvance(font, text[i], text[i + 1]);
			x += (int)(kern * scale);
		}
	}
	mesh = Mesh::Create(deviceResources, verts, idx, PrimitiveTopology::TriangleList);
	/*
	//texture = Texture2D(deviceResources, "Content/textures/text.png");
	std::vector<Shaders::TextShader::Vertex> _vx;
	Shaders::TextShader::Vertex _v;
	_v.position = Vector3(0, 0, 0);
	_v.texCoord = Vector2(0, 0);
	_vx.push_back(_v);
	_v.position = Vector3(texWidth, 0, 0);
	_v.texCoord = Vector2(1, 0);
	_vx.push_back(_v);
	_v.position = Vector3(0, texHeight, 0);
	_v.texCoord = Vector2(0, 1);
	_vx.push_back(_v);
	_v.position = Vector3(texWidth, texHeight, 0);
	_v.texCoord = Vector2(1, 1);
	_vx.push_back(_v);
	unsigned __i[] = { 0, 2, 1, 1, 2, 3 };
	auto _ix = std::vector<unsigned>(__i, __i + 6);
	mesh = Mesh::Create(deviceResources, _vx, _ix, PrimitiveTopology::TriangleList);
	*/
	//create hw texture
	if (!texture.IsValid() || texture.Width() != texWidth || texture.Height() != texHeight)
		texture = Texture2D(deviceResources, texWidth, texHeight, DXGI_FORMAT_R8_UINT, 1, true);
	texture.SetData(tex, 0, 0, texWidth, texHeight);
	delete[] tex;
}

void BitmapTextMesh::Draw()
{
	if (texture.IsValid())
		texture.Apply();

	if (mesh.IsValid())
		mesh.Draw();
}