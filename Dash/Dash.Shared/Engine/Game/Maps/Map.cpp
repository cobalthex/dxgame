#include "Pch.hpp"
#include "Map.hpp"
#include "Engine/Graphics/Shaders/ShaderCache.hpp"

//A single item to draw 
struct DrawItem
{
	Model* model;
	ModelMesh* mesh;
	ShaderType shader;

	//Sort by shader type
	inline bool operator < (const DrawItem& Other) const { return shader < Other.shader; }

	DrawItem(const std::shared_ptr<Model>& Model, ModelMesh& Mesh)
		: model(Model.get()), mesh(&Mesh), shader(Mesh.material.GetShaderType(Model->VertexFormat())) { }
};

void Map::Draw(const ComPtr<ID3D11DeviceContext>& DevContext, const Camera& Camera, ShaderCache& ShCache)
{
	std::priority_queue<DrawItem> drawlist; //keep a sorted list of models

	//add all models to be rendered
	if (world != nullptr)
	{
		for (auto& mesh : world->meshes)
			drawlist.push(DrawItem(world, mesh.second));
	}
	for (auto& model : models)
	{
		for (auto& mesh : model->meshes)
			drawlist.push(DrawItem(world, mesh.second));
	}

	Model* lastModel = nullptr;

	//draw all meshes
	//Model meshes should typically share shader type for efficiency
	while (!drawlist.empty())
	{
		DrawItem item = drawlist.top();
		drawlist.pop();

		auto& shader = ShCache.ApplyShader(item.shader);
		shader->ApplyMaterial(item.mesh->material);


		if (item.model != lastModel)
		{
			lastModel = item.model;
			lastModel->BeginDraw();
		}

		item.mesh->Draw(DevContext);
	}
}
