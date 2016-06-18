#pragma once

class DesktopGrabber
{
public:
    DesktopGrabber(ID3D11DeviceContext* context);
    virtual ~DesktopGrabber();

    HRESULT InitDupl(ID3D11Device *pDevice, const RECT &PsPrect);
    HRESULT GetFrame(bool* Timeout, const RECT &rect);
    ID3D11ShaderResourceView* GetDesktopResourceView();

private:
    HRESULT SimpleCopy(const RECT &rect, ID3D11Texture2D* acquiredDesktopImage);

    CComPtr<ID3D11Device> m_pDevice;
    CComPtr<ID3D11DeviceContext> m_Dx11Context;
    CComPtr<IDXGIOutputDuplication> m_DeskDupl;
    CComPtr<ID3D11Texture2D>        m_SharedSurf;
};

