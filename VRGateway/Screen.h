////////////////////////////////////////////////////////////////////////////////
// Filename: Screen.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _Screen_H_
#define _Screen_H_

#include "Model.h"

class Screen : public Model
{

public:
    Screen();
    virtual ~Screen();

    bool Initialize(ID3D11Device*, float scale_x, float texture_scale=1.0f, float texture_offset=0.0f);

    void SetScreenSize(float size);
    void SetScreenPosition(Vector3 screen_position);
private:
    Screen(const Screen&);

    bool InitializeBuffers(ID3D11Device*);
    void AddScreenToScene(Matrix4 mat, std::vector<VertexType> &vertdata, std::vector<unsigned long> &indices);

    float m_screen_size;
    float m_texture_scale;
    float m_texture_offset;
    Vector3 m_screen_position;
};

#endif