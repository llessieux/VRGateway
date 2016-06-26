#include "stdafx.h"
#include "TaikoVRApp.h"
#include "Model.h"
#include "colorshader.h"
#include "Drum.h"
#include "DrumStick.h"
#include "Screen.h"

//#define SHOW_STICK_END
#pragma optimize ("", off)

TaikoVRApp::TaikoVRApp() : 
    m_enable_logging(false), 
    m_stick{Drum::StickHitRegion::e_none,Drum::StickHitRegion::e_none }
{
    m_keys = std::vector<std::string>{
        "Left", "Up", "Right", "Down", "L", "An.Up",
        "Square", "Triangle", "Circle", "Cross", "R", "An.Left"
    };

}


TaikoVRApp::~TaikoVRApp()
{
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

            if (controller == 0 || controller == 1)
            {
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

                std::string keys[5] = { "", "Down", "Triangle", "L", "R" };

                Vector3 wp(world_pos.x, world_pos.y, world_pos.z);
                stick.SetTipPosition(ftip);
                stick.SetOriginPosition(wp);

                Drum::StickHitRegion hit = m_drum->CheckStickHit(stick, m_stick[controller] != Drum::StickHitRegion::e_none);
                if (hit == Drum::StickHitRegion::e_none && m_stick[controller] != Drum::StickHitRegion::e_none)
                {
                    if (m_enable_logging)
                    {
                        DebugPrint("Sending Key Up: %s\n", keys[m_stick[controller]].c_str());
                    }
                    SendKey(WM_KEYUP, m_button_to_key_map[keys[m_stick[controller]]]);
                }
                else if (hit != Drum::StickHitRegion::e_none && m_stick[controller] == Drum::StickHitRegion::e_none)
                {
                    if (m_enable_logging)
                    {
                        DebugPrint("Sending Key Down: %s\n", keys[hit].c_str());
                    }
                    SendKey(WM_KEYDOWN, m_button_to_key_map[keys[hit]]);
                    m_pHMD->TriggerHapticPulse(unTrackedDevice, 0, 5000);
                }
                m_stick[controller] = hit;
            }
        }
        controller += 1;
    }
}


bool TaikoVRApp::renderWorld(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, bool )
{
    
    bool result = m_drum->render(m_pImmediateContext,[&](int index_count, ID3D11ShaderResourceView*texture) {
        // Render the model using the color shader.
        return m_ColorShader->Render(m_pImmediateContext, index_count, worldMatrix, viewMatrix, projectionMatrix, texture);
    });

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

void TaikoVRApp::ConfigureScreen()
{
    m_Screen->SetScreenPosition(Vector3(0, -1.0f, -6.0f));
}

bool TaikoVRApp::setupWorld()
{
    // Create the model object.
    //m_Model = new Model;
    Matrix4 drumMat;
    drumMat.translate(0.8f / 0.4f, 0, -0.4f / 0.4f);
    drumMat.scale(0.4f);
    drumMat.rotateY(90.0f);

    m_drum.reset(new Drum());
    if (!m_drum->Load(m_pDevice,false, drumMat))
        return false;

    m_prev_state.at(0) = 0;
    m_prev_state.at(1) = 0;

    Matrix4 drumStickMat;
    drumStickMat.rotateX(-90.0f);
    drumStickMat.scale(0.03f);

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

#ifdef SHOW_STICK_END
    m_cube.reset(new Model());
    m_cube->InitializeBuffers(m_pDevice);
#endif
    return true;
}
