#include "stdafx.h"
#include "DesktopGrabber.h"

DesktopGrabber::DesktopGrabber(ID3D11DeviceContext* context) :
    m_DeskDupl(nullptr),
    m_SharedSurf(nullptr),
    m_Dx11Context(context)
{
}


DesktopGrabber::~DesktopGrabber()
{
}


HRESULT DesktopGrabber::InitDupl(ID3D11Device *pDevice, const RECT &PsPrect)
{
    m_pDevice = pDevice;

    // Get DXGI device
    CComPtr<IDXGIDevice> DxgiDevice;
    HRESULT hr = pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DxgiDevice));
    if (FAILED(hr))
    {
        return hr;
    }

    // Get DXGI adapter
    CComPtr<IDXGIAdapter> DxgiAdapter;
    hr = DxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DxgiAdapter));
    if (FAILED(hr))
    {
        return hr;
    }

    // Get output
    CComPtr<IDXGIOutput> DxgiOutput;
    hr = DxgiAdapter->EnumOutputs(0, &DxgiOutput);
    if (FAILED(hr))
    {
        return hr;
    }

    //DxgiOutput->GetDesc(&m_OutputDesc);

    // QI for Output 1
    CComPtr<IDXGIOutput1> DxgiOutput1;
    hr = DxgiOutput->QueryInterface(__uuidof(DxgiOutput1), reinterpret_cast<void**>(&DxgiOutput1));
    if (FAILED(hr))
    {
        return hr;
    }

    // Create desktop duplication
    hr = DxgiOutput1->DuplicateOutput(pDevice, &m_DeskDupl);
    if (FAILED(hr))
    {
        return hr;
    }

    // Create shared texture for all duplication threads to draw into
    D3D11_TEXTURE2D_DESC DeskTexD;
    RtlZeroMemory(&DeskTexD, sizeof(D3D11_TEXTURE2D_DESC));
    DeskTexD.Width = PsPrect.right - PsPrect.left + 1;
    DeskTexD.Height = PsPrect.bottom - PsPrect.top + 1;
    DeskTexD.MipLevels = 1;
    DeskTexD.ArraySize = 1;
    DeskTexD.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    DeskTexD.SampleDesc.Count = 1;
    DeskTexD.Usage = D3D11_USAGE_DEFAULT;
    DeskTexD.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    DeskTexD.CPUAccessFlags = 0;
    DeskTexD.MiscFlags = 0; // D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

    hr = pDevice->CreateTexture2D(&DeskTexD, nullptr, &m_SharedSurf);
    if (FAILED(hr))
    {
        OutputError(hr, __FILE__, __LINE__);
        return hr;
    }

    return S_OK;
}

HRESULT DesktopGrabber::SimpleCopy(const RECT &rect, ID3D11Texture2D* acquiredDesktopImage)
{
    int offsetX = rect.left;
    int offsetY = rect.top;
    int right = rect.right;
    int bottom = rect.bottom;

    D3D11_BOX Box;
    Box.left = offsetX;
    Box.top = offsetY;
    Box.front = 0;
    Box.right = right;
    Box.bottom = bottom;
    Box.back = 1;
    m_Dx11Context->CopySubresourceRegion(m_SharedSurf, 0, 0, 0, 0, acquiredDesktopImage, 0, &Box);
    return S_OK;
}

HRESULT DesktopGrabber::GetFrame(bool* Timeout, const RECT &rect)
{
    CComPtr<IDXGIResource> DesktopResource;
    CComPtr<ID3D11Texture2D> AcquiredDesktopImage;
    DXGI_OUTDUPL_FRAME_INFO FrameInfo;

    // Get new frame
    HRESULT hr = m_DeskDupl->AcquireNextFrame(0, &FrameInfo, &DesktopResource);
    if (hr == DXGI_ERROR_WAIT_TIMEOUT)
    {
        *Timeout = true;
        return S_FALSE;
    }
    *Timeout = false;

    if (FAILED(hr))
    {
        OutputError(hr, __FILE__, __LINE__);
        return hr;
    }

    // QI for IDXGIResource
    hr = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&AcquiredDesktopImage));
    if (FAILED(hr))
    {
        OutputError(hr, __FILE__, __LINE__);
        return hr;
    }

    hr = SimpleCopy(rect, AcquiredDesktopImage);
    if (FAILED(hr))
    {
        OutputError(hr, __FILE__, __LINE__);
    }

    hr = m_DeskDupl->ReleaseFrame();
    if (FAILED(hr))
    {
        OutputError(hr, __FILE__, __LINE__);
        return hr;
    }
    return S_OK;
}

ID3D11ShaderResourceView* 
DesktopGrabber::GetDesktopResourceView()
{
    D3D11_TEXTURE2D_DESC FrameDesc;
    m_SharedSurf->GetDesc(&FrameDesc);

    D3D11_SHADER_RESOURCE_VIEW_DESC ShaderDesc;
    ShaderDesc.Format = FrameDesc.Format;
    ShaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    ShaderDesc.Texture2D.MostDetailedMip = FrameDesc.MipLevels - 1;
    ShaderDesc.Texture2D.MipLevels = FrameDesc.MipLevels;

    // Create new shader resource view
    ID3D11ShaderResourceView* ShaderResource = nullptr;
    //AutoRelease sr(ShaderResource);
    HRESULT hr = m_pDevice->CreateShaderResourceView(m_SharedSurf, &ShaderDesc, &ShaderResource);
    if (FAILED(hr))
    {
        OutputError(hr, __FILE__, __LINE__);
        return nullptr;
    }
    return ShaderResource;
}