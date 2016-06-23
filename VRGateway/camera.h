////////////////////////////////////////////////////////////////////////////////
// Filename: camera.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Vectors.h"
#include "Matrices.h"

//////////////
// INCLUDES //
//////////////
//#include <d3dx10math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class Camera
{
public:
    Camera();
    ~Camera();

    void SetPosition(float, float, float);
    void SetRotation(float, float, float);

    D3DXVECTOR3 GetPosition();
    D3DXVECTOR3 GetRotation();

    void Render();
    void GetViewMatrix(D3DXMATRIX&);

private:
    Camera(const Camera&);

    Vector3 m_position;
    Vector3 m_rotation;
    D3DXMATRIX m_viewMatrix;
};

#endif