#pragma once
class DrumStick;
class Model;
interface ID3D11Device;

class Drum
{
public:
    Drum();
    virtual ~Drum();

    bool Load(ID3D11Device *device, bool without_stand, const Matrix4 &drumMat);

    enum StickHitRegion {
        e_none,
        e_inside_left,
        e_inside_right,
        e_outside_left,
        e_outside_right
    };

    StickHitRegion CheckStickHit(const DrumStick &stick, bool previous_hit);
    bool render(ID3D11DeviceContext *context, std::function<bool(int, ID3D11ShaderResourceView*)> render_callback);
private:
    bool linePlaneIntersection(Vector3 &contact, const Vector3 &pt1, const Vector3 &pt2, const Vector3 &plane_normal, const Vector3 &plane_coord);


    std::vector<Model *> m_objects;

    D3DXVECTOR3 m_center;
    D3DXVECTOR3 m_normal;
    float m_drum_max_radius;
    float m_drum_inside_zone_radius;
    bool m_enable_logging;
};

