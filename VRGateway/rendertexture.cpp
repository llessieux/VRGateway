////////////////////////////////////////////////////////////////////////////////
// Filename: renderTexture.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "renderTexture.h"


RenderTexture::RenderTexture()
{
    m_renderTargetTexture = 0;
    m_renderTargetView = 0;
    m_shaderResourceView = 0;
}

RenderTexture::~RenderTexture()
{
}


bool RenderTexture::Initialize(ID3D11Device* device, int textureWidth, int textureHeight)
{
    D3D11_TEXTURE2D_DESC textureDesc;

    // Initialize the render target texture description.
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    // Setup the render target texture description.
    textureDesc.Width = textureWidth;
    textureDesc.Height = textureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Create the render target texture.
    HRESULT result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
    if(FAILED(result))
    {
        return false;
    }

    // Setup the description of the render target view.
    D3D11_RENDER_TARGET_VIEW_DESC  renderTargetViewDesc = { textureDesc.Format, D3D11_RTV_DIMENSION_TEXTURE2D, 0 };

    // Create the render target view.
    result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
    if(FAILED(result))
    {
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

    // Setup the description of the shader resource view.
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    // Create the shader resource view.
    result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
    if(FAILED(result))
    {
        return false;
    }

    return true;
}


void RenderTexture::Shutdown()
{
    if(m_shaderResourceView)
    {
        m_shaderResourceView->Release();
        m_shaderResourceView = 0;
    }

    if(m_renderTargetView)
    {
        m_renderTargetView->Release();
        m_renderTargetView = 0;
    }

    if(m_renderTargetTexture)
    {
        m_renderTargetTexture->Release();
        m_renderTargetTexture = 0;
    }
}


void RenderTexture::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
    // Bind the render target view and depth stencil buffer to the output render pipeline.
    deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView);
}


void RenderTexture::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, 
                                           float red, float green, float blue, float alpha)
{
    // Setup the color to clear the buffer to.
    float color[4] = { red, green, blue, alpha };

    // Clear the back buffer.
    deviceContext->ClearRenderTargetView(m_renderTargetView, color);
    
    // Clear the depth buffer.
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}


ID3D11ShaderResourceView* RenderTexture::GetShaderResourceView()
{
    return m_shaderResourceView;
}


ID3D11Texture2D* RenderTexture::GetTexture()
{
    return m_renderTargetTexture;
}