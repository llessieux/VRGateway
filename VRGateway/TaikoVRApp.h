#pragma once
#include "PSPVRApp.h"
#include "Drum.h"
#include "DrumStick.h"


class TaikoVRApp :
    public PSPVRApp
{
public:
    TaikoVRApp();
    virtual ~TaikoVRApp();

protected:
    virtual void HandleController() override;
    virtual bool setupWorld() override;
    virtual bool renderWorld(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, bool left) override;
    virtual void ConfigureScreen() override;

    std::unique_ptr<Drum> m_drum;
    std::unique_ptr<Model> m_cube;
    std::unique_ptr<Model> m_drum_stick;
    DrumStick m_left_stick;
    DrumStick m_right_stick;

    Drum::StickHitRegion m_stick[2];
    
    bool m_enable_logging;
};

