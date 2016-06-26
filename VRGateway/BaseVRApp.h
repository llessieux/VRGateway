#pragma once

class DesktopGrabber;
class Camera;
class Model;
class Screen;
class ColorShader;
class RenderTexture;
class DebugWindow;
class Texture;

#define VRCLIENTWIDTH 640
#define VRCLIENTHEIGHT 360

class BaseVRApp
{
public:
    BaseVRApp();
    virtual ~BaseVRApp();

    bool init(HWND hWnd);

    void render_frame(void);

    virtual const wchar_t *GetSourceParentClassName() const = 0;
    virtual const wchar_t *GetSourceParentWindowName() const = 0;
    virtual const wchar_t *GetSourceClassName() const = 0;
    virtual const wchar_t *GetSourceWindowName() const = 0;

    void SetSourceParentHWnd(HWND hwnd);
    void SetSourceHWnd(HWND hwnd);
protected:
    enum ControllerID {
        e_controller_0,
        e_controller_1
    };

    virtual bool setupWorld() = 0;
    virtual void HandleController() = 0;
    virtual bool renderWorld(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, bool left) = 0;

    virtual void ProcessButton(const ControllerID device, const vr::VRControllerState_t &state);
    virtual bool NeedScreen(); 
    virtual void ConfigureScreen();

    typedef struct _FRAME_DATA
    {
        ID3D11Texture2D* Frame;
        DXGI_OUTDUPL_FRAME_INFO FrameInfo;
        _Field_size_bytes_((MoveCount * sizeof(DXGI_OUTDUPL_MOVE_RECT)) + (DirtyCount * sizeof(RECT))) BYTE* MetaData;
        UINT DirtyCount;
        UINT MoveCount;
    } FRAME_DATA;


    void clean();
    bool GetTargetRect(RECT &rect);
    void SetupCameras();
    void TurnZBufferOn();
    void TurnZBufferOff();
    bool RenderScene(vr::Hmd_Eye nEye);
    bool RenderToTexture();
    uint64_t ButtonsFromState(const vr::VRControllerState_t &state);
    void FindKeyboard();
    void SendKey(int action, int key);
    void UpdateHMDMatrixPose();
    Matrix4 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose);
    Matrix4 GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye);
    Matrix4 GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye);
    Matrix4 GetHMDMatrixPoseEye(vr::Hmd_Eye nEye);

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
    D3D_DRIVER_TYPE            m_driverType;
    D3D_FEATURE_LEVEL        m_featureLevel;
    D3D11_VIEWPORT            m_viewport;
    std::unique_ptr<Camera> m_CameraLeft;
    std::unique_ptr<Camera> m_CameraRight;

    std::unique_ptr<Screen> m_Screen;

    std::unique_ptr<ColorShader> m_ColorShader;
    std::unique_ptr<RenderTexture> m_RenderTextureLeft;
    std::unique_ptr<RenderTexture> m_RenderTextureRight;
    std::unique_ptr<DebugWindow> m_DebugWindowLeft;
    std::unique_ptr<DebugWindow>m_DebugWindowRight;
    
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
    std::vector<vr::TrackedDevicePose_t> m_rTrackedDevicePose;
    std::vector<Matrix4> m_rmat4DevicePose;

    int m_iTrackedControllerCount;
    int m_iTrackedControllerCount_Last;
    int m_iValidPoseCount;
    int m_iValidPoseCount_Last;

    std::array<uint64_t,2> m_prev_state;

    std::string m_strPoseClasses;                            // what classes we saw poses for this frame
    std::vector<char> m_rDevClassChar;   // for each device, a character representing its class

    HWND m_source_parent_window;
    HWND m_source_window;

    bool m_errorshown;
    HANDLE m_keyboard_handle;

    std::shared_ptr<Texture> m_source_texture;

    float m_background_colors[3];
};

