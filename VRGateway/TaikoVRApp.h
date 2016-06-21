#pragma once
#include "BaseVRApp.h"
class TaikoVRApp :
    public BaseVRApp
{
public:
    TaikoVRApp();
    virtual ~TaikoVRApp();

protected:
    enum StickHitRegion {
        e_none,
        e_inside_left,
        e_inside_right,
        e_outside_left,
        e_outside_right
    };

    class DrumHitData
    {
    public:
        D3DXVECTOR3 m_center;
        D3DXVECTOR3 m_normal;
        float m_drum_max_radius;
        float m_drum_inside_zone_radius;

        float m_stick_thickness;
        float m_stick_length;
        float m_stick_radius;

        StickHitRegion m_stick[2];
    };
    virtual void HandleController();
    virtual bool setupWorld();
    virtual bool renderWorld(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, bool left);
    virtual void ProcessButton(int device, const vr::VRControllerState_t &state);

    StickHitRegion CheckStickHit(const Vector3 &tip_position, const Vector3 &stick_origin);
    bool linePlaneIntersection(Vector3 &contact, const Vector3 &pt1, const Vector3 &pt2, const Vector3 &plane_normal, const Vector3 &plane_coord);

    std::unique_ptr<Model> m_drum_stick;
    std::vector<Model *> m_objects;

    std::unique_ptr<Model> m_cube;
    DrumHitData m_drumHitData;

    bool m_enable_logging;
};

