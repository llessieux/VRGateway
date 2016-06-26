#include "stdafx.h"
#include "Drum.h"
#include "DrumStick.h"
#include "Model.h"

Drum::Drum() :
    m_drum_max_radius(0),
    m_drum_inside_zone_radius(0),
    m_enable_logging(false)
{
}


Drum::~Drum()
{
}

bool Drum::Load(ID3D11Device *device, bool without_stand, const Matrix4 &drumMat)
{
    auto empty = [](const std::vector<Model::VertexType> &, const std::vector<unsigned long> &) {};


    auto SetupDrumHitRegion = [&](const std::vector<Model::VertexType> &vertices, const std::vector<unsigned long> &indices) {

        if (vertices.size() == 0 || indices.size() == 0)
            return;

        Vector3 barycenter(0, 0, 0);
        for (auto &v : vertices)
        {
            barycenter += v.position;
        }
        barycenter *= (1.0f / static_cast<float>(vertices.size()));
        float min_d = 10000000000.0f;
        Vector3 center;
        Vector3 centerNormal;
        size_t center_index = 0;
        for (auto &v : vertices)
        {
            float d = v.position.distance(barycenter);
            if (d < min_d)
            {
                min_d = d;
                center = v.position;
                centerNormal = v.normal;
                center_index = &v - &vertices[0];
            }
        }

        D3DXVECTOR3 centerNoZ(center.x, center.y, 0);
        float drum_max_radius = 0.0f;
        for (auto &v : vertices)
        {
            D3DXVECTOR3 pNoZ(v.position.x, v.position.y, 0);
            float d = pNoZ.distance(centerNoZ);
            if (d > drum_max_radius)
            {
                drum_max_radius = d;
            }
        }

        float drum_inside_zone_radius = 0;
        for (size_t i = 0; i < indices.size(); i += 3)
        {
            if ((indices[i] == center_index) ||
                (indices[i + 1] == center_index) ||
                (indices[i + 2] == center_index))
            {
                for (int j = 0; j < 3; j++)
                {
                    if (indices[i + j] == center_index)
                        continue;
                    float d = vertices[indices[i + j]].position.distance(center);
                    if (d > drum_inside_zone_radius)
                    {
                        drum_inside_zone_radius = d;
                    }
                }
            }
        }
        m_center = center;
        m_normal = centerNormal.normalize();
        m_drum_inside_zone_radius = drum_inside_zone_radius;
        m_drum_max_radius = drum_max_radius * 0.8f; //Too much stuff around
        DebugPrint("Center : %f %f %f\n", center.x, center.y, center.z);
        DebugPrint("Normal : %f %f %f\n", m_normal.x, m_normal.y, m_normal.z);
        DebugPrint("Inside Radius : %f\n", m_drum_inside_zone_radius);
        DebugPrint("Drum Radius : %f\n", m_drum_max_radius);
    };

    for (int i = 0; i < 17; i++)
    {
        if (without_stand)
        {
            if (i == 1 || i == 8 || i == 14)
                continue;
        }

        std::string txt("object");
        txt += std::to_string(i) + std::string(".obj");
        Model *obj = new Model;
        if (i == 2)
        {
            if (!obj->InitializeFromWavefrontFile(device, txt, drumMat, SetupDrumHitRegion))
                return false;
        }
        else
        {
            if (!obj->InitializeFromWavefrontFile(device, txt, drumMat, empty))
                return false;

        }
        m_objects.push_back(obj);
    }
    return true;
}

bool Drum::render(ID3D11DeviceContext *context, std::function<bool(int, ID3D11ShaderResourceView*)> render_callback)
{
    bool result = true;
    for (auto obj : m_objects)
    {
        obj->Render(context);
        result &= render_callback(obj->GetIndexCount(), obj->GetTexture());
    }
    return result;
}

Drum::StickHitRegion Drum::CheckStickHit(const DrumStick &stick, bool previous_hit)
{
    Vector3 tip_in_plane = stick.GetTipPosition() - m_center;
    Vector3 dx(1, 0, 0);
    Vector3 dy = m_normal.cross(dx).normalize();
    float depth = tip_in_plane.dot(m_normal);
    float lx = tip_in_plane.dot(dx);
    float ly = tip_in_plane.dot(dy);
    float radius = sqrt(lx*lx + ly*ly);; //2D Radius

    if (m_enable_logging)
    {
        DebugPrint("Center : %f %f %f\n", m_center.x, m_center.y, m_center.z);
        DebugPrint("Normal : %f %f %f\n", m_normal.x, m_normal.y, m_normal.z);
        DebugPrint("Controller Tip in plane : %f %f %f\n", tip_in_plane.x, tip_in_plane.y, tip_in_plane.z);
        DebugPrint("Depth : %f \n", depth);
        DebugPrint("Radius : %f \n", radius);
    }
    if (depth < -stick.GetLength())
        return StickHitRegion::e_none;

    float min_depth = previous_hit ? 0.1f : 0.0f;
    if (depth > min_depth)
        return StickHitRegion::e_none;

    if (radius < m_drum_inside_zone_radius && depth < min_depth)
    {
        if (tip_in_plane.x < 0)
        {
            DebugPrint("Hit Inside Left\n");
            return StickHitRegion::e_inside_left;
        }
        else
        {
            DebugPrint("Hit Inside Right\n");
            return StickHitRegion::e_inside_right;
        }
    }

    Vector3 contact;
    if (linePlaneIntersection(contact, stick.GetTipPosition(), stick.GetOriginPosition(), m_normal, m_center))
    {
        float r = contact.distance(m_center);
        if (r < (m_drum_max_radius + stick.GetRadius()) && r >(m_drum_inside_zone_radius - stick.GetRadius()))
        {
            if (tip_in_plane.x < 0)
            {
                DebugPrint("Hit Outside Left\n");
                return StickHitRegion::e_outside_left;
            }
            else
            {
                DebugPrint("Hit Outside Right\n");
                return StickHitRegion::e_outside_right;
            }
        }
    }

    return StickHitRegion::e_none;
}


bool Drum::linePlaneIntersection(Vector3 &contact, const Vector3 &pt1, const Vector3 &pt2, const Vector3 &plane_normal, const Vector3 &plane_coord) {

    // calculate plane
    Vector3 ray = pt1 - pt2;
    float d = plane_normal.dot(plane_coord);

    float ray_d = plane_normal.dot(ray);
    if (ray_d == 0.0f) {
        return false; // avoid divide by zero //ray parallel to plane normal
    }

    // Compute the t value for the directed line ray intersecting the plane
    float t = (d - plane_normal.dot(pt2)) / ray_d;

    // scale the ray by t
    Vector3 newRay = ray * t;

    // calc contact point
    contact = pt2 + newRay;

    if (t >= 0.0f && t <= 1.0f) {
        return true; // line intersects plane
    }
    return false; // line does not
}
