////////////////////////////////////////////////////////////////////////////////
// Filename: Model.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Screen.h"


Screen::Screen()
{
    m_screen_size = 8.0f;
    m_screen_position = Vector3(0, 0, -6.0f);
}


Screen::Screen(const Screen& other)
{
}


Screen::~Screen()
{
}

void Screen::SetScreenSize(float size)
{
    m_screen_size = size;
}

void Screen::SetScreenPosition(Vector3 screen_position)
{
    m_screen_position = screen_position;
}

bool Screen::Initialize(ID3D11Device* device, float scale_x, float texture_scale, float texture_offset)
{
	bool result;

    m_scale_x = scale_x;
    m_texture_scale = texture_scale;
    m_texture_offset = texture_offset;

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	return result;
}


void Screen::AddScreenToScene(Matrix4 mat, std::vector<VertexType> &vertdata, std::vector<unsigned long> &indices)
{
    // Matrix4 mat( outermat.data() );
    Vector4 A = mat * Vector4(-0.5f* m_scale_x, 0, 0, 1);
    Vector4 B = mat * Vector4(0.5f* m_scale_x, 0, 0, 1);
    Vector4 C = mat * Vector4(0.5f* m_scale_x, 1, 0, 1);
    Vector4 D = mat * Vector4(-0.5f* m_scale_x, 1, 0, 1);
    
    int old_vertex_index = (int)vertdata.size();
    // triangles instead of quads
    D3DXVECTOR3 normal;
    normal.set(0, 0, -1);

    AddVertex(A.x, A.y, A.z, m_texture_offset, 1.0f, vertdata, normal);
    AddVertex(B.x, B.y, B.z, 1.0f* m_texture_scale+ m_texture_offset, 1.0f, vertdata, normal); //Back
    AddVertex(C.x, C.y, C.z, 1.0f* m_texture_scale+ m_texture_offset, 0, vertdata, normal);
    AddVertex(D.x, D.y, D.z, m_texture_offset, 0, vertdata, normal);

    indices.push_back(old_vertex_index + 1);
    indices.push_back(old_vertex_index);
    indices.push_back(old_vertex_index + 3);
    indices.push_back(old_vertex_index + 3);
    indices.push_back(old_vertex_index + 2);
    indices.push_back(old_vertex_index + 1);
}

bool Screen::InitializeBuffers(ID3D11Device* device)
{
    std::vector<VertexType> vertices;
    std::vector<unsigned long> indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;

    Matrix4 matScale;
    matScale.scale(m_screen_size, m_screen_size, 1);
    Matrix4 matTransform;
    matTransform.translate(m_screen_position);

    Matrix4 mat = matScale * matTransform;


	AddScreenToScene(mat, vertices, indices);

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = (UINT)(sizeof(VertexType) * vertices.size());
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = &vertices[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = (UINT)(sizeof(unsigned long) * indices.size());
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = &indices[0];
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	m_vertexCount = (int)vertices.size();
	m_indexCount = (int)indices.size();

	return true;
}

void Screen::SetTexture(Texture *texture)
{
    m_Texture = texture;
}
