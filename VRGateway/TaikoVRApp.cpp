#include "stdafx.h"
#include "TaikoVRApp.h"
#include "modelclass.h"
#include "colorshaderclass.h"

TaikoVRApp::TaikoVRApp()
{
    for (int i = 0; i < 2; i++)
        m_drumHitData.m_stick[i] = StickHitRegion::e_none;
}


TaikoVRApp::~TaikoVRApp()
{
}

void TaikoVRApp::InternalClean()
{
}

TaikoVRApp::StickHitRegion TaikoVRApp::CheckStickHit(const Vector3 &world_pos, const Vector3 &zDir)
{
    Vector3 tip = world_pos - zDir * m_drumHitData.m_stick_length;
    Vector3 tip_in_plane = tip - m_drumHitData.m_center;
    float depth = tip_in_plane.dot(m_drumHitData.m_normal);
    float radius = tip_in_plane.length();

    if (radius < m_drumHitData.m_drum_inside_zone_radius && depth < 0)
    {
        if (tip_in_plane.x < 0)
        {
            OutputDebugStringA("Hit Inside Left");
            return StickHitRegion::e_inside_left;
        }
        else
        {
            OutputDebugStringA("Hit Inside Right");
            return StickHitRegion::e_inside_right;
        }
    }

    Vector3 contact;
    if (linePlaneIntersection(contact, tip, world_pos, m_drumHitData.m_normal, m_drumHitData.m_center))
    {
        float r = contact.distance(m_drumHitData.m_center);
        if (r < (m_drumHitData.m_drum_max_radius + m_drumHitData.m_stick_radius) && r >(m_drumHitData.m_drum_inside_zone_radius - m_drumHitData.m_stick_radius))
        {
            if (tip_in_plane.x < 0)
            {
                OutputDebugStringA("Hit Outside Left");
                return StickHitRegion::e_outside_left;
            }
            else
            {
                OutputDebugStringA("Hit Outside Right");
                return StickHitRegion::e_outside_right;
            }
        }
    }

    return StickHitRegion::e_none;
}

void TaikoVRApp::HandleController()
{
    int controller = 0;
    for (vr::TrackedDeviceIndex_t unTrackedDevice = vr::k_unTrackedDeviceIndex_Hmd + 1; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; ++unTrackedDevice)
    {
        if (!m_pHMD->IsTrackedDeviceConnected(unTrackedDevice))
            continue;

        if (m_pHMD->GetTrackedDeviceClass(unTrackedDevice) != vr::TrackedDeviceClass_Controller)
            continue;

        if (m_rTrackedDevicePose[unTrackedDevice].bPoseIsValid)
        {
            const Matrix4 & mat = m_rmat4DevicePose[unTrackedDevice];
            Vector4 controller_origin(0, 0, 0, 1);
            Vector4 world_pos = mat * controller_origin;
            Vector3 zDir(0, 0, 1);
            zDir = mat * zDir;

            std::string keys[5] = { "", "Down", "Triangle", "L", "R" };

            Vector3 wp(world_pos.x, world_pos.y, world_pos.z);
            StickHitRegion hit = CheckStickHit(wp, zDir);
            if (hit == StickHitRegion::e_none && m_drumHitData.m_stick[controller] != StickHitRegion::e_none)
            {
                SendKey(WM_KEYUP, m_button_to_key_map[keys[m_drumHitData.m_stick[controller]]]);
            }
            else if (hit != StickHitRegion::e_none && m_drumHitData.m_stick[controller] == StickHitRegion::e_none)
            {
                SendKey(WM_KEYDOWN, m_button_to_key_map[keys[hit]]);
                m_pHMD->TriggerHapticPulse(unTrackedDevice, 0, 50000);
            }
            m_drumHitData.m_stick[controller] = hit;
        }
        controller += 1;
    }
}


bool TaikoVRApp::linePlaneIntersection(Vector3 &contact, const Vector3 &pt1, const Vector3 &pt2, const Vector3 &plane_normal, const Vector3 &plane_coord) {

    // calculate plane
    Vector3 ray = pt1 - pt2;
    float d = plane_normal.dot(plane_coord);

    float ray_d = plane_normal.dot(ray);
    if (ray_d == 0.0f) {
        return false; // avoid divide by zero //ray parallel to plane normal
    }

    // Compute the t value for the directed line ray intersecting the plane
    float t = (d - plane_normal.dot(pt2)) / plane_normal.dot(ray);

    // scale the ray by t
    Vector3 newRay = ray * t;

    // calc contact point
    contact = pt2 + newRay;

    if (t >= 0.0f && t <= 1.0f) {
        return true; // line intersects plane
    }
    return false; // line does not
}

bool TaikoVRApp::renderWorld(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, bool left)
{
    bool result = true;
    for (auto obj : m_objects)
    {
        obj->Render(m_pImmediateContext);
        // Render the model using the color shader.
        result = m_ColorShader->Render(m_pImmediateContext, obj->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, obj->GetTexture());
        if (!m_errorshown && !result)
        {
            m_errorshown = true;
            return false;
            //MyDebug(_T("render failed"));
        }
    }

    for (vr::TrackedDeviceIndex_t unTrackedDevice = vr::k_unTrackedDeviceIndex_Hmd + 1; unTrackedDevice < vr::k_unMaxTrackedDeviceCount; ++unTrackedDevice)
    {
        if (!m_pHMD->IsTrackedDeviceConnected(unTrackedDevice))
            continue;

        if (m_pHMD->GetTrackedDeviceClass(unTrackedDevice) != vr::TrackedDeviceClass_Controller)
            continue;

        m_iTrackedControllerCount += 1;

        if (!m_rTrackedDevicePose[unTrackedDevice].bPoseIsValid)
            continue;

        const Matrix4 & mat = m_rmat4DevicePose[unTrackedDevice];

        m_drum_stick->Render(m_pImmediateContext);
        // Render the model using the color shader.
        result = m_ColorShader->Render(m_pImmediateContext, m_drum_stick->GetIndexCount(), mat, viewMatrix, projectionMatrix, m_drum_stick->GetTexture());
        if (!m_errorshown && !result)
        {
            m_errorshown = true;
            return false;
            //MyDebug(_T("render failed"));
        }
    }
    return true;
}


bool TaikoVRApp::setupWorld()
{
    // Create the model object.
    //m_Model = new ModelClass;

    char txt[256];
    Matrix4 drumMat;
    drumMat.translate(0.8f / 0.4f, 0, -0.4f / 0.4f);
    drumMat.scale(0.4f);
    drumMat.rotateY(90.0f);


    auto empty = [](const std::vector<ModelClass::VertexType> &vertices, const std::vector<unsigned long> &indices) {};


    auto SetupDrumHitRegion = [&](const std::vector<ModelClass::VertexType> &vertices, const std::vector<unsigned long> &indices) {

        if (vertices.size() == 0 || indices.size() == 0)
            return;

        size_t center_index = vertices.size() - 1; //The last vertex is the center one.
        D3DXVECTOR3 center = vertices[center_index].position;
        D3DXVECTOR3 centerNormal = vertices[center_index].normal;

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

        m_drumHitData.m_center = center;
        m_drumHitData.m_normal = centerNormal.normalize();
        m_drumHitData.m_drum_inside_zone_radius = drum_inside_zone_radius * 0.9f; //shrink it a bit
        m_drumHitData.m_drum_max_radius = drum_max_radius;
    };

    for (int i = 0; i < 17; i++)
    {
        sprintf_s(txt, 256, "object%d.obj", i);
        ModelClass *obj = new ModelClass;
        if (i == 2)
        {
            if (!obj->InitializeFromWavefrontFile(m_pDevice, txt, drumMat, SetupDrumHitRegion))
                return false;
        }
        else
        {
            if (!obj->InitializeFromWavefrontFile(m_pDevice, txt, drumMat, empty))
                return false;

        }
        m_objects.push_back(obj);
    }

    m_prev_state[0] = 0;
    m_prev_state[1] = 0;

    Matrix4 drumStickMat;
    drumStickMat.rotateX(-90.0f);
    drumStickMat.scale(0.08f);

    auto StickSizeDetection = [&](const std::vector<ModelClass::VertexType> &vertices, const std::vector<unsigned long> &indices) {
        D3DXVECTOR3 bboxMinPos, bboxMaxPos;
        if (vertices.size() == 0 || indices.size() == 0)
            return;
        bboxMinPos = vertices[0].position;
        bboxMaxPos = vertices[0].position;
        for (auto &v : vertices)
        {
            bboxMinPos.x = min(bboxMinPos.x, v.position.x);
            bboxMinPos.y = min(bboxMinPos.y, v.position.y);
            bboxMinPos.z = min(bboxMinPos.z, v.position.z);

            bboxMaxPos.x = max(bboxMaxPos.x, v.position.x);
            bboxMaxPos.y = max(bboxMaxPos.y, v.position.y);
            bboxMaxPos.z = max(bboxMaxPos.z, v.position.z);
        }
        float dx = bboxMaxPos.x - bboxMinPos.x;
        float dy = bboxMaxPos.y - bboxMinPos.y;
        float dz = bboxMaxPos.z - bboxMinPos.z;
        m_drumHitData.m_stick_thickness = min(dz, min(dx, dy));
        m_drumHitData.m_stick_radius = m_drumHitData.m_stick_thickness / 2.0f;
        m_drumHitData.m_stick_length = max(dz, max(dx, dy));
    };


    m_drum_stick.reset(new ModelClass);
    m_drum_stick->InitializeFromWavefrontFile(m_pDevice, "taiko_stick.obj", drumStickMat, StickSizeDetection);

    return true;
}