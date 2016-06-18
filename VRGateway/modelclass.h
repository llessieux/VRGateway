////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "Matrices.h"
#include <vector>
#include <functional>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
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
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, WCHAR*);
    bool InitializeFromWavefrontFile(ID3D11Device*, const char*, const Matrix4 &, std::function<void(const std::vector<VertexType> &vertices, const std::vector<unsigned long> &indices)> callback);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();

protected:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	void ReleaseTexture();

	void AddVertex(float x, float y, float z, float tx, float ty, std::vector<VertexType> &vertdata, D3DXVECTOR3 normal);
	void AddCubeToScene(Matrix4 mat, std::vector<VertexType> &vertdata, std::vector<unsigned long> &indices);

    bool LoadFromWavefrontFile(const char *filename, std::vector<VertexType> &final_vertices, std::vector<unsigned long> &indices, std::vector<StoredMaterial> &libs, const Matrix4 &mat);
    void LoadMaterialLibs(char *directory, char *filename, std::vector<StoredMaterial> &libs);

    template<typename T>
    bool ModelClass::LoadTexture(ID3D11Device* device, T* filename)
    {
        // Create the texture object.
        m_Texture = new TextureClass;
        if (!m_Texture) { return false; }

        // Initialize the texture object.
        return m_Texture->Initialize(device, filename);
    }


protected:

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	TextureClass* m_Texture;
    float m_scale_x;
};

#endif