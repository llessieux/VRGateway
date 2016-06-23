////////////////////////////////////////////////////////////////////////////////
// Filename: Texture.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _Texture_H_
#define _Texture_H_


////////////////////////////////////////////////////////////////////////////////
// Class name: Texture
////////////////////////////////////////////////////////////////////////////////
class Texture
{
public:
    Texture();
    ~Texture();

    //Initialize the texture via a filename
    bool Initialize(ID3D11Device* device, const WCHAR* filename);
    bool Initialize(ID3D11Device* device, const char* filename);


    void Shutdown();

    ID3D11ShaderResourceView* GetTexture();

    void SetTexture(ID3D11ShaderResourceView* texture);

private:
    Texture(const Texture&);
    
    CComPtr<ID3D11ShaderResourceView> m_texture;
};

#endif