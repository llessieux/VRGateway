////////////////////////////////////////////////////////////////////////////////
// Filename: renderTexture.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _RENDERTexture_H_
#define _RENDERTexture_H_

////////////////////////////////////////////////////////////////////////////////
// Class name: RenderTexture
////////////////////////////////////////////////////////////////////////////////
class RenderTexture
{
public:
    RenderTexture();
    ~RenderTexture();

    bool Initialize(ID3D11Device*, int, int);
    void Shutdown();

    void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
    void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
    ID3D11ShaderResourceView* GetShaderResourceView();
    ID3D11Texture2D* GetTexture();

private:
    RenderTexture(const RenderTexture&);
    
    ID3D11Texture2D* m_renderTargetTexture;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11ShaderResourceView* m_shaderResourceView;
};

#endif