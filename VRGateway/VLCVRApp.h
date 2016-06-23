#pragma once
#include "BaseVRApp.h"

class VLCVRApp :
    public BaseVRApp
{
public:
    VLCVRApp(bool stereo_input);
    virtual ~VLCVRApp();

protected:
    virtual void HandleController() override;
    virtual bool setupWorld() override;
    virtual bool renderWorld(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, bool left) override;
    virtual bool NeedScreen() override;
    virtual const wchar_t *GetSourceParentClassName() const override;
    virtual const wchar_t *GetSourceParentWindowName() const override;
    virtual const wchar_t *GetSourceClassName() const override;
    virtual const wchar_t *GetSourceWindowName() const override;
    virtual void ProcessButton(const ControllerID device, const vr::VRControllerState_t &state) override;


    void ButtonPressed(vr::EVRButtonId button);
    std::unique_ptr<Screen> m_left_screen;
    std::unique_ptr<Screen> m_right_screen;

    Matrix4 m_scale_matrix;
    Matrix4 m_position_matrix;
    bool m_stereo_input;
};

