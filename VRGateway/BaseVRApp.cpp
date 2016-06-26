#include "stdafx.h"
#include "camera.h"
#include "Model.h"
#include "Screen.h"
#include "colorshader.h"
#include "renderTexture.h"
#include "debugwindow.h"
#include "DesktopGrabber.h"
#include "TaikoVRApp.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "openvr_api.lib")
#pragma optimize ("",off)
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
const float MOVE_STEP = 0.3f;
const float ROTATE_STEP = 5;

BaseVRApp::BaseVRApp() :
    m_source_parent_window(0),
    m_source_window(0),
    m_errorshown(false),
    m_keyboard_handle(0),
    m_pRenderModels(nullptr)
{
    m_rTrackedDevicePose.resize(vr::k_unMaxTrackedDeviceCount);
    m_rmat4DevicePose.resize(vr::k_unMaxTrackedDeviceCount);
    m_rDevClassChar.resize(vr::k_unMaxTrackedDeviceCount);

    m_background_colors[0] = 0.0f;
    m_background_colors[1] = 0.0f;
    m_background_colors[2] = 1.0f;
}


BaseVRApp::~BaseVRApp()
{
    clean();
}

const wchar_t *BaseVRApp::GetSourceParentClassName() const
{
    return L"PPSSPPWnd";
}

const wchar_t *BaseVRApp::GetSourceParentWindowName() const
{
    return nullptr;
}

const wchar_t *BaseVRApp::GetSourceClassName() const
{
    return L"PPSSPPDisplay";
}
const wchar_t *BaseVRApp::GetSourceWindowName() const
{
    return nullptr;
}

void BaseVRApp::SetSourceParentHWnd(HWND hwnd)
{
    m_source_parent_window = hwnd;
}

void BaseVRApp::SetSourceHWnd(HWND hwnd)
{
    m_source_window = hwnd;
}

BOOL CALLBACK MyEnumWindowsProc(HWND   hwnd,LPARAM lParam)
{
    BOOL found = TRUE;

    std::pair<BaseVRApp *, bool> *data = (std::pair<BaseVRApp *, bool> *)lParam;
    const wchar_t *sclassName = data->second ? data->first->GetSourceParentClassName() : data->first->GetSourceClassName();
    const wchar_t *sWindowTitle = data->second ? data->first->GetSourceParentWindowName() : data->first->GetSourceWindowName();

    if (sWindowTitle)
    {
        wchar_t title[256];
        GetWindowText(hwnd, title, 256);
        found = (wcsstr(title, sWindowTitle) != nullptr) ? TRUE : FALSE;
    }

    if (sclassName)
    {
        wchar_t className[256];
        GetClassName(hwnd, className, 256);
        found = (wcsstr(className, sclassName) != nullptr) ? found : FALSE;
    }
    if (sclassName == nullptr && sWindowTitle == nullptr)
        return FALSE;

    if (found)
    {
        if (data->second)
            data->first->SetSourceParentHWnd(hwnd);
        else
            data->first->SetSourceHWnd(hwnd);
        return FALSE;
    }

    return TRUE;
}


bool BaseVRApp::GetTargetRect(RECT &rect)
{
    if (m_source_parent_window == 0)
    {
        m_source_parent_window = FindWindow(GetSourceParentClassName(), GetSourceParentWindowName());
    }

    if (m_source_parent_window == 0)
    {
        std::pair<BaseVRApp *, bool> data(this, true);
        EnumWindows(MyEnumWindowsProc, (LPARAM)&data);
    }
    
    if (m_source_parent_window == 0)
        return false;

    if (m_source_parent_window)
        SetWindowPos(m_source_parent_window, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

    m_source_window = FindWindowEx(m_source_parent_window, 0, GetSourceClassName(), GetSourceWindowName());

    if (m_source_window == 0)
    {
        std::pair<BaseVRApp *, bool> data(this, false);
        EnumChildWindows(m_source_parent_window, MyEnumWindowsProc, (LPARAM)&data);

        if (m_source_window == 0)
            return false;
    }

    if (m_source_window)
    {
        GetWindowRect(m_source_window, &rect);
        return true;
    }

    return false;
}
// this function initializes D3D and VR
bool BaseVRApp::init(HWND hWnd)
{
    UINT createDeviceFlags = 0;

    RECT PsPrect;
    if (!GetTargetRect(PsPrect))
    {
        OutputError(E_FAIL, __FILE__, __LINE__);
        return false;
    }

#ifdef DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

#ifndef VR_DISABLED

    // Loading the SteamVR Runtime
    vr::EVRInitError eError = vr::VRInitError_None;

    m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

    if (eError != vr::VRInitError_None)
    {
        m_pHMD = NULL;
        char buf[1024];
        sprintf_s(buf, ARRAYSIZE(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
        string temp(buf);
        wstring wtemp(temp.begin(), temp.end());
        MessageBox(hWnd, wtemp.c_str(), L"VR_Init Failed", 0);
        return false;
    }

    m_pHMD->GetRecommendedRenderTargetSize(&m_nRenderWidth, &m_nRenderHeight);

    DebugPrint("width = %d, height = %d", m_nRenderWidth, m_nRenderHeight);


    m_pRenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError);
    if (!m_pRenderModels)
    {
        m_pHMD = NULL;
        vr::VR_Shutdown();

        char buf[1024];
        sprintf_s(buf, ARRAYSIZE(buf), "Unable to get render model interface: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
        string temp(buf);
        wstring wtemp(temp.begin(), temp.end());
        MessageBox(hWnd, wtemp.c_str(), L"VR_Init Failed", NULL);
        return false;
    }

    if (!vr::VRCompositor())
    {
        DebugPrint("Compositor initialization failed. See log file for details\n");
        return false;
    }

#endif

    // CREATE DEVICE AND SWAP CHAIN
    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE, // the first thing to try, if failed, go to the next
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0, // texture size and others..
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC swapDesc;
    ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
    swapDesc.BufferCount = 1;
    swapDesc.BufferDesc.Width = VRCLIENTWIDTH;
    swapDesc.BufferDesc.Height = VRCLIENTHEIGHT;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // unsigned normal
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = hWnd;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapDesc.Windowed = true;
    swapDesc.SampleDesc.Count = 1; // multisampling, which antialiasing for geometry. Turn it off
    swapDesc.SampleDesc.Quality = 0;
    swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // alt-enter fullscreen

    swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    HRESULT errorCode = E_FAIL;
    for (unsigned i = 0; i < numDriverTypes; ++i)
    {
        errorCode = D3D11CreateDeviceAndSwapChain(NULL, driverTypes[i], NULL, createDeviceFlags,
            featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice,
            &m_featureLevel, &m_pImmediateContext);

        if (SUCCEEDED(errorCode))
        {
            m_driverType = driverTypes[i];
            break;
        }
    }

    if (FAILED(errorCode))
    {
        OutputDebugString(_T("FAILED TO CREATE DEVICE AND SWAP CHAIN"));
        MyDebugDlg(_T("FAILED TO CREATE DEVICE AND SWAP CHAIN"));
        return false;
    }

    CComPtr<ID3D11Texture2D> pBackBufferTex;
    HRESULT result;
    // CREATE RENDER TARGET VIEW
    result = m_pSwapChain->GetBuffer(NULL, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBufferTex));
    if (FAILED(result))
    {
        return false;
    }

    D3D11_TEXTURE2D_DESC BBDesc;
    ZeroMemory(&BBDesc, sizeof(D3D11_TEXTURE2D_DESC));
    pBackBufferTex->GetDesc(&BBDesc);

    D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
    ZeroMemory(&RTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
    RTVDesc.Format = BBDesc.Format;
    RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    //RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
    RTVDesc.Texture2D.MipSlice = 0;

    result = m_pDevice->CreateRenderTargetView(pBackBufferTex, &RTVDesc, &m_pRenderTargetView);
    if (FAILED(result))
    {
        MyDebugDlg(_T("ERROR"));
    }

    // CREATE DEPTH STENCIL
    CComPtr<ID3D11Texture2D> pDepthStencil;
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = m_nRenderWidth;// swapDesc.BufferDesc.Width;
    descDepth.Height = m_nRenderHeight;// swapDesc.BufferDesc.Height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;// DXGI_FORMAT_D32_FLOAT;//DXGI_FORMAT_D24_UNORM_S8_UINT;;//pDeviceSettings->d3d11.AutoDepthStencilFormat;
                                                        // DXGI_FORMAT_D32_FLOAT_S8X24_UINT
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    result = m_pDevice->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
    if (FAILED(result))
        return false;


    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    // Depth test parameters
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    // Stencil test parameters
    dsDesc.StencilEnable = true;
    dsDesc.StencilReadMask = 0xFF;
    dsDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create depth stencil state
    result = m_pDevice->CreateDepthStencilState(&dsDesc, &m_pDSState);
    if (FAILED(result))
    {
        return false;
    }

    // Bind depth stencil state
    m_pImmediateContext->OMSetDepthStencilState(m_pDSState, 1);



    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;// DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    //descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    descDSV.Texture2D.MipSlice = 0;

    // Create the depth stencil view
    result = m_pDevice->CreateDepthStencilView(pDepthStencil, // Depth stencil texture
        &descDSV, // Depth stencil desc
        &m_pDepthStencilView);  // [out] Depth stencil view

    if (FAILED(result))
    {
        DebugPrint("%x", result);
        return false;
    }

    //BIND RENDER TARGET VIEW
    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView); // depth stencil view is for shadow map


    D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
    // Clear the second depth stencil state before setting the parameters.
    ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

    // Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
    // that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
    depthDisabledStencilDesc.DepthEnable = false;
    depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthDisabledStencilDesc.StencilEnable = true;
    depthDisabledStencilDesc.StencilReadMask = 0xFF;
    depthDisabledStencilDesc.StencilWriteMask = 0xFF;
    depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the state using the device.
    if (FAILED(m_pDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState)))
    {
        return false;
    }

    //VIEWPORT CREATION
    m_viewport.Width = static_cast<float>(m_nRenderWidth);
    m_viewport.Height = static_cast<float>(m_nRenderHeight);
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    // BIND VIEWPORT
    m_pImmediateContext->RSSetViewports(1, &m_viewport);


    m_DesktopGrabber.reset(new DesktopGrabber(m_pImmediateContext));
    HRESULT hr = m_DesktopGrabber->InitDupl(m_pDevice, PsPrect);
    if (FAILED(hr))
    {
        OutputError(hr, __FILE__, __LINE__);
        return false;
    }

    ID3D11ShaderResourceView*ShaderResource = m_DesktopGrabber->GetDesktopResourceView();
    if (ShaderResource == nullptr)
    {
        return false;
    }

    m_source_texture.reset(new Texture());
    m_source_texture->SetTexture(ShaderResource);

    // Create the camera object.
    m_CameraLeft.reset(new Camera);
    if (!m_CameraLeft)
    {
        return false;
    }

    // Set the initial position of the camera.
    m_CameraLeft->SetPosition(0.0f, 0.0f, -10.0f);

    // Create the camera object.
    m_CameraRight.reset(new Camera);
    if (!m_CameraRight)
    {
        return false;
    }

    // Set the initial position of the camera.
    m_CameraRight->SetPosition(1.5f, 0.0f, -10.0f);

    if (!setupWorld())
        return false;

    if (NeedScreen())
    {
        m_Screen.reset(new Screen);

        int rect_width = PsPrect.right - PsPrect.left + 1;
        int rect_height = PsPrect.bottom - PsPrect.top + 1;

        float sx = (float)rect_width / (float)rect_height;

        ConfigureScreen();

        // Initialize the model object.
        if (!m_Screen->Initialize(m_pDevice, sx))
        {
            MyDebugDlg(L"Could not initialize the model object.");
            return false;
        }
    }

    // Create the color shader object.
    m_ColorShader.reset(new ColorShader);
    if (!m_ColorShader)
    {
        return false;
    }

    // Initialize the color shader object.
    if (!m_ColorShader->Initialize(m_pDevice, hWnd))
    {
        MyDebugDlg(L"Could not initialize the color shader object.");
        return false;
    }


    // Create the render to texture object.
    m_RenderTextureLeft.reset(new RenderTexture);
    if (!m_RenderTextureLeft)
    {
        return false;
    }

    // Initialize the render to texture object.
    if (!m_RenderTextureLeft->Initialize(m_pDevice, m_nRenderWidth, m_nRenderHeight))
    {
        return false;
    }

    m_RenderTextureRight.reset(new RenderTexture);
    if (!m_RenderTextureRight)
    {
        return false;
    }

    // Initialize the render to texture object.
    if (!m_RenderTextureRight->Initialize(m_pDevice, m_nRenderWidth, m_nRenderHeight))
    {
        return false;
    }

    // Create the debug window object.
    m_DebugWindowLeft.reset(new DebugWindow);
    if (!m_DebugWindowLeft)
    {
        return false;
    }

    // Initialize the debug window object.
    if (!m_DebugWindowLeft->Initialize(m_pDevice, VRCLIENTWIDTH, VRCLIENTHEIGHT, VRCLIENTWIDTH / 2, VRCLIENTHEIGHT))
    {
        MyDebugDlg(L"Could not initialize the debug window object.");
        return false;
    }

    m_DebugWindowRight.reset(new DebugWindow);
    if (!m_DebugWindowRight)
    {
        return false;
    }
    // Initialize the debug window object.
    if (!m_DebugWindowRight->Initialize(m_pDevice, VRCLIENTWIDTH, VRCLIENTHEIGHT, VRCLIENTWIDTH / 2, VRCLIENTHEIGHT))
    {
        MyDebugDlg(L"Could not initialize the debug window object.");
        return false;
    }


    // Create an orthographic projection matrix for 2D rendering.
    //D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);
    DirectX::XMMATRIX mo = DirectX::XMMatrixOrthographicLH((float)VRCLIENTWIDTH, (float)VRCLIENTHEIGHT, 0, 10);
    m_orthoMatrix.set((const float*)&mo.r);

    if ( NeedScreen())
        m_Screen->SetTexture(m_source_texture);

    m_fScale = 0.3f;
    m_fScaleSpacing = 4.0f;

    m_fNearClip = 0.1f;
    m_fFarClip = 30.0f;

    SetupCameras();

    if (!vr::VRCompositor())
    {
        DebugPrint("Compositor initialization failed. See log file for details\n");
        return false;
    }

    return true;

}

void BaseVRApp::ConfigureScreen()
{

}

void BaseVRApp::TurnZBufferOn()
{
    m_pImmediateContext->OMSetDepthStencilState(m_pDSState, 1);
}


void BaseVRApp::TurnZBufferOff()
{
    m_pImmediateContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
}



bool BaseVRApp::RenderScene(vr::Hmd_Eye nEye)
{
    bool result = false;
    D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix;


    projectionMatrix = GetCurrentViewProjectionMatrix(nEye);

    if (NeedScreen())
    {
        // Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
        m_Screen->Render(m_pImmediateContext);
        // Render the model using the color shader.
        result = m_ColorShader->Render(m_pImmediateContext, m_Screen->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Screen->GetTexture());
        if (!m_errorshown && !result)
        {
            m_errorshown = true;
            return false;
        }
    }

    return renderWorld(worldMatrix, viewMatrix, projectionMatrix, nEye == vr::Hmd_Eye::Eye_Left);
}

bool BaseVRApp::RenderToTexture()
{
    bool result;


    // Set the render target to be the render to texture.
    m_RenderTextureLeft->SetRenderTarget(m_pImmediateContext, m_pDepthStencilView);
    //Clear the render to texture background to blue so we can differentiate it from the rest of the normal scene.

    // Clear the render to texture.
    m_RenderTextureLeft->ClearRenderTarget(m_pImmediateContext, m_pDepthStencilView, m_background_colors[0], m_background_colors[1], m_background_colors[2], 1.0f);

    // Render the scene now and it will draw to the render to texture instead of the back buffer.
    result = RenderScene(vr::Hmd_Eye::Eye_Left);
    if (!result)
    {
        return false;
    }


    // Set the render target to be the render to texture.
    m_RenderTextureRight->SetRenderTarget(m_pImmediateContext, m_pDepthStencilView);
    //Clear the render to texture background to blue so we can differentiate it from the rest of the normal scene.

    // Clear the render to texture.
    m_RenderTextureRight->ClearRenderTarget(m_pImmediateContext, m_pDepthStencilView, m_background_colors[0], m_background_colors[1], m_background_colors[2], 1.0f);

    // Render the scene now and it will draw to the render to texture instead of the back buffer.
    result = RenderScene(vr::Hmd_Eye::Eye_Right);
    if (!result)
    {
        return false;
    }

    // Reset the render target back to the original back buffer and not the render to texture anymore.
    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

    return true;
}

uint64_t BaseVRApp::ButtonsFromState(const vr::VRControllerState_t &state)
{
    uint64_t buttons = 0;
    if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad))
    {
        if (state.rAxis[0].x > 0 && fabs(state.rAxis[0].x) > fabs(state.rAxis[0].y))
        {
            buttons |= vr::ButtonMaskFromId(vr::k_EButton_DPad_Right);
        }
        else if (state.rAxis[0].x < 0 && fabs(state.rAxis[0].x) > fabs(state.rAxis[0].y))
        {
            buttons |= vr::ButtonMaskFromId(vr::k_EButton_DPad_Left);
        }
        else if (state.rAxis[0].y > 0 && fabs(state.rAxis[0].x) < fabs(state.rAxis[0].y))
        {
            buttons |= vr::ButtonMaskFromId(vr::k_EButton_DPad_Up);
        }
        else if (state.rAxis[0].y < 0 && fabs(state.rAxis[0].x) < fabs(state.rAxis[0].y))
        {
            buttons |= vr::ButtonMaskFromId(vr::k_EButton_DPad_Down);
        }
    }

    if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger))
    {
        if (state.rAxis[1].x > 0)
        {
            buttons |= vr::ButtonMaskFromId(vr::k_EButton_A);
        }
    }

    if (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu))
    {
        buttons |= vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu);
    }

    return buttons;
}

void BaseVRApp::FindKeyboard()
{
    UINT numDevices = 0;
    GetRawInputDeviceList(nullptr, &numDevices, sizeof(RAWINPUTDEVICELIST));
    if (numDevices == 0)
        return;

    std::vector<RAWINPUTDEVICELIST> devices(numDevices);
    GetRawInputDeviceList(&devices[0], &numDevices, sizeof(RAWINPUTDEVICELIST));
    for (auto &device : devices)
    {
        if (device.dwType == RIM_TYPEKEYBOARD)
        {
            m_keyboard_handle = device.hDevice;
            return;
        }
    }
}

void BaseVRApp::SendKey(int action, int key)
{
    if (action == WM_KEYDOWN)
    {
        ::SendMessage(m_source_parent_window, WM_CHAR, key, 1);
    }
    else if (action == WM_KEYUP)
    {
        ::SendMessage(m_source_parent_window, WM_CHAR, key, (1UL << 31));
    }
}

void BaseVRApp::ProcessButton(const ControllerID , const vr::VRControllerState_t &)
{
}


// this is the function used to render a single frame
void BaseVRApp::render_frame(void)
{
    bool result = false;
    bool timeout = false;

    int controller_id = 0;
    for (vr::TrackedDeviceIndex_t unTrackedDevice = vr::k_unTrackedDeviceIndex_Hmd + 1; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; ++unTrackedDevice)
    {
        if (!m_pHMD->IsTrackedDeviceConnected(unTrackedDevice))
            continue;

        if (m_pHMD->GetTrackedDeviceClass(unTrackedDevice) != vr::TrackedDeviceClass_Controller)
            continue;

        vr::VRControllerState_t state;
        if (m_pHMD->GetControllerState(unTrackedDevice, &state))
        {
            ProcessButton(controller_id == 0 ? e_controller_0 : e_controller_1, state);
        }

        controller_id++;
    }

    HandleController();

    RECT pspRect;
    if (!GetTargetRect(pspRect))
        return;

    HRESULT hr = m_DesktopGrabber->GetFrame(&timeout, pspRect);
    if (timeout)
        return;
    if (FAILED(hr))
    {
        OutputError(hr, __FILE__, __LINE__);
    }
    // Render the entire scene to the texture first.
    result = RenderToTexture();
    if (!result)
    {
        return;
    }

    D3DXMATRIX orthoMatrix;

    TurnZBufferOff();

    result = m_DebugWindowLeft->Render(m_pImmediateContext, 0, 0);
    if (!result)
    {
        return;
    }

    orthoMatrix = m_orthoMatrix;

    Matrix4 identity;
    // Render the debug window using the texture shader.
    result = m_ColorShader->Render(m_pImmediateContext, m_DebugWindowLeft->GetIndexCount(), identity, identity,
        orthoMatrix, m_RenderTextureLeft->GetShaderResourceView());
    if (!result)
    {
        return;
    }


    result = m_DebugWindowRight->Render(m_pImmediateContext, 546, 0);
    if (!result)
    {
        return;
    }

    orthoMatrix = m_orthoMatrix;

    // Render the debug window using the texture shader.
    result = m_ColorShader->Render(m_pImmediateContext, m_DebugWindowRight->GetIndexCount(), identity, identity,
        orthoMatrix, m_RenderTextureRight->GetShaderResourceView());
    if (!result)
    {
        return;
    }

    TurnZBufferOn();


    m_pSwapChain->Present(0, 0);

    vr::Texture_t leftEyeTexture = { m_RenderTextureLeft->GetTexture(), vr::API_DirectX, vr::ColorSpace_Auto };
    vr::EVRCompositorError error1 = vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
    vr::Texture_t rightEyeTexture = { m_RenderTextureRight->GetTexture(), vr::API_DirectX, vr::ColorSpace_Auto };
    vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
    if (error1)
        DebugPrint("error is %d \n", error1);

    UpdateHMDMatrixPose();

}

Matrix4 BaseVRApp::GetHMDMatrixPoseEye(vr::Hmd_Eye nEye)
{
    if (!m_pHMD)
        return Matrix4();

    vr::HmdMatrix34_t matEyeRight = m_pHMD->GetEyeToHeadTransform(nEye);
    Matrix4 matrixObj(
        matEyeRight.m[0][0], matEyeRight.m[1][0], matEyeRight.m[2][0], 0.0,
        matEyeRight.m[0][1], matEyeRight.m[1][1], matEyeRight.m[2][1], 0.0,
        matEyeRight.m[0][2], matEyeRight.m[1][2], matEyeRight.m[2][2], 0.0,
        matEyeRight.m[0][3], matEyeRight.m[1][3], matEyeRight.m[2][3], 1.0f
    );

    return matrixObj.invert();
}

Matrix4 BaseVRApp::GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye)
{
    if (!m_pHMD)
        return Matrix4();

    vr::HmdMatrix44_t mat = m_pHMD->GetProjectionMatrix(nEye, m_fNearClip, m_fFarClip, vr::API_DirectX);

    return Matrix4(
        mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
        mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
        mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
        mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
    );
}

void BaseVRApp::SetupCameras()
{
    m_mat4ProjectionLeft = GetHMDMatrixProjectionEye(vr::Eye_Left);
    m_mat4ProjectionRight = GetHMDMatrixProjectionEye(vr::Eye_Right);
    m_mat4eyePosLeft = GetHMDMatrixPoseEye(vr::Eye_Left);
    m_mat4eyePosRight = GetHMDMatrixPoseEye(vr::Eye_Right);
}


Matrix4 BaseVRApp::GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye)
{
    Matrix4 matMVP;
    if (nEye == vr::Eye_Left)
    {
        matMVP = m_mat4ProjectionLeft * m_mat4eyePosLeft * m_mat4HMDPose;
    }
    else if (nEye == vr::Eye_Right)
    {
        matMVP = m_mat4ProjectionRight * m_mat4eyePosRight *  m_mat4HMDPose;
    }

    return matMVP;
}


Matrix4 BaseVRApp::ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose)
{
    Matrix4 matrixObj(
        matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
        matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
        matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
        matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
    );
    return matrixObj;
}

void BaseVRApp::UpdateHMDMatrixPose()
{
    if (!m_pHMD)
        return;

    vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose.data(), vr::k_unMaxTrackedDeviceCount, NULL, 0);

    m_iValidPoseCount = 0;
    m_strPoseClasses = "";
    for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
    {
        if (m_rTrackedDevicePose[nDevice].bPoseIsValid)
        {
            m_iValidPoseCount++;
            m_rmat4DevicePose[nDevice] = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
            if (m_rDevClassChar[nDevice] == 0)
            {
                switch (m_pHMD->GetTrackedDeviceClass(nDevice))
                {
                case vr::TrackedDeviceClass_Controller:        m_rDevClassChar[nDevice] = 'C'; break;
                case vr::TrackedDeviceClass_HMD:               m_rDevClassChar[nDevice] = 'H'; break;
                case vr::TrackedDeviceClass_Invalid:           m_rDevClassChar[nDevice] = 'I'; break;
                case vr::TrackedDeviceClass_Other:             m_rDevClassChar[nDevice] = 'O'; break;
                case vr::TrackedDeviceClass_TrackingReference: m_rDevClassChar[nDevice] = 'T'; break;
                default:                                       m_rDevClassChar[nDevice] = '?'; break;
                }
            }
            m_strPoseClasses += m_rDevClassChar[nDevice];
        }
    }

    if (m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
    {
        m_mat4HMDPose = m_rmat4DevicePose[vr::k_unTrackedDeviceIndex_Hmd].invert();
    }
    else
    {
        DebugPrint("pose not valid\n");
    }
}


// this is the function that cleans up D3D and VR
void BaseVRApp::clean(void)
{
    // Release the color shader object.
    if (m_ColorShader.get())
        m_ColorShader.release();

    // Release the model object.    
    if (m_Screen.get())
        m_Screen.release();

    // Release the camera object.
    if (m_CameraLeft.get())
        m_CameraLeft.release();

    if (m_pImmediateContext) m_pImmediateContext->ClearState();
    m_pDepthStencilView.Release();
    m_pDepthStencilView.Release();
    m_pRenderTargetView.Release();
    m_pSwapChain.Release();
    m_pImmediateContext.Release();
    m_pDevice.Release();

    if (m_DesktopGrabber.get())
        m_DesktopGrabber.release();

    if (m_RenderTextureLeft.get())
        m_RenderTextureLeft->Shutdown();
    if (m_RenderTextureRight.get())
        m_RenderTextureRight->Shutdown();
    if (m_DebugWindowLeft.get())
        m_DebugWindowLeft->Shutdown();
    if (m_DebugWindowRight.get())
        m_DebugWindowRight->Shutdown();

    if (m_pHMD)
    {
        vr::VR_Shutdown();
    }
}

bool BaseVRApp::NeedScreen()
{
    return true;
}