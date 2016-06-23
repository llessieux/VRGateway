////////////////////////////////////////////////////////////////////////////////
// Filename: Model.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _Model_H_
#define _Model_H_


//////////////
// INCLUDES //
//////////////
#include "Matrices.h"
#include <vector>
#include <functional>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Texture.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: Model
////////////////////////////////////////////////////////////////////////////////
class Model
{
public:
    struct VertexType
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
        D3DXVECTOR3 normal;

        VertexType() {}
    };

    class ObjectMaterialProperties
    {
    public:
        ObjectMaterialProperties()
        {
            for (int i = 1; i <= 3; i++)
            {
                m_ambient_color[i] = 0.2f;
                m_diffuse_color[i] = 0.8f;
                m_specular_color[i] = 0.8f;
            }
            m_ambient_color[0] = 0;
            m_diffuse_color[0] = 0;
            m_specular_color[0] = 0;
            m_specular_exponent = 80;
        }

        bool Equals(const ObjectMaterialProperties &other);
        float m_ambient_color[4];
        float m_diffuse_color[4];
        float m_specular_color[4];
        float m_specular_exponent;
        std::string m_texture;
    };


    class StoredMaterial
    {
    public:
        StoredMaterial()
        {
        }
        ~StoredMaterial()
        {
        }

        ObjectMaterialProperties mat;
        std::string name;
    };

public:
    Model();
    virtual ~Model();

    bool Initialize(ID3D11Device*, WCHAR*);
    bool InitializeFromWavefrontFile(ID3D11Device*, const std::string &, const Matrix4 &, std::function<void(const std::vector<VertexType> &vertices, const std::vector<unsigned long> &indices)> callback);
    void Shutdown();
    void Render(ID3D11DeviceContext*);

    int GetIndexCount();

    ID3D11ShaderResourceView* GetTexture();

    bool InitializeBuffers(ID3D11Device*);

    void SetTexture(std::shared_ptr<Texture> texture);

protected:
    
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

    void ReleaseTexture();

    void AddVertex(float x, float y, float z, float tx, float ty, std::vector<VertexType> &vertdata, D3DXVECTOR3 normal);
    void AddCubeToScene(Matrix4 mat, std::vector<VertexType> &vertdata, std::vector<unsigned long> &indices);

    bool LoadFromWavefrontFile(const std::string &filename, std::vector<VertexType> &final_vertices, std::vector<unsigned long> &indices, std::vector<StoredMaterial> &libs, const Matrix4 &mat);
    void LoadMaterialLibs(const std::string &directory, const std::string &filename, std::vector<StoredMaterial> &libs);

    template<typename T>
    bool Model::LoadTexture(ID3D11Device* device, T* filename)
    {
        // Create the texture object.
        m_Texture = std::make_shared<Texture>();
        if (m_Texture.get() == nullptr) { return false; }

        // Initialize the texture object.
        return m_Texture->Initialize(device, filename);
    }


    Model(const Model&);

protected:

    CComPtr<ID3D11Buffer> m_vertexBuffer;
    CComPtr<ID3D11Buffer> m_indexBuffer;
    unsigned int m_vertexCount;
    unsigned int m_indexCount;

    std::shared_ptr<Texture> m_Texture;
    float m_scale_x;
};

#endif