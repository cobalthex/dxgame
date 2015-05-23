#include "Pch.hpp"

#include "FpsRenderer.hpp"
#include "Engine/Common/PlatformHelpers.hpp"
#include "App/SystemSettings.hpp"
#include "Engine/Graphics/Textures/Formats/StbImageWrite.hpp"

//Initializes D2D resources used for text rendering
FpsRenderer::FpsRenderer(const std::shared_ptr<DeviceResources>& DeviceResources, const std::shared_ptr<Shaders::TextShader>& Shader)
	: lastFps(0), textBuffer(nullptr), Drawable(DeviceResources), width(128), height(16), textColor(Color(1, 1, 1, 1.f))
{
	fontData = Sys::ReadFile(SystemSettings::GetFontFile("OCRAExt.ttf"));
	
	if (stbtt_InitFont(&fontInfo, fontData.data(), 0))
	{
		textBuffer = new byte[width * height];
		ZeroMemory(textBuffer, width * height);
		scale = stbtt_ScaleForPixelHeight(&fontInfo, 12);

		stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);
		baseline = (int)(ascent * scale);
	}

	shader = Shader;
	CreateDeviceResources();
}
FpsRenderer::~FpsRenderer()
{
	if (textBuffer != nullptr)
		delete[] textBuffer;
}

void FpsRenderer::CreateDeviceResources()
{
	tex = Texture2D(deviceResources, 128, 16, DXGI_FORMAT_R8_UINT, 1, true);

	std::vector<Shaders::TextShader::Vertex> verts;
	//create quad to display
	Shaders::TextShader::Vertex v;
	v.position = Vector3(0, 0, 0);
	v.texCoord = Vector2(0, 0);
	verts.push_back(v);
	v.position.x += width;
	v.texCoord.x = 1;
	verts.push_back(v);
	v.position = Vector3(0, (float)height, 0);
	v.texCoord = Vector2(0, 1);
	verts.push_back(v);
	v.position.x += width;
	v.texCoord.x = 1;
	verts.push_back(v);

	//create vertex buffer
	CD3D11_BUFFER_DESC vertexBufferDesc((unsigned)verts.size() * sizeof(Shaders::TextShader::Vertex), D3D11_BIND_VERTEX_BUFFER);
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = verts.data();
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	Sys::ThrowIfFailed(deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertices));
	
	auto sz = deviceResources->GetLogicalSize();
	worldViewProjection
		= Matrix::CreateTranslation(0.01f, 0.01f, 0)
		* Matrix::CreateLookAt(Vector3::Zero, Vector3::Forward, Vector3::Up)
		* Matrix::CreateOrthographicOffCenter(0, sz.Width, sz.Height, 0, -1, 1);
}

void FpsRenderer::ReleaseDeviceResources()
{
	tex.Destroy();
}

//Renders a frame to the screen
void FpsRenderer::Draw(const StepTimer& Timer)
{
	//Update display text
	unsigned fps = Timer.GetFramesPerSecond();

	//recreate fps text
	if (fps != lastFps)
	{
		int x = 0;

		auto text = prefix + std::to_string(fps) + suffix;

		for (size_t i = 0; i < text.length(); i++)
		{
			int advance, lsb, x0, y0, x1, y1;
			float x_shift = x - (float)floor(x);
			stbtt_GetCodepointHMetrics(&fontInfo, text[i], &advance, &lsb);
			stbtt_GetCodepointBitmapBoxSubpixel(&fontInfo, text[i], scale, scale, x_shift, 0, &x0, &y0, &x1, &y1);
			stbtt_MakeCodepointBitmapSubpixel(&fontInfo, textBuffer + (width * (baseline + y0) + (x  + x0)), x1 - x0, y1 - y0, 79, scale, scale, x_shift, 0, text[i]);

			// note that this stomps the old data, so where character boxes overlap (e.g. 'lj') it's wrong
			// because this API is really for baking character bitmaps into textures. if you want to render
			// a sequence of characters, you really need to render each bitmap to a temp buffer, then
			// "alpha blend" that into the working buffer
			x += (int)(advance * scale);
			if (text[i + 1])
				x += (int)(scale * stbtt_GetCodepointKernAdvance(&fontInfo, text[i], text[i + 1]));
		}

		RECT rct;
		rct.left = 0;
		rct.top = 0;
		rct.right = width;
		rct.bottom = height;
		tex.SetData(textBuffer, rct);
	}
	lastFps = fps;

	shader->color.data.color = textColor;
	shader->wvp.data.wvp = worldViewProjection;
	shader->Update();
	shader->Apply();
	shader->SetInputLayout();
	tex.Apply();

	unsigned stride = sizeof(Shaders::TextShader::Vertex);
	unsigned offset = 0;
	auto context = deviceResources->GetD3DDeviceContext();
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetVertexBuffers(0, 1, &vertices, &stride, &offset);
	context->Draw(4, 0);
}