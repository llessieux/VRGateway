#pragma once
#include "BaseVRApp.h"

class VLCVRApp :
    public BaseVRApp
{
public:
    VLCVRApp(bool stereo_input);
    virtual ~VLCVRApp();

protected:
    virtual void HandleController();
    virtual bool setupWorld();
    virtual bool renderWorld(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, bool left);
    virtual bool NeedScreen();
    virtual const wchar_t *GetSourceParentClassName() const;
    virtual const wchar_t *GetSourceParentWindowName() const;
    virtual const wchar_t *GetSourceClassName() const;
    virtual const wchar_t *GetSourceWindowName() const;
    virtual void ProcessButton(int device, const vr::VRControllerState_t &state);


    void ButtonPressed(vr::EVRButtonId button);
    std::unique_ptr<Screen> m_left_screen;
    std::unique_ptr<Screen> m_right_screen;

    Matrix4 m_scale_matrix;
    Matrix4 m_position_matrix;
    bool m_stereo_input;
};

