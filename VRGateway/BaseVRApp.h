#pragma once

class DesktopGrabber;
class CameraClass;
class ModelClass;
class ScreenClass;
class ColorShaderClass;
class RenderTextureClass;
class DebugWindowClass;
class TextureClass;

#define VRCLIENTWIDTH 640
#define VRCLIENTHEIGHT 360

class BaseVRApp
{
public:
    BaseVRApp();
    virtual ~BaseVRApp();

    bool init(HWND hWnd);
    void ReadPSPControlSettings(const char *pspControlsIniFileName);

    void render_frame(void);

    virtual const wchar_t *GetSourceParentClassName() const;
    virtual const wchar_t *GetSourceParentWindowName() const;
    virtual const wchar_t *GetSourceClassName() const;
    virtual const wchar_t *GetSourceWindowName() const;

    void SetSourceParentHWnd(HWND hwnd);
    void SetSourceHWnd(HWND hwnd);
protected:

    virtual bool setupWorld() = 0;
    virtual void HandleController() = 0;
    virtual bool renderWorld(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, bool left) = 0;
    virtual void InternalClean() = 0;
    virtual bool NeedScreen(); 


    typedef struct _FRAME_DATA
    {
        ID3D11Texture2D* Frame;
        DXGI_OUTDUPL_FRAME_INFO FrameInfo;
        _Field_size_bytes_((MoveCount * sizeof(DXGI_OUTDUPL_MOVE_RECT)) + (DirtyCount * sizeof(RECT))) BYTE* MetaData;
        UINT DirtyCount;
        UINT MoveCount;
    } FRAME_DATA;

    void clean();
    bool GetPSPRect(RECT &rect);
    void SetupCameras();
    void TurnZBufferOn();
    void TurnZBufferOff();
    bool RenderScene(vr::Hmd_Eye nEye);
    bool RenderToTexture();
    uint64_t ButtonsFromState(const vr::VRControllerState_t &state);
    void FindKeyboard();
    void SendKey(int action, int key);
    void ProcessButton(int device, const vr::VRControllerState_t &state);
    void UpdateHMDMatrixPose();
    Matrix4 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose);
    Matrix4 GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye);
    Matrix4 GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye);
    Matrix4 GetHMDMatrixPoseEye(vr::Hmd_Eye nEye);
    std::string MatrixToString(const Matrix4& matrix);

    D3DXMATRIX m_projectionMatrix;

    D3DXMATRIX m_orthoMatrix;

    CComPtr<ID3D11Device>   m_pDevice;
    CComPtr<ID3D11DeviceContext>    m_pImmediateContext;
    CComPtr<ID3D11Texture2D>    m_pBackBufferTex;
    CComPtr<IDXGISwapChain> m_pSwapChain;

    std::unique_ptr<DesktopGrabber> m_DesktopGrabber;

    CComPtr<ID3D11RenderTargetView>     m_pRenderTargetView;
    CComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
    CComPtr<ID3D11DepthStencilState> m_pDSState;
    CComPtr<ID3D11DepthStencilState> m_depthDisabledStencilState;
    D3D_DRIVER_TYPE			m_driverType;
    D3D_FEATURE_LEVEL		m_featureLevel;
    D3D11_VIEWPORT			m_viewport;
    std::unique_ptr<CameraClass> m_CameraLeft;
    std::unique_ptr<CameraClass> m_CameraRight;

    std::unique_ptr<ScreenClass> m_Screen;

    std::unique_ptr<ColorShaderClass> m_ColorShader;
    std::unique_ptr<RenderTextureClass> m_RenderTextureLeft;
    std::unique_ptr<RenderTextureClass> m_RenderTextureRight;
    std::unique_ptr<DebugWindowClass> m_DebugWindowLeft;
    std::unique_ptr<DebugWindowClass>m_DebugWindowRight;
    
    uint32_t m_nRenderWidth;
    uint32_t m_nRenderHeight;

    float m_fNearClip;
    float m_fFarClip;
    float m_fScaleSpacing;
    float m_fScale;

    Matrix4 m_mat4HMDPose;
    Matrix4 m_mat4eyePosLeft;
    Matrix4 m_mat4eyePosRight;

    Matrix4 m_mat4ProjectionCenter;
    Matrix4 m_mat4ProjectionLeft;
    Matrix4 m_mat4ProjectionRight;

    vr::IVRSystem *m_pHMD;
    vr::IVRRenderModels *m_pRenderModels;
    vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
    Matrix4 m_rmat4DevicePose[vr::k_unMaxTrackedDeviceCount];

    int m_iTrackedControllerCount;
    int m_iTrackedControllerCount_Last;
    int m_iValidPoseCount;
    int m_iValidPoseCount_Last;

    uint64_t m_prev_state[2];

    std::string m_strPoseClasses;                            // what classes we saw poses for this frame
    char m_rDevClassChar[vr::k_unMaxTrackedDeviceCount];   // for each device, a character representing its class

    HWND m_source_parent_window;
    HWND m_source_window;

    bool m_errorshown;
    HANDLE m_keyboard_handle;

    std::unique_ptr<TextureClass> m_source_texture;
    std::vector<std::string> m_keys;
    std::map<std::string, int> m_button_to_key_map;

    float m_background_colors[3];
};
