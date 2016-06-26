#include "stdafx.h"
#include "PataponVRApp.h"
#include "Model.h"
#include "colorshader.h"
#include "Drum.h"
#include "DrumStick.h"
#include "Screen.h"

PataponVRApp::PataponVRApp() :
    m_stick{ -1, -1 },
    m_enable_logging(false)
{
    m_keys = std::vector<std::string>{
        "Left", "Up", "Right", "Down", "L", "Select",
        "Square", "Triangle", "Circle", "Cross", "R", "Start"
    };

}


PataponVRApp::~PataponVRApp()
{
}

void PataponVRApp::ConfigureScreen()
{
    m_Screen->SetScreenSize(2.0f);

    Vector3 screen_position(0, 0.3f, -2.0f);
    m_Screen->SetScreenPosition(screen_position);
}

void PataponVRApp::HandleController()
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
            if (controller == 0 || controller == 1)
            {
                const Matrix4 & mat = m_rmat4DevicePose[unTrackedDevice];
                Vector4 origin(0, 0, 0, 1);
                Vector4 world_pos = mat * origin;
                Vector3 zDir(0, 0, -1);
                Vector3 yDir(0, 1, 0);
                zDir = mat * zDir;
                yDir = mat * yDir;

                DrumStick &stick = controller == 0 ? m_left_stick : m_right_stick;

                Vector3 tip;
                tip = zDir * stick.GetLength() - yDir * stick.GetRadius();
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

                std::string keys[5] = { "Triangle", "Circle", "Square", "Cross" };

                Vector3 wp(world_pos.x, world_pos.y, world_pos.z);
                stick.SetTipPosition(ftip);
                stick.SetOriginPosition(wp);

                Drum::StickHitRegion hit;
                int drum_hit_index = -1;
                for (int i = 0; i < 4; i++)
                {
                    hit = m_drum[i]->CheckStickHit(stick, m_stick[i] != -1);
                    if (hit != Drum::StickHitRegion::e_none)
                    {
                        drum_hit_index = i;
                        break;
                    }
                }

                if ( hit == Drum::StickHitRegion::e_none && m_stick[controller] != -1)
                {
                    if (m_enable_logging)
                    {
                        DebugPrint("Sending Key Up: %s\n", keys[m_stick[controller]].c_str());
                    }
                    SendKey(WM_KEYUP, m_button_to_key_map[keys[m_stick[controller]]]);
                }
                else if (hit != Drum::StickHitRegion::e_none && m_stick[controller] == -1)
                {
                    if (m_enable_logging)
                    {
                        DebugPrint("Sending Key Down: %s\n", keys[drum_hit_index].c_str());
                    }
                    SendKey(WM_KEYDOWN, m_button_to_key_map[keys[drum_hit_index]]);
                    m_pHMD->TriggerHapticPulse(unTrackedDevice, 0, 5000);
                }
                m_stick[controller] = drum_hit_index;
            }
        }
        controller += 1;
    }
}


bool PataponVRApp::renderWorld(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, bool)
{

    for (int i = 0; i < 4; i++)
    {
        bool result = m_drum[i]->render(m_pImmediateContext, [&](int index_count, ID3D11ShaderResourceView*texture) {
            // Render the model using the color shader.
            return m_ColorShader->Render(m_pImmediateContext, index_count, worldMatrix, viewMatrix, projectionMatrix, texture);
        });
        if (!result)
            return false;
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
        bool result = m_ColorShader->Render(m_pImmediateContext, m_drum_stick->GetIndexCount(), mat, viewMatrix, projectionMatrix, m_drum_stick->GetTexture());
        if (!m_errorshown && !result)
        {
            m_errorshown = true;
            return false;
        }
    }
    return true;
}


bool PataponVRApp::setupWorld()
{
    for (int i = 0; i < 4; i++)
    {
        Matrix4 drumMat;
        float drum_offset = 3.0f;
        drumMat.translate(3.0f , 8.0f, -(drum_offset*1.5f) + i * drum_offset);
        drumMat.scale(0.1f);
        drumMat.rotateY(90.0f);

        m_drum[i].reset(new Drum());
        if (!m_drum[i]->Load(m_pDevice, true, drumMat))
            return false;
    }
    m_prev_state.at(0) = 0;
    m_prev_state.at(1) = 0;

    Matrix4 drumStickMat;
    drumStickMat.rotateX(-90.0f);
    drumStickMat.scale(0.02f);

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

        float stick_thickness = min(dz, min(dx, dy));
        float stick_radius = stick_thickness / 2.0f;
        float stick_length = max(dz, max(dx, dy));

        m_left_stick.Initialize(stick_length, stick_radius, stick_thickness);
        m_right_stick.Initialize(stick_length, stick_radius, stick_thickness);

        DebugPrint("Stick thickness: %f  Radius : %f length :%f\n", stick_thickness, stick_radius, stick_length);
    };


    m_drum_stick.reset(new Model);
    m_drum_stick->InitializeFromWavefrontFile(m_pDevice, "taiko_stick.obj", drumStickMat, StickSizeDetection);

    return true;
}
