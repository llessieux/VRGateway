#pragma once
#include "BaseVRApp.h"
class PSPVRApp :
    public BaseVRApp
{
public:
    PSPVRApp();
    virtual ~PSPVRApp();

    virtual const wchar_t *GetSourceParentClassName() const override;
    virtual const wchar_t *GetSourceParentWindowName() const override;
    virtual const wchar_t *GetSourceClassName() const override;
    virtual const wchar_t *GetSourceWindowName() const override;

    void ReadPSPControlSettings(const char *pspControlsIniFileName);

    virtual void ProcessButton(const ControllerID device, const vr::VRControllerState_t &state) override;


protected:
    std::vector<std::string> m_keys;
    std::map<std::string, int> m_button_to_key_map;

};

