#pragma once

#include "Engine/Graphics/Models/Mesh.hpp"
#include "Engine/Graphics/Textures/Texture2D.hpp"
#include "Engine/Graphics/Text/Formats/StbFreetype.hpp"

//A mesh of quads that make up a bitmap text block
class BitmapTextMesh
{
public:
	BitmapTextMesh() : deviceResources(nullptr), font(nullptr) { }
	BitmapTextMesh(const DeviceResourcesPtr& DeviceResources, const std::string& Text, const stbtt_fontinfo* Font, float FontSize, bool UsePow2Textures = true);

	void Refresh(); //recreate the mesh

	void Draw(); //Draw the mesh

	inline Mesh& CharsMesh() { return mesh; }
	inline const Mesh& CharsMesh() const { return mesh; }

	inline Texture2D& Texture() { return texture; }
	inline const Texture2D& Texture() const { return texture; }

	inline std::string Text() const { return text; }

protected:
	DeviceResourcesPtr deviceResources;

	std::string text; //the rendered string
	
	const stbtt_fontinfo* font;
	float fontSize;
	Mesh mesh;
	Texture2D texture; //all char images share this texture
	bool usePow2Textures;
};