#include "Pch.hpp"

#include "FpsRenderer.hpp"
#include "Common/PlatformHelpers.hpp"
#include "App/SystemSettings.hpp"
#include "Common/StreamOps.hpp"

//Initializes D2D resources used for text rendering
FpsRenderer::FpsRenderer(Game& Game, const std::shared_ptr<DeviceResources>& DeviceResources)
	: lastFps(0), fontBuffer(), GameComponent(Game, DeviceResources), width(128), height(16), textColor(Color(1, 1, 1, 1.f))
{
	fontBuffer = StreamOps::ReadFile(SystemSettings::GetFontFile("DebugFixed/DebugFixed.ttf"), true);
	
	if (stbtt_InitFont(&fontInfo, (byte*)fontBuffer.data(), 0))
	{
		buffer = new byte[width * height];
		scale = stbtt_ScaleForPixelHeight(&fontInfo, 12);

		stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);
	}

	CreateDeviceResources();
}

void FpsRenderer::CreateDeviceResources()
{
	shader = game.LoadShader<Shaders::TextShader>(ShaderType::Text);

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

FpsRenderer::~FpsRenderer()
{
	delete[] buffer;
}

//Updates the text to be displayed
void FpsRenderer::Update(const StepTimer& Timer)
{
	//Update display text
	unsigned fps = Timer.GetFramesPerSecond();

	//recreate fps text
	if (fps != lastFps)
	{
		int x = 0;
		ascent = (int)(ascent * scale);
		descent = (int)(descent * scale);

		auto text = prefix + std::to_string(fps) + suffix;

		for (size_t i = 0; i < text.length(); i++)
		{
			/* get bounding box for character (may be offset to account for chars that dip above or below the line */
			int c_x1, c_y1, c_x2, c_y2;
			stbtt_GetCodepointBitmapBox(&fontInfo, text[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

			/* compute y (different characters have different heights */
			int y = ascent + c_y1;

			/* render character (stride and offset is important here) */
			int byteOffset = x + (y  * tex.Width());
			stbtt_MakeCodepointBitmap(&fontInfo, buffer + byteOffset, c_x2 - c_x1, c_y2 - c_y1, width, scale, scale, text[i]);

			/* how wide is this character */
			int ax;
			stbtt_GetCodepointHMetrics(&fontInfo, text[i], &ax, nullptr);
			x += (int)(ax * scale);

			if (i < text.length() - 1)
			{
				/* add kerning */
				int kern;
				kern = stbtt_GetCodepointKernAdvance(&fontInfo, text[i], text[i + 1]);
				x += (int)(kern * scale);
			}
		}

		RECT rct;
		rct.left = 0;
		rct.top = 0;
		rct.right = width;
		rct.bottom = height;
		tex.SetData(buffer, rct);
	}
	lastFps = fps;
}

//Renders a frame to the screen
void FpsRenderer::Render()
{
	shader->color.data.color = textColor;
	shader->wvp.data.wvp = worldViewProjection;
	shader->Update();
	shader->Apply();
	tex.Apply();

	unsigned stride = sizeof(Shaders::TextShader::Vertex);
	unsigned offset = 0;
	auto context = deviceResources->GetD3DDeviceContext();
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetVertexBuffers(0, 1, &vertices, &stride, &offset);
	context->Draw(4, 0);
}