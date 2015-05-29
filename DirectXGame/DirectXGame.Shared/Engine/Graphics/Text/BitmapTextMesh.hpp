#pragma once

#include "Engine/Graphics/Models/Mesh.hpp"
#include "Engine/Graphics/Textures/Texture2D.hpp"
#include "Engine/Graphics/Text/Formats/StbFreetype.hpp"

//A mesh of quads that make up a bitmap text block
class BitmapTextMesh
{
public:
	BitmapTextMesh() : deviceResources(nullptr), font(nullptr) { }
	BitmapTextMesh(const DeviceResourcesPtr& DeviceResources, const std::string& Text, const stbtt_fontinfo* Font, float FontSize);

	void Refresh(); //recreate the mesh

	void Draw(); //Draw the mesh

	inline Mesh& CharsMesh() { return chars; }
	inline const Mesh& CharsMesh() const { return chars; }

	inline Texture2D& Texture() { return texture; }
	inline const Texture2D& Texture() const { return texture; }

	inline std::string Text() const { return text; }

protected:
	DeviceResourcesPtr deviceResources;

	std::string text; //the rendered string
	
	const stbtt_fontinfo* font;
	float fontSize;
	Mesh chars;
	Texture2D texture; //all char images share this texture
};