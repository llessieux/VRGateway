////////////////////////////////////////////////////////////////////////////////
// Filename: Model.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Model.h"
#include <string>

bool Model::ObjectMaterialProperties::Equals(const Model::ObjectMaterialProperties &other)
{
    for (int i = 0; i<4; i++)
    {
        if (m_ambient_color[i] != other.m_ambient_color[i])
            return false;
        if (m_diffuse_color[i] != other.m_diffuse_color[i])
            return false;
        if (m_specular_color[i] != other.m_specular_color[i])
            return false;
    }
    if (m_specular_exponent != other.m_specular_exponent)
        return false;
    if (m_texture != other.m_texture)
        return false;

    return true;
}


Model::Model():
    m_scale_x(1.0f),
    m_indexCount(0),
    m_vertexCount(0)
{
}

Model::~Model()
{
}


void Model::SetTexture(std::shared_ptr<Texture> texture)
{
    m_Texture = texture;
}


bool
Model::LoadFromWavefrontFile(const std::string &filename, std::vector<VertexType> &final_vertices, std::vector<unsigned long> &indices, std::vector<StoredMaterial> &libs, const Matrix4 &mat)
{
    if (filename.empty())
        return false;

    FILE *f = NULL;
    std::string directory;
    size_t last_bslash = filename.rfind('\\');
    size_t last_fslash = filename.rfind('/');
    
    if ((last_bslash == std::string::npos)&&
        (last_fslash == std::string::npos))
    {
        directory = ".\\";
    }
    else
    {
        size_t size = (last_bslash == std::string::npos) ? last_fslash : (last_fslash == std::string::npos ? last_bslash : std::max(last_bslash, last_fslash));
        directory = filename;
        directory.resize(size+1);
    }

    errno_t err = fopen_s(&f, filename.c_str(), "r");
    if (err != 0)
        return false;

    if (f)
    {
        char buffer[1024] = { 0 };
        std::vector<D3DXVECTOR3> wvertices;
        std::vector<D3DXVECTOR3> wnormals;
        std::vector<D3DXVECTOR2> wtexcoords;
        std::vector<D3DXVECTOR3> vertices;
        std::vector<D3DXVECTOR3> normals;
        std::vector<D3DXVECTOR2> texcoords;
        //std::vector<unsigned int> colors;
        vertices.reserve(1024);
        wvertices.reserve(1024);
        wnormals.reserve(1024);
        //colors.reserve(1024);
        wtexcoords.reserve(1024);
        std::vector<std::tuple<int, int, int>> witems;
        witems.reserve(4096);
        indices.reserve(4096);
        std::vector<ObjectMaterialProperties> subObjects;

        unsigned int current_index = 0;
        while (!feof(f))
        {
            fgets(buffer, 1024, f);

            char *context;
            char *buffer_ptr = strtok_s(buffer, " \t\r\n", &context);

            if (buffer_ptr)
            {
                switch (buffer_ptr[0])
                {
                case 'm':
                {
                    if (strncmp(buffer_ptr, "mtllib", 6) == 0)
                    {
                        buffer_ptr = strtok_s(NULL, " \r\n", &context);

                        LoadMaterialLibs(directory, buffer_ptr, libs);
                    }
                    break;
                }
                case 'u':
                {
                    if (strncmp(buffer_ptr, "usemtl", 6) == 0)
                    {
                        buffer_ptr = strtok_s(NULL, " ", &context);

                        std::string mat_name(buffer_ptr);
                        std::vector<StoredMaterial>::iterator it = libs.begin();
                        while (it != libs.end())
                        {
                            if (it->name == mat_name)
                            {
                                ObjectMaterialProperties material;

                                material = it->mat;
                                if (material.m_specular_exponent == 0.0)
                                    material.m_specular_exponent = 80.0f;

                                subObjects.push_back(material);
                                break;
                            }
                            it++;
                        }
                    }
                    break;
                }
                case 'g':
                {
                    if (strncmp(buffer_ptr, "g", 1) == 0)
                    {
                        buffer_ptr = strtok_s(NULL, " ", &context);
                        ObjectMaterialProperties material;

                        if (!libs.empty())
                        {
                            material = libs.back().mat;
                            if (material.m_specular_exponent == 0.0)
                                material.m_specular_exponent = 80.0f;

                        }
                        subObjects.push_back(material);
                    }
                    break;
                }
                case 'v':
                    if ((buffer_ptr[1] != 't') &&
                        (buffer_ptr[1] != 'c') &&
                        (buffer_ptr[1] != 'n'))
                    {
                        float flt[3] = { 0.0f, 0.0f, 0.0f };
                        buffer_ptr = strtok_s(NULL, " ", &context);
                        for (int i = 0; i<3; i++)
                        {
                            if (buffer_ptr == NULL)
                            {
                                break;
                            }
                            flt[i] = (float)atof(buffer_ptr);
                            buffer_ptr = strtok_s(NULL, " ", &context);
                        }

                        wvertices.push_back(D3DXVECTOR3(flt[0], flt[1], flt[2]));
                    }
                    else if (buffer[1] == 'n')
                    {
                        float flt[3] = { 0.0f, 0.0f, 0.0f };
                        buffer_ptr = strtok_s(NULL, " ", &context);
                        for (int i = 0; i<3; i++)
                        {
                            if (buffer_ptr == NULL)
                            {
                                break;
                            }
                            flt[i] = (float)atof(buffer_ptr);
                            buffer_ptr = strtok_s(NULL, " ", &context);
                        }
                        wnormals.push_back(D3DXVECTOR3(flt[0], flt[1], flt[2]));
                    }
                    /*else if (buffer[1] == 'c')
                    {
                        buffer_ptr = strtok_s(NULL, " ", &context);
                        unsigned int color;
                        sscanf(buffer_ptr, "%X", &color);
                        colors.push_back(color);
                    }*/
                    else if (buffer[1] == 't')
                    {
                        float texture[2] = { 0.0f, 0.0f };
                        buffer_ptr = strtok_s(NULL, " ", &context);
                        for (int i = 0; i<2; i++)
                        {
                            if (buffer_ptr == NULL)
                            {
                                break;
                            }
                            texture[i] = (float)atof(buffer_ptr);
                            buffer_ptr = strtok_s(NULL, " ", &context);
                        }
                        wtexcoords.push_back(D3DXVECTOR2(texture[0], texture[1]));
                    }
                    break;
                    break;

                case 'f':
                {
                    int v[4] = { 0, 0, 0, 0 };
                    int n[4] = { 0, 0, 0, 0 };
                    int t[4] = { 0, 0, 0, 0 };
                    buffer_ptr = strtok_s(NULL, " ", &context);
                    int ok = 0;
                    for (int i = 0; i<4; i++)
                    {
                        if (buffer_ptr == NULL)
                        {
                            break;
                        }
                        v[i] = atoi(buffer_ptr);
                        t[i] = 0;
                        n[i] = 0;

                        char *slash = strchr(buffer_ptr, '/');
                        if (slash)
                        {
                            if (slash[1] != '/')
                                t[i] = atoi(slash + 1);
                            slash = strchr(slash + 1,'/');
                            if (slash)
                                n[i] = atoi(slash + 1);
                        }
                        if (v[i] != 0)
                            ok++;
                        buffer_ptr = strtok_s(NULL, " ", &context);
                    }

                    auto store_vertex = [&](int v, int t, int n) {
                        auto wv = std::make_tuple(v, t, n);
                        auto it = std::find(witems.begin(), witems.end(), wv );
                        if (it == witems.end())
                        {
                            indices.push_back((unsigned long)witems.size());
                            vertices.push_back(wvertices[v]);
                            if (wnormals.size()!=0)
                                normals.push_back(wnormals[n]);
                            if (wtexcoords.size() != 0)
                                texcoords.push_back(wtexcoords[t]);
                            witems.push_back(wv);
                        }
                        else
                        {
                            indices.push_back((unsigned long)(it - witems.begin()));
                        }
                    };
                    //-1 because the OBJ format starts at 1...
                    if (ok == 3)
                    {
                        store_vertex(v[0] - 1, t[0] - 1, n[0] - 1);
                        store_vertex(v[2] - 1, t[2] - 1, n[2] - 1);
                        store_vertex(v[1] - 1, t[1] - 1, n[1] - 1);
                        current_index += 3;
                    }
                    else if (ok == 4) //Deal with the quad
                    {
                        store_vertex(v[0] - 1, t[0] - 1, n[0] - 1);
                        store_vertex(v[2] - 1, t[2] - 1, n[2] - 1);
                        store_vertex(v[1] - 1, t[1] - 1, n[1] - 1);
                        store_vertex(v[0] - 1, t[0] - 1, n[0] - 1);
                        store_vertex(v[3] - 1, t[3] - 1, n[3] - 1);
                        store_vertex(v[2] - 1, t[2] - 1, n[2] - 1);
                        current_index += 6;
                    }
                }
                break;
                default:
                    break;
                }
            }
        }

        fclose(f);

        if (indices.size() != 0 && vertices.size() != 0)
        {
            if (normals.size() == 0)
            {
                if (vertices.size() != normals.size())
                {
                    //We do not allow vertices with different normals per triangle.
                    //So just go over all triangle and average the normals
                    normals.resize(vertices.size());
                    memset(&normals[0], 0, sizeof(D3DXVECTOR3)*vertices.size());

                    for (int t = 0; t<indices.size(); t += 3)
                    {
                        int i0 = indices[t];
                        int i1 = indices[t + 1];
                        int i2 = indices[t + 2];

                        D3DXVECTOR3 dx = vertices[i1] - vertices[i0];
                        D3DXVECTOR3 dy = vertices[i2] - vertices[i0];

                        D3DXVECTOR3 normal = dx.cross(dy).normalize();

                        normals[i0] += normal;
                        normals[i1] += normal;
                        normals[i2] += normal;
                    }

                    for (int n = 0; n<normals.size(); n++)
                    {
                        //3dmodel code should handle 0,0,0 normals as NaN or null. TODO mabo - check it does?
                        normals[n] = normals[n].normalize();
                    }
                }
            }
            bool store_texture = (texcoords.size() == vertices.size());
            for (auto i = 0; i < vertices.size(); i++)
            {
                auto &vertex = vertices[i];
                auto &normal = normals[i];

                Vector4 v4(vertex.x, vertex.y, vertex.z,1);
                v4 = mat * v4;
                D3DXVECTOR3 v(v4.x,v4.y,v4.z);
                D3DXVECTOR3 n = mat * normal;
                n = n.normalize();

                D3DXVECTOR2 textCooord(0, 0);
                if (store_texture)
                {
                    textCooord = texcoords[i];
                }

                AddVertex(v.x, v.y, v.z, textCooord.x, textCooord.y, final_vertices, n); //Front
            }
        }
    }

    return true;
}


void Model::LoadMaterialLibs(const std::string &directory, const std::string &filename, std::vector<StoredMaterial> &libs)
{
    FILE *f = nullptr;
    std::string fullname = directory + filename;
    errno_t err = fopen_s(&f, fullname.c_str(), "r");
    char buffer[256] = { 0 };
    bool new_mat = false;
    StoredMaterial stm;
    char *context = nullptr;
    char *buffer_ptr = nullptr;

    if (err == 0)
    {
        while (!feof(f))
        {
            fgets(buffer, 256, f);

            buffer_ptr = strtok_s(buffer, " \t\r\n", &context);

            if (buffer_ptr == NULL)
                continue;

            if (strncmp(buffer_ptr, "newmtl", 6) == 0)
            {
                if (new_mat)
                    libs.push_back(stm);

                stm.name.clear();
                stm = StoredMaterial();

                buffer_ptr = strtok_s(NULL, " ", &context);
                new_mat = true;
                stm.name = std::string(buffer_ptr);
            }
            else if (strncmp(buffer_ptr, "Ka", 2) == 0)
            {
                float r, g, b;
                sscanf_s(buffer_ptr + 3, "%f %f %f", &r, &g, &b);
                stm.mat.m_ambient_color[3] = b;
                stm.mat.m_ambient_color[2] = g;
                stm.mat.m_ambient_color[1] = r;
                stm.mat.m_ambient_color[0] = 0;
            }
            else if (strncmp(buffer_ptr, "Kd", 2) == 0)
            {
                float r, g, b;
                sscanf_s(buffer_ptr + 3, "%f %f %f", &r, &g, &b);
                stm.mat.m_diffuse_color[3] = b;
                stm.mat.m_diffuse_color[2] = g;
                stm.mat.m_diffuse_color[1] = r;
                stm.mat.m_diffuse_color[0] = 0;
            }
            else if (strncmp(buffer_ptr, "Ks", 2) == 0)
            {
                float sr, sg, sb;
                sscanf_s(buffer_ptr + 3, "%f %f %f", &sr, &sg, &sb);
                stm.mat.m_specular_color[3] = sb;
                stm.mat.m_specular_color[2] = sg;
                stm.mat.m_specular_color[1] = sr;
                stm.mat.m_specular_color[0] = 0;
            }
            else if (strncmp(buffer_ptr, "Ns", 2) == 0)
            {
                float exp = (float)atof(buffer_ptr + 3);
                stm.mat.m_specular_exponent = exp;
            }
            else if (strncmp(buffer_ptr, "map_Kd", 6) == 0)
            {
                stm.mat.m_texture = std::string(buffer_ptr + 7);
                for (auto &c : stm.mat.m_texture)
                {
                    if (c == 0xD)
                        c = 0;
                    else if (c == 0xA)
                        c = 0;
                }
            }
            /*else if (strncmp(buffer_ptr, "d ", 2) == 0)
            {
                float opacity;
                sscanf_s(buffer_ptr + 2, "%f", &opacity);
                stm.mat.m_opacity = opacity;
            }*/
        }
        if (new_mat)
            libs.push_back(stm);

        //stm.name = NULL;

        fclose(f);
    }
}

bool Model::InitializeFromWavefrontFile(ID3D11Device* device, const std::string &filename, const Matrix4 &mat, std::function<void(const std::vector<VertexType> &vertices, const std::vector<unsigned long> &indices)> callback)
{
    std::vector<VertexType> vertices;
    std::vector<StoredMaterial> libs;
    std::vector<unsigned long> indices;
    if (!LoadFromWavefrontFile(filename, vertices, indices,libs,mat))
        return false;
    /*if (libs.size() != 1)
        return false;*/

    if (!libs[0].mat.m_texture.empty())
    {
        // Load the texture for this model.
        bool result = LoadTexture(device, libs[0].mat.m_texture.c_str());
        if (!result)
        {
            return false;
        }
    }

    callback(vertices, indices);

    // Set up the description of the static vertex buffer.
    D3D11_BUFFER_DESC vertexBufferDesc = {
        (UINT)(sizeof(VertexType) * vertices.size()),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_VERTEX_BUFFER,
        0, 0, 0 };

    // Give the subresource structure a pointer to the vertex data.
    D3D11_SUBRESOURCE_DATA vertexData = { vertices.data(), 0, 0 };

    // Now create the vertex buffer.
    HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    // Set up the description of the static index buffer.
    D3D11_BUFFER_DESC indexBufferDesc = {
        (UINT)(sizeof(unsigned long) * indices.size()) ,
        D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER , 0, 0, 0 };

    // Give the subresource structure a pointer to the index data.
    D3D11_SUBRESOURCE_DATA indexData = { indices.data(), 0,0 };

    // Create the index buffer.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(result))
    {
        return false;
    }

    m_vertexCount = (int)vertices.size();
    m_indexCount = (int)indices.size();

    return true;
}

bool Model::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
    bool result;


    // Initialize the vertex and index buffers.
    result = InitializeBuffers(device);
    if(!result)
    {
        return false;
    }

    // Load the texture for this model.
    result = LoadTexture(device, textureFilename);
    if (!result)
    {
        return false;
    }

    return true;
}

void Model::Shutdown()
{
    // Release the model texture.
    ReleaseTexture();

    // Shutdown the vertex and index buffers.
    ShutdownBuffers();

    return;
}


void Model::Render(ID3D11DeviceContext* deviceContext)
{
    // Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
    RenderBuffers(deviceContext);

    return;
}


int Model::GetIndexCount()
{
    return m_indexCount;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
    if (m_Texture)
        return m_Texture->GetTexture();
    return nullptr;
}

void Model::AddVertex(float x, float y, float z, float tx, float ty, std::vector<VertexType> &vertdata, D3DXVECTOR3 normal)
{
    VertexType temp;
    temp.position.set(x, y, z);
    temp.texture.set(tx, ty);
    temp.normal = normal;
    vertdata.push_back(temp);
}


void Model::AddCubeToScene(Matrix4 mat, std::vector<VertexType> &vertdata, std::vector<unsigned long> &indices)
{
    // Matrix4 mat( outermat.data() );
    float depth = 0.01f;
    Vector4 A = mat * Vector4(-0.5f, 0, 0, 1);
    Vector4 B = mat * Vector4(0.5f* m_scale_x, 0, 0, 1);
    Vector4 C = mat * Vector4(0.5f* m_scale_x, 1, 0, 1);
    Vector4 D = mat * Vector4(-0.5f, 1, 0, 1);
    Vector4 E = mat * Vector4(-0.5f, 0, depth, 1);
    Vector4 F = mat * Vector4(0.5f* m_scale_x, 0, depth, 1);
    Vector4 G = mat * Vector4(0.5f* m_scale_x, 1, depth, 1);
    Vector4 H = mat * Vector4(-0.5f, 1, depth, 1);

    int old_vertex_index = (int)vertdata.size();
    // triangles instead of quads
    D3DXVECTOR3 normal;
    normal.set(0, 0, -1);
    AddVertex(E.x, E.y, E.z, 0, 1, vertdata, normal); //Front
    AddVertex(F.x, F.y, F.z, 1, 1, vertdata, normal);
    AddVertex(G.x, G.y, G.z, 1, 0, vertdata, normal);
    AddVertex(G.x, G.y, G.z, 1, 0, vertdata, normal);
    AddVertex(H.x, H.y, H.z, 0, 0, vertdata, normal);
    AddVertex(E.x, E.y, E.z, 0, 1, vertdata, normal);

    normal.set(0, 0, 1);
    AddVertex(B.x, B.y, B.z, 0, 1, vertdata, normal); //Back
    AddVertex(A.x, A.y, A.z, 1, 1, vertdata, normal);
    AddVertex(D.x, D.y, D.z, 1, 0, vertdata, normal);
    AddVertex(D.x, D.y, D.z, 1, 0, vertdata, normal);
    AddVertex(C.x, C.y, C.z, 0, 0, vertdata, normal);
    AddVertex(B.x, B.y, B.z, 0, 1, vertdata, normal);

    normal.set(0, 1, 0);
    AddVertex(H.x, H.y, H.z, 0, 1, vertdata, normal); //Top
    AddVertex(G.x, G.y, G.z, 1, 1, vertdata, normal);
    AddVertex(C.x, C.y, C.z, 1, 0, vertdata, normal);
    AddVertex(C.x, C.y, C.z, 1, 0, vertdata, normal);
    AddVertex(D.x, D.y, D.z, 0, 0, vertdata, normal);
    AddVertex(H.x, H.y, H.z, 0, 1, vertdata, normal);

    normal.set(0, -1, 0);
    AddVertex(A.x, A.y, A.z, 0, 1, vertdata, normal); //Bottom
    AddVertex(B.x, B.y, B.z, 1, 1, vertdata, normal);
    AddVertex(F.x, F.y, F.z, 1, 0, vertdata, normal);
    AddVertex(F.x, F.y, F.z, 1, 0, vertdata, normal);
    AddVertex(E.x, E.y, E.z, 0, 0, vertdata, normal);
    AddVertex(A.x, A.y, A.z, 0, 1, vertdata, normal);

    normal.set(-1, 0, 0);
    AddVertex(A.x, A.y, A.z, 0, 1, vertdata, normal); //Left
    AddVertex(E.x, E.y, E.z, 1, 1, vertdata, normal);
    AddVertex(H.x, H.y, H.z, 1, 0, vertdata, normal);
    AddVertex(H.x, H.y, H.z, 1, 0, vertdata, normal);
    AddVertex(D.x, D.y, D.z, 0, 0, vertdata, normal);
    AddVertex(A.x, A.y, A.z, 0, 1, vertdata, normal);

    normal.set(1, 0, 0);
    AddVertex(F.x, F.y, F.z, 0, 1, vertdata, normal); //Right
    AddVertex(B.x, B.y, B.z, 1, 1, vertdata, normal);
    AddVertex(C.x, C.y, C.z, 1, 0, vertdata, normal);
    AddVertex(C.x, C.y, C.z, 1, 0, vertdata, normal);
    AddVertex(G.x, G.y, G.z, 0, 0, vertdata, normal);
    AddVertex(F.x, F.y, F.z, 0, 1, vertdata, normal);

    int new_vertex_index = (int)vertdata.size();

    for (int i = old_vertex_index; i < new_vertex_index; i++)
    {
        indices.push_back(i);
    }
}


bool Model::InitializeBuffers(ID3D11Device* device)
{
    std::vector<VertexType> vertices;
    std::vector<unsigned long> indices;

    float fScale = 0.05f;
    Matrix4 matScale;
    matScale.scale(fScale, fScale, fScale);

    Matrix4 mat = matScale;

    AddCubeToScene(mat, vertices, indices);

    for (int i = 0; i < vertices.size() / 2; i++)
    {
        std::swap(vertices[i], vertices[vertices.size() - i - 1]);
    }
    // Set up the description of the static vertex buffer.
    D3D11_BUFFER_DESC vertexBufferDesc = {
        (UINT)(sizeof(VertexType) * vertices.size()),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_VERTEX_BUFFER,
        0, 0, 0 };

    // Give the subresource structure a pointer to the vertex data.
    D3D11_SUBRESOURCE_DATA vertexData = { vertices.data(), 0, 0 };

    // Now create the vertex buffer.
    HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    // Set up the description of the static index buffer.
    D3D11_BUFFER_DESC indexBufferDesc = {
        (UINT)(sizeof(unsigned long) * indices.size()),
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_INDEX_BUFFER,
        0, 0, 0 };

    // Give the subresource structure a pointer to the index data.
    D3D11_SUBRESOURCE_DATA indexData = { indices.data(), 0, 0 };

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


void Model::ShutdownBuffers()
{
    // Release the index buffer.
    if(m_indexBuffer)
    {
        m_indexBuffer.Release();
    }

    // Release the vertex buffer.
    if(m_vertexBuffer)
    {
        m_vertexBuffer.Release();
    }

    return;
}


void Model::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    // Set vertex buffer stride and offset.
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;
    
    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return;
}

void Model::ReleaseTexture()
{
    // Release the texture object.
    if (m_Texture)
    {
        m_Texture->Shutdown();
        m_Texture.reset();
    }

    return;
}

