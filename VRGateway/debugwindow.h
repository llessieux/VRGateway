////////////////////////////////////////////////////////////////////////////////
// Filename: debugwindow.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _debugwindow_H_
#define _debugwindow_H_


//////////////
// INCLUDES //
//////////////
#include "Vectors.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: debugwindow
////////////////////////////////////////////////////////////////////////////////
class DebugWindow
{
private:
    struct VertexType
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
    };

public:
    DebugWindow();
    ~DebugWindow();

    bool Initialize(ID3D11Device*, int, int, int, int);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, int, int);

    int GetIndexCount();

private:
    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    bool UpdateBuffers(ID3D11DeviceContext*, int, int);
    void RenderBuffers(ID3D11DeviceContext*);

    DebugWindow(const DebugWindow&);

private:
    CComPtr<ID3D11Buffer> m_vertexBuffer;
    CComPtr<ID3D11Buffer> m_indexBuffer;
    int m_vertexCount, m_indexCount;
    int m_screenWidth, m_screenHeight;
    int m_bitmapWidth, m_bitmapHeight;
    int m_previousPosX, m_previousPosY;
};

#endif