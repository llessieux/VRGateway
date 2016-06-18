#include "stdafx.h"
#include "VLCVRApp.h"
#include "ScreenClass.h"
#include "colorshaderclass.h"

VLCVRApp::VLCVRApp(bool stereo_input) : m_stereo_input(stereo_input)
{
    m_background_colors[0] = 0.0f;
    m_background_colors[1] = 0.0f;
    m_background_colors[2] = 0.0f;

}


VLCVRApp::~VLCVRApp()
{
}

void VLCVRApp::InternalClean()
{
}


void VLCVRApp::HandleController()
{
}

bool VLCVRApp::renderWorld(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, bool left)
{
    bool result = true;
    
    ModelClass *obj = (!left & m_stereo_input ) ? m_right_screen.get() : m_left_screen.get();
    if (obj == nullptr)
        return false;
    obj->Render(m_pImmediateContext);
    // Render the model using the color shader.
    result = m_ColorShader->Render(m_pImmediateContext, obj->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, obj->GetTexture());
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

    float sx = (float)rect_width / (float)rect_height;

    Vector3 screen_position(0, -0.1f, -6.0f);

    float input_scale = m_stereo_input ? 0.5f : 1.0f;

    m_left_screen.reset(new ScreenClass);

    m_left_screen->SetScreenSize(6.0f);
    m_left_screen->SetScreenPosition(screen_position);
    // Initialize the model object.
    bool result = m_left_screen->Initialize(m_pDevice, sx, input_scale,0.0f);
    if (!result)
    {
        ::MessageBox(0, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }
    m_left_screen->SetTexture(m_source_texture.get());

    if (m_stereo_input)
    {
        m_right_screen.reset(new ScreenClass);
        m_right_screen->SetScreenSize(6.0f);
        m_right_screen->SetScreenPosition(screen_position);
        // Initialize the model object.
        result = m_right_screen->Initialize(m_pDevice, sx, 0.5f, 0.5f);
        if (!result)
        {
            ::MessageBox(0, L"Could not initialize the model object.", L"Error", MB_OK);
            return false;
        }
        m_right_screen->SetTexture(m_source_texture.get());
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