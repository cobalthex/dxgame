#include "Pch.hpp"
#include "Model.hpp"
#include "Common/PlatformHelpers.hpp"

Model::Model
(
	const DeviceResourcesPtr& DeviceResources,
	const std::vector<VertexType>& Vertices,
	const std::vector<IndexType>& Indices,
	PrimitiveTopology Topology,
	const std::vector<MeshType>& Meshes,
	const std::vector<Joint>& Joints,
	const std::map<std::string, SkinnedSequence> Poses
	) :
	deviceResources(DeviceResources),
	devContext(DeviceResources->GetD3DDeviceContext()),
	meshes(Meshes),
	joints(Joints),
	poses(Poses)
{
	CreateFrom(DeviceResources, Vertices, Indices, Topology);
}

void Model::Draw(unsigned Slot) const
{
	Bind(Slot);

	//Draw all of the meshes
	for (auto& m : meshes)
	{
		if (m.material.shader != nullptr)
		{
			m.material.shader->material.data = m.material;
			m.material.shader->Update();
			m.material.shader->Apply();
		}
		if (m.material.diffuseMap != nullptr)
			m.material.diffuseMap->Apply();

		//Draw the objects.
		devContext->DrawIndexed((unsigned)m.IndexCount(), (unsigned)m.StartIndex(), 0);
	}
}

void Model::Skin(const std::string& Pose, Matrix* PoseArray, size_t MaxPoses) const
{
	auto nj = min(MaxPoses, joints.size());
	std::vector<Matrix> poseMats(nj);

	bool hasPose = (poses.find(Pose) != poses.end());

	for (size_t i = 0; i < nj; i++)
	{
		auto parent = joints[i].parent;
		
		Matrix local;
		if (hasPose)
		{
			auto p = &poses.at(Pose).pose; //the current pose, transforms set by animation

			local *= Matrix::CreateScale(p->scales[i]);
			local *= Matrix::CreateFromQuaternion(p->rotations[i]);
			local *= Matrix::CreateTranslation(p->translations[i]);
		}
		else
			local = joints[i].transform;

		if (parent >= 0)
			poseMats[i] = local * poseMats[parent];
		else
			poseMats[i] = local;
	
		PoseArray[i] = (joints[i].inverseTransform * poseMats[i]).Transpose();
	}
}

BasicMesh<Shaders::PositionColorShader::Vertex, unsigned> Model::CreateSkeletalMesh(const std::string& Pose, const Color& VertexColor) const
{
	std::vector<Shaders::PositionColorShader::Vertex> vertices;
	std::vector<unsigned> indices;
	vertices.reserve(joints.size() * 3); //octahedrons (6 vertices)
	indices.reserve(joints.size() * 3); //8 faces

	Shaders::PositionColorShader::Vertex v;
	v.color = VertexColor;

	float radius = 0.05f;

	bool hasPose = (poses.find(Pose) != poses.end());

	auto transforms = new Matrix[joints.size()];

	unsigned tri = 0;
	for (unsigned i = 0; i < joints.size(); i++)
	{
		auto& j = joints[i];

		if (hasPose)
		{
			auto p = &poses.at(Pose).pose; //the current pose, transforms set by animation

			transforms[i]  = Matrix::CreateScale(p->scales[j.index]);
			transforms[i] *= Matrix::CreateFromQuaternion(p->rotations[j.index]);
			transforms[i] *= Matrix::CreateTranslation(p->translations[j.index]);

			if (j.parent >= 0)
				transforms[i] *= transforms[j.parent];
		}
		else
			transforms[i] = j.transform; //joints are already transformed by parent


		//parent joints
		v.position = Vector3::Transform(Vector3(-radius, 0, 0), transforms[j.parent]);
		vertices.push_back(v);

		v.position = Vector3::Transform(Vector3(radius, 0, 0), transforms[j.parent]);
		vertices.push_back(v);

		//current joint
		v.position = Vector3::Transform(Vector3::Zero, transforms[i]);
		vertices.push_back(v);

		unsigned tris[] = { tri, tri + 1, tri + 2, tri + 2, tri, tri + 1 };
		indices.insert(indices.end(), tris, tris + ARRAYSIZE(tris));
		tri += 3;
	}

	delete[] transforms;

	return BasicMesh<Shaders::PositionColorShader::Vertex, unsigned>(vertices, indices, PrimitiveTopology::TriangleList);
}