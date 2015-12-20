#include "Pch.hpp"
#include "Model.hpp"
#include "iqm.hpp"
#include "Graphics/DirectXHelper.hpp"

//Todo: maybe move to memory mapped files

//Temporary storage for loading IQM data
struct IqmTemp
{
	Iqm::Header header;

	uint8* buffer = nullptr; //The file read into memory. Most of the pointers in this temp are directly mapped here

	Iqm::Mesh* meshes = nullptr;
	Iqm::Vertex* vertices = nullptr;
	Iqm::Normal* normals = nullptr;
	Iqm::Tangent* tangents = nullptr;
	Iqm::TexCoord* texCoords = nullptr;
	Iqm::Color* colors = nullptr;
	Iqm::BlendIndex* blendIndices = nullptr;
	Iqm::BlendWeight* blendWeights = nullptr;

	Iqm::Triangle* tris = nullptr;
	Iqm::Adjacency* adjacencies = nullptr;
	Iqm::Joint* joints = nullptr;
	Iqm::Pose* poses = nullptr;
	Iqm::Anim* anims = nullptr;
	Iqm::Bounds* bounds = nullptr;

	char* texts = nullptr; //all of the texts
	char* comments = nullptr; //all of the comments

	Iqm::Extension* extensions = nullptr;
};

bool LoadMeshes(IqmTemp& Temp); //Load the meshes (and materials) from the buffer
bool LoadAnimations(IqmTemp& Temp); //Load the animations from the buffer

void CleanupTemp(IqmTemp& Temp)
{
	delete[] Temp.buffer;
}

Model::Model(const DeviceResourcesPtr& DeviceResources, std::istream& Input, ComPtr<ID3D12GraphicsCommandList>& CommandList)
	: deviceResources(DeviceResources)
{
	//Store temp data
	IqmTemp tmp;

	//load header
	Input.read((char*)&tmp.header, sizeof(tmp.header) * sizeof(uint8));
	assert(!memcmp(tmp.header.magic, IQMAGIC, sizeof(tmp.header.magic)));

	assert(tmp.header.version == IQVERSION);

	//Files > 16mb not supported
	assert(tmp.header.fileSize <= (16 << 20));

	tmp.buffer = new uint8[tmp.header.fileSize];

	//read the file into memory
	auto headerSz = sizeof(Iqm::Header);
	Input.read((char*)tmp.buffer + headerSz, tmp.header.fileSize - headerSz);

	assert(tmp.header.numMeshes < 1 || LoadMeshes(tmp));

	//Create model from generated data

	indexCount = 0;
	vertexCount = 0;

	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;
	for (unsigned i = 0; i < tmp.header.numMeshes; i++)
	{
		auto& m = tmp.meshes[i];

		indexCount += m.numTriangles * 3;
		vertexCount += m.numVertices;

		indices.reserve(indices.size() + m.numTriangles * 3);
		vertices.reserve(vertices.size() + m.numVertices);

		//indices
		for (unsigned j = 0; j < m.numTriangles; j++)
		{
			//For independent vertex buffers, subtract m.firstVertex from each of the vertices of the triangle
			indices.push_back(tmp.tris[m.firstTriangle + j].vertex[0]);
			indices.push_back(tmp.tris[m.firstTriangle + j].vertex[1]);
			indices.push_back(tmp.tris[m.firstTriangle + j].vertex[2]);
		}

		//vertices
		for (unsigned j = 0; j < m.numVertices; j++)
		{
			Vertex v;

			auto& _v = tmp.vertices[m.firstVertex + j];
			auto& _n = tmp.normals[m.firstVertex + j];
			auto& _g = tmp.tangents[m.firstVertex + j];
			auto& _t = tmp.texCoords[m.firstVertex + j];
			auto& _c = tmp.colors[m.firstVertex + j];
			auto& _i = tmp.blendIndices[m.firstVertex + j];
			auto& _w = tmp.blendWeights[m.firstVertex + j];

			//correct orientation would be yzx for DX

			v.position = XMFLOAT3(_v.x, _v.y, _v.z);
			if (tmp.normals != nullptr)
				v.normal = XMFLOAT3(_n.x, _n.y, _n.z);
			if (tmp.tangents != nullptr)
				v.tangent = XMFLOAT4(_g.x, _g.y, _g.z, _g.w);
			if (tmp.texCoords != nullptr)
				v.texcoord = XMFLOAT2(_t.u, _t.v);
			if (tmp.blendIndices != nullptr)
				v.blendIndices = PackedVector::XMUBYTE4(_i.a, _i.b, _i.c, _i.d);
			if (tmp.blendWeights != nullptr)
				v.blendWeights = PackedVector::XMUBYTEN4(_w.a, _w.b, _w.c, _w.d);
			//if (tmp.colors != nullptr)
			//	v.color = DirectX::PackedVector::XMUBYTEN4(_c.r, _c.g, _c.b, 1);

			vertices.push_back(v);
		}

		//material

		Material mtl;
		ModelMesh mesh;
		mesh.name = m.name;
		mesh.startVertex = m.firstVertex;
		mesh.startIndex = m.firstTriangle * 3;
		mesh.vertexCount = m.numVertices;
		mesh.indexCount = m.numTriangles * 3;
		mesh.material = mtl;
		meshes[mesh.name] = mesh;
	}

	//create model
	GpuCreate(vertices, indices, CommandList);

	CleanupTemp(tmp);
}

bool LoadMeshes(IqmTemp& Temp)
{
	unsigned char* vindex = NULL, *vweight = NULL;
	Iqm::VertexArray* vas = (Iqm::VertexArray*)&Temp.buffer[Temp.header.offsetVertexArrays];
	for (unsigned i = 0; i < Temp.header.numVertexArrays; i++)
	{
		Iqm::VertexArray &va = vas[i];
		switch (va.type)
		{
		case Iqm::IQMPOSITION:
			//Make sure format is valid
			if (va.format != Iqm::IQMFLOAT || va.size != 3)
				return false;

			Temp.vertices = (Iqm::Vertex*)&Temp.buffer[va.offset];
			break;

		case Iqm::IQMNORMAL:
			if (va.format != Iqm::IQMFLOAT || va.size != 3)
				return false;

			Temp.normals = (Iqm::Normal*)&Temp.buffer[va.offset];
			break;

		case Iqm::IQMTANGENT:
			if (va.format != Iqm::IQMFLOAT || va.size != 4)
				return false;

			Temp.tangents = (Iqm::Tangent*)&Temp.buffer[va.offset];
			break;

		case Iqm::IQMTEXCOORD:
			if (va.format != Iqm::IQMFLOAT || va.size != 2)
				return false;

			Temp.texCoords = (Iqm::TexCoord*)&Temp.buffer[va.offset];
			break;

		case Iqm::IQMCOLOR:
			if (va.format != Iqm::IQMUBYTE || va.size != 4)
				return false;

			Temp.colors = (Iqm::Color*)&Temp.buffer[va.offset];
			break;

		case Iqm::IQMBLENDINDEXES:
			if (va.format != Iqm::IQMUBYTE || va.size != 4)
				return false;

			Temp.blendIndices = (Iqm::BlendIndex*)&Temp.buffer[va.offset];
			break;

		case Iqm::IQMBLENDWEIGHTS:
			if (va.format != Iqm::IQMUBYTE || va.size != 4)
				return false;

			Temp.blendWeights = (Iqm::BlendWeight*)&Temp.buffer[va.offset];
			break;
		}
	}

	if (Temp.vertices == nullptr)
		return false;

	Temp.meshes = (Iqm::Mesh*)&Temp.buffer[Temp.header.offsetMeshes];
	Temp.tris = (Iqm::Triangle*)&Temp.buffer[Temp.header.offsetTriangles];

	if (Temp.header.offsetAdjacency > 0)
		Temp.adjacencies = (Iqm::Adjacency*)&Temp.buffer[Temp.header.offsetAdjacency];

	Temp.texts = Temp.header.offsetTexts > 0 ? (char*)&Temp.buffer[Temp.header.offsetTexts] : "";
	Temp.comments = Temp.header.offsetComments > 0 ? (char*)&Temp.buffer[Temp.header.offsetComments] : "";

	return true;
}

void Model::GpuCreate(const std::vector<Vertex>& Vertices, const std::vector<unsigned>& Indices, const ComPtr<ID3D12GraphicsCommandList>& CommandList)
{
	auto d3dDevice = deviceResources->GetD3DDevice();

	CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(Vertices.size() * sizeof(Vertex));
	DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&defaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexBufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&vertexBuffer)));

	CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
	DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexBufferUpload)));

	vertexBuffer->SetName(L"Vertex Buffer Resource");
	vertexBufferUpload->SetName(L"Vertex Buffer Upload Resource");

	// Upload the vertex buffer to the GPU.
	{
		D3D12_SUBRESOURCE_DATA vertexData = {};
		vertexData.pData = reinterpret_cast<const byte*>(Vertices.data());
		vertexData.RowPitch = Vertices.size() * sizeof(Vertex);
		vertexData.SlicePitch = vertexData.RowPitch;

		UpdateSubresources(CommandList.Get(), vertexBuffer.Get(), vertexBufferUpload.Get(), 0, 0, 1, &vertexData);

		CD3DX12_RESOURCE_BARRIER vertexBufferResourceBarrier =
			CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		CommandList->ResourceBarrier(1, &vertexBufferResourceBarrier);
	}

	size_t indexBufferSize = Indices.size() * sizeof(unsigned);
	CD3DX12_RESOURCE_DESC indexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
	DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&defaultHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&indexBufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&indexBuffer)));

	DX::ThrowIfFailed(d3dDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&indexBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBufferUpload)));

	indexBuffer->SetName(L"Index Buffer Resource");
	indexBufferUpload->SetName(L"Index Buffer Upload Resource");

	// Upload the index buffer to the GPU.
	{
		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = reinterpret_cast<const byte*>(Indices.data());
		indexData.RowPitch = indexBufferSize;
		indexData.SlicePitch = indexData.RowPitch;

		UpdateSubresources(CommandList.Get(), indexBuffer.Get(), indexBufferUpload.Get(), 0, 0, 1, &indexData);

		CD3DX12_RESOURCE_BARRIER indexBufferResourceBarrier =
			CD3DX12_RESOURCE_BARRIER::Transition(indexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		CommandList->ResourceBarrier(1, &indexBufferResourceBarrier);
	}
}

bool LoadAnimations(IqmTemp& Temp)
{
	/*
	Temp.anims = (Iqm::Anim*)&Temp.buffer[Temp.header.offsetAnims];
	Temp.poses = (Iqm::Pose*)&Temp.buffer[Temp.header.offsetPoses];
	//Temp.frames = new Matrix[Temp.header.numFrames * Temp.header.numPoses];
	unsigned short* frameData = (unsigned short*)&Temp.buffer[Temp.header.offsetFrames];

	Temp.genPoses = new ::Pose[Temp.header.numFrames];

	for (unsigned i = 0; i < Temp.header.numFrames; i++)
	{
		auto& pose = Temp.genPoses[i];
		pose = Pose(Temp.header.numPoses);

		for (unsigned j = 0; j < Temp.header.numPoses; j++)
		{
			auto p = Temp.poses[j];

			pose.translations[j].x = p.channelOffset[0]; if (p.mask & 0x01) pose.translations[j].x += *frameData++ * p.channelScale[0];
			pose.translations[j].y = p.channelOffset[1]; if (p.mask & 0x02) pose.translations[j].y += *frameData++ * p.channelScale[1];
			pose.translations[j].z = p.channelOffset[2]; if (p.mask & 0x04) pose.translations[j].z += *frameData++ * p.channelScale[2];

			pose.rotations[j].x = p.channelOffset[3]; if (p.mask & 0x08) pose.rotations[j].x += *frameData++ * p.channelScale[3];
			pose.rotations[j].y = p.channelOffset[4]; if (p.mask & 0x10) pose.rotations[j].y += *frameData++ * p.channelScale[4];
			pose.rotations[j].z = p.channelOffset[5]; if (p.mask & 0x20) pose.rotations[j].z += *frameData++ * p.channelScale[5];
			pose.rotations[j].w = p.channelOffset[6]; if (p.mask & 0x40) pose.rotations[j].w += *frameData++ * p.channelScale[6];
			pose.rotations[j].Normalize();

			pose.scales[j].x = p.channelOffset[7]; if (p.mask & 0x080) pose.scales[j].x += *frameData++ * p.channelScale[7];
			pose.scales[j].y = p.channelOffset[8]; if (p.mask & 0x100) pose.scales[j].y += *frameData++ * p.channelScale[8];
			pose.scales[j].z = p.channelOffset[9]; if (p.mask & 0x200) pose.scales[j].z += *frameData++ * p.channelScale[9];
		}
	}
	*/
	return true;
}