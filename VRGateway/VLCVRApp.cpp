#include "stdafx.h"
#include "VLCVRApp.h"
#include "Screen.h"
#include "colorshader.h"
#include <array>

VLCVRApp::VLCVRApp(bool stereo_input) : m_stereo_input(stereo_input)
{
    m_background_colors[0] = 0.0f;
    m_background_colors[1] = 0.0f;
    m_background_colors[2] = 0.0f;
}


VLCVRApp::~VLCVRApp()
{
}

void VLCVRApp::ButtonPressed(vr::EVRButtonId button)
{
    switch (button)
    {
    case vr::k_EButton_DPad_Right:
        {
            Matrix4 m;
            m.scale(1.1f);
            m_scale_matrix *= m;
            break;
        }
    case vr::k_EButton_DPad_Left:
    {
        Matrix4 m;
        m.scale(1.0f/1.1f);
        m_scale_matrix *= m;
        break;
    }
    case vr::k_EButton_DPad_Up:
    {
        Matrix4 m;
        m.translate(Vector3(0,0.1f,0));
        m_position_matrix *= m;
        break;
    }
    case vr::k_EButton_DPad_Down:
    {
        Matrix4 m;
        m.translate(Vector3(0, -0.1f, 0));
        m_position_matrix *= m;
        break;
    }

    }
}

void VLCVRApp::ProcessButton(const ControllerID device, const vr::VRControllerState_t &state)
{
    std::array<vr::EVRButtonId,6>  buttons = { vr::k_EButton_DPad_Left, vr::k_EButton_DPad_Up , vr::k_EButton_DPad_Right, vr::k_EButton_DPad_Down, vr::k_EButton_A, vr::k_EButton_ApplicationMenu };

    uint64_t pressed_buttons = ButtonsFromState(state);
    for(auto button : buttons)
    {
        uint64_t mask = vr::ButtonMaskFromId(button);
        uint64_t current_button_state = pressed_buttons & mask;
        uint64_t previous_button_state = m_prev_state.at(device) & mask;
        if (current_button_state && !previous_button_state)
        {
            ButtonPressed(button);
        }
    }

    m_prev_state.at(device) = pressed_buttons;
}

void VLCVRApp::HandleController()
{
}

bool VLCVRApp::renderWorld(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, bool left)
{
    bool result = true;
    
    Model *obj = ((!left) & m_stereo_input ) ? m_right_screen.get() : m_left_screen.get();
    if (obj == nullptr)
        return false;
    obj->Render(m_pImmediateContext);
    // Render the model using the color shader.

    D3DXMATRIX world = worldMatrix * m_scale_matrix * m_position_matrix;
    result = m_ColorShader->Render(m_pImmediateContext, obj->GetIndexCount(), world, viewMatrix, projectionMatrix, obj->GetTexture());
    if (!result)
    {
        return false;
    }
    return true;
}


bool VLCVRApp::setupWorld()
{
    RECT src_rect;
    GetWindowRect(m_source_window, &src_rect);

    int rect_width = src_rect.right - src_rect.left + 1;
    int rect_height = src_rect.bottom - src_rect.top + 1;

    float sx = static_cast<float>(rect_width) / static_cast<float>(rect_height);

    Vector3 screen_position(0, -0.1f, -2.0f);

    float input_scale = m_stereo_input ? 0.5f : 1.0f;

    m_left_screen.reset(new Screen);

    m_left_screen->SetScreenSize(2.0f);
    m_left_screen->SetScreenPosition(screen_position);
    // Initialize the model object.
    bool result = m_left_screen->Initialize(m_pDevice, sx, input_scale,0.0f);
    if (!result)
    {
        MyDebugDlg(L"Could not initialize the model object.");
        return false;
    }
    m_left_screen->SetTexture(m_source_texture);

    if (m_stereo_input)
    {
        m_right_screen.reset(new Screen);
        m_right_screen->SetScreenSize(6.0f);
        m_right_screen->SetScreenPosition(screen_position);
        // Initialize the model object.
        result = m_right_screen->Initialize(m_pDevice, sx, 0.5f, 0.5f);
        if (!result)
        {
            MyDebugDlg(L"Could not initialize the model object." );
            return false;
        }
        m_right_screen->SetTexture(m_source_texture);
    }
    return true;
}

bool VLCVRApp::NeedScreen()
{
    return false;
}

const wchar_t *VLCVRApp::GetSourceParentClassName() const
{
    return L"QWidget";
}

const wchar_t *VLCVRApp::GetSourceParentWindowName() const
{
    return L"VLC media player";
}

const wchar_t *VLCVRApp::GetSourceClassName() const
{
    return L"VLC video output";
}
const wchar_t *VLCVRApp::GetSourceWindowName() const
{
    return nullptr;
}