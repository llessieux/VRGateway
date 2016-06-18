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
    virtual void InternalClean();
    virtual bool NeedScreen();
    virtual const wchar_t *GetSourceParentClassName() const;
    virtual const wchar_t *GetSourceParentWindowName() const;
    virtual const wchar_t *GetSourceClassName() const;
    virtual const wchar_t *GetSourceWindowName() const;

    std::unique_ptr<ScreenClass> m_left_screen;
    std::unique_ptr<ScreenClass> m_right_screen;

    bool m_stereo_input;
};

