#include "stdafx.h"
#include "TaikoVRApp.h"
#include "Model.h"
#include "colorshader.h"
//#define SHOW_STICK_END
#pragma optimize ("", off)

TaikoVRApp::DrumHitData::DrumHitData():
    m_drum_max_radius(0),
    m_drum_inside_zone_radius(0),
    m_stick_thickness(0),
    m_stick_length(0),
    m_stick_radius(0),
    m_stick{ TaikoVRApp::StickHitRegion::e_none, TaikoVRApp::StickHitRegion::e_none }
{
}

TaikoVRApp::TaikoVRApp() : m_enable_logging(false)
{
}


TaikoVRApp::~TaikoVRApp()
{
}

TaikoVRApp::StickHitRegion TaikoVRApp::CheckStickHit(const Vector3 &tip, const Vector3 &stick_origin)
{
    Vector3 tip_in_plane = tip - m_drumHitData.m_center;
    Vector3 dx(1, 0, 0);
    Vector3 dy = m_drumHitData.m_normal.cross(dx).normalize();
    float depth = tip_in_plane.dot(m_drumHitData.m_normal);
    float lx = tip_in_plane.dot(dx);
    float ly = tip_in_plane.dot(dy);
    float radius = sqrt(lx*lx + ly*ly);; //2D Radius

    if (m_enable_logging)
    {
        DebugPrint("Center : %f %f %f\n", m_drumHitData.m_center.x, m_drumHitData.m_center.y, m_drumHitData.m_center.z);
        DebugPrint("Normal : %f %f %f\n", m_drumHitData.m_normal.x, m_drumHitData.m_normal.y, m_drumHitData.m_normal.z);
        DebugPrint("Controller Tip in plane : %f %f %f\n", tip_in_plane.x, tip_in_plane.y, tip_in_plane.z);
        DebugPrint("Depth : %f \n", depth);
        DebugPrint("Radius : %f \n", radius);
    }
    if ( depth < -m_drumHitData.m_stick_length)
        return StickHitRegion::e_none;

    if (depth > 0)
        return StickHitRegion::e_none;

    if (radius < m_drumHitData.m_drum_inside_zone_radius && depth < 0)
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
    if (linePlaneIntersection(contact, tip, stick_origin, m_drumHitData.m_normal, m_drumHitData.m_center))
    {
        float r = contact.distance(m_drumHitData.m_center);
        if (r < (m_drumHitData.m_drum_max_radius + m_drumHitData.m_stick_radius) && r >(m_drumHitData.m_drum_inside_zone_radius - m_drumHitData.m_stick_radius))
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
            Vector4 origin(0, 0, 0, 1);
            Vector4 world_pos = mat * origin;
            Vector3 zDir(0, 0, -1);
            Vector3 yDir(0, 1, 0);
            zDir = mat * zDir;
            yDir = mat * yDir;

            Vector3 tip;
            tip = zDir * m_drumHitData.m_stick_length - yDir * m_drumHitData.m_stick_radius;
            Matrix4 tr;
            tr.translate(tip);
            Matrix4 tipM = tr * mat;

            Vector4 final_tip = tipM * origin;
            Vector3 ftip(final_tip.x, final_tip.y, final_tip.z);

            if (m_enable_logging)
            {
                DebugPrint("Controller %d origin : %f %f %f\n", controller, world_pos.x, world_pos.y, world_pos.z);
                DebugPrint("Controller %d tip : %f %f %f\n", controller, ftip.x, ftip.y, ftip.z);
            }

            std::string keys[5] = { "", "Down", "Triangle", "L", "R" };

            Vector3 wp(world_pos.x, world_pos.y, world_pos.z);
            StickHitRegion hit = CheckStickHit(ftip, wp);
            if (hit == StickHitRegion::e_none && m_drumHitData.m_stick[controller] != StickHitRegion::e_none)
            {
                if (m_enable_logging)
                {
                    DebugPrint("Sending Key Up: %s\n",keys[m_drumHitData.m_stick[controller]].c_str());
                }
                SendKey(WM_KEYUP, m_button_to_key_map[keys[m_drumHitData.m_stick[controller]]]);
            }
            else if (hit != StickHitRegion::e_none && m_drumHitData.m_stick[controller] == StickHitRegion::e_none)
            {
                if (m_enable_logging)
                {
                    DebugPrint("Sending Key Down: %s\n",keys[hit].c_str());
                }
                SendKey(WM_KEYDOWN, m_button_to_key_map[keys[hit]]);
                m_pHMD->TriggerHapticPulse(unTrackedDevice, 0, 5000);
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

bool TaikoVRApp::renderWorld(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, bool )
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
        }
    }

#ifdef SHOW_STICK_END
    Matrix4 trc;
    trc.translate(m_drumHitData.m_center);
    Matrix4 drumCenterM = trc * worldMatrix;

    m_cube->Render(m_pImmediateContext);
    result = m_ColorShader->Render(m_pImmediateContext, m_cube->GetIndexCount(), drumCenterM, viewMatrix, projectionMatrix, m_cube->GetTexture());
    if (!m_errorshown && !result)
    {
        m_errorshown = true;
        return false;
        //MyDebug(_T("render failed"));
    }

    {
        Vector3 centern;
        centern = m_drumHitData.m_center + m_drumHitData.m_normal;
        Matrix4 trn;
        trn.translate(centern);
        Matrix4 centerM = trn * worldMatrix;

        m_cube->Render(m_pImmediateContext);
        result = m_ColorShader->Render(m_pImmediateContext, m_cube->GetIndexCount(), centerM, viewMatrix, projectionMatrix, m_cube->GetTexture());
        if (!m_errorshown && !result)
        {
            m_errorshown = true;
            return false;
            //MyDebug(_T("render failed"));
        }
    }
#endif

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
        }

#ifdef SHOW_STICK_END
        Vector4 controller_origin(0, 0, 0, 1);
        Vector4 world_pos = mat * controller_origin;
        Vector3 zDir(0, 0, -1);
        Vector3 yDir(0, 1, 0);
        zDir = mat * zDir;
        yDir = mat * yDir;

        Vector3 tip;
        tip = zDir * m_drumHitData.m_stick_length - yDir * m_drumHitData.m_stick_radius;
        Matrix4 tr;
        tr.translate(tip);
        Matrix4 tipM = tr * mat;

        m_cube->Render(m_pImmediateContext);
        result = m_ColorShader->Render(m_pImmediateContext, m_cube->GetIndexCount(), tipM, viewMatrix, projectionMatrix, m_cube->GetTexture());
        if (!m_errorshown && !result)
        {
            m_errorshown = true;
            return false;
        }
#endif
    }
    return true;
}


bool TaikoVRApp::setupWorld()
{
    // Create the model object.
    //m_Model = new Model;

    Matrix4 drumMat;
    drumMat.translate(0.8f / 0.4f, 0, -0.4f / 0.4f);
    drumMat.scale(0.4f);
    drumMat.rotateY(90.0f);


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

        m_drumHitData.m_center = center;
        m_drumHitData.m_normal = centerNormal.normalize();
        m_drumHitData.m_drum_inside_zone_radius = drum_inside_zone_radius;
        m_drumHitData.m_drum_max_radius = drum_max_radius * 0.8f; //Too much stuff around
        DebugPrint("Center : %f %f %f\n", center.x, center.y, center.z);
        DebugPrint("Normal : %f %f %f\n", m_drumHitData.m_normal.x, m_drumHitData.m_normal.y, m_drumHitData.m_normal.z);
        DebugPrint("Inside Radius : %f\n", m_drumHitData.m_drum_inside_zone_radius);
        DebugPrint("Drum Radius : %f\n", m_drumHitData.m_drum_max_radius);
    };

    for (int i = 0; i < 17; i++)
    {
        std::string txt("object");
        txt += std::to_string(i) + std::string(".obj");
        Model *obj = new Model;
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

    m_prev_state.at(0) = 0;
    m_prev_state.at(1) = 0;

    Matrix4 drumStickMat;
    drumStickMat.rotateX(-90.0f);
    drumStickMat.scale(0.04f);

    auto StickSizeDetection = [&](const std::vector<Model::VertexType> &vertices, const std::vector<unsigned long> &indices) {
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

        DebugPrint("Stick thickness: %f  Radius : %f length :%f\n", m_drumHitData.m_stick_thickness, m_drumHitData.m_stick_radius, m_drumHitData.m_stick_length);
    };


    m_drum_stick.reset(new Model);
    m_drum_stick->InitializeFromWavefrontFile(m_pDevice, "taiko_stick.obj", drumStickMat, StickSizeDetection);
#ifdef SHOW_STICK_END
    m_cube.reset(new Model());
    m_cube->InitializeBuffers(m_pDevice);
#endif
    return true;
}

void TaikoVRApp::ProcessButton(const ControllerID device, const vr::VRControllerState_t &state)
{
    std::array<vr::EVRButtonId, 6> buttons = { vr::k_EButton_DPad_Left, vr::k_EButton_DPad_Up , vr::k_EButton_DPad_Right, vr::k_EButton_DPad_Down, vr::k_EButton_A, vr::k_EButton_ApplicationMenu };

    FindKeyboard();

    SetFocus(m_source_parent_window);

    uint64_t pressed_buttons = ButtonsFromState(state);
    for (int i = 0; i < 6; i++)
    {
        uint64_t mask = vr::ButtonMaskFromId(buttons[i]);
        uint64_t current_button_state = pressed_buttons & mask;
        uint64_t previous_button_state = m_prev_state.at(device) & mask;
        if (current_button_state && !previous_button_state)
        {
            SendKey(WM_KEYDOWN, m_button_to_key_map[m_keys[device * 6 + i]]);
        }
        if (!current_button_state && previous_button_state)
        {
            SendKey(WM_KEYUP, m_button_to_key_map[m_keys[device * 6 + i]]);
        }
    }

    m_prev_state.at(device) = pressed_buttons;
}
