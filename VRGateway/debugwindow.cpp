////////////////////////////////////////////////////////////////////////////////
// Filename: debugwindow.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "debugwindow.h"

DebugWindow::DebugWindow():
    m_vertexBuffer(0),
    m_indexBuffer(0),
    m_screenWidth(0),
    m_screenHeight(0),
    m_bitmapWidth(0), 
    m_bitmapHeight(0),
    m_previousPosX(0), 
    m_previousPosY(0)
{

}

DebugWindow::~DebugWindow()
{
}


bool DebugWindow::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight)
{
    // Store the screen size.
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    // Store the size in pixels that this bitmap should be rendered at.
    m_bitmapWidth = bitmapWidth;
    m_bitmapHeight = bitmapHeight;

    // Initialize the previous rendering position to negative one.
    m_previousPosX = -1;
    m_previousPosY = -1;

    // Initialize the vertex and index buffers.
    bool result = InitializeBuffers(device);
    if(!result)
    {
        return false;
    }

    return true;
}


void DebugWindow::Shutdown()
{
    // Shutdown the vertex and index buffers.
    ShutdownBuffers();
}


bool DebugWindow::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
    bool result;


    // Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen.
    result = UpdateBuffers(deviceContext, positionX, positionY);
    if(!result)
    {
        return false;
    }

    // Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
    RenderBuffers(deviceContext);

    return true;
}


int DebugWindow::GetIndexCount()
{
    return m_indexCount;
}


bool DebugWindow::InitializeBuffers(ID3D11Device* device)
{
    // Set the number of vertices in the vertex array.
    m_vertexCount = 6;

    // Set the number of indices in the index array.
    m_indexCount = m_vertexCount;

    // Create the vertex array.
    std::vector<VertexType> vertices(m_vertexCount);
    if(vertices.empty())
    {
        return false;
    }

    // Create the index array.
    std::vector<unsigned long> indices(m_indexCount);
    if(indices.empty())
    {
        return false;
    }

    // Initialize vertex array to zeros at first.
    memset(vertices.data(), 0, (sizeof(VertexType) * m_vertexCount));

    // Load the index array with data.
    for(int i=0; i<m_indexCount; i++)
    {
        indices[i] = i;
    }

    // Set up the description of the static vertex buffer.
    D3D11_BUFFER_DESC vertexBufferDesc = {
        sizeof(VertexType) * m_vertexCount,
        D3D11_USAGE_DYNAMIC,
        D3D11_BIND_VERTEX_BUFFER,
        D3D11_CPU_ACCESS_WRITE,
        0,0
    };

    // Give the subresource structure a pointer to the vertex data.
    D3D11_SUBRESOURCE_DATA vertexData = { vertices.data() , 0 , 0 };

    // Now create the vertex buffer.
    HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    // Set up the description of the static index buffer.
    D3D11_BUFFER_DESC indexBufferDesc = {
        sizeof(unsigned long) * m_indexCount,
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_INDEX_BUFFER,
        0,0,0
    };

    D3D11_SUBRESOURCE_DATA indexData = { indices.data(), 0 , 0 };

    // Give the subresource structure a pointer to the index data.

    // Create the index buffer.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    return true;
}


void DebugWindow::ShutdownBuffers()
{
    // Release the index buffer.
    if(m_indexBuffer)
    {
        m_indexBuffer.Release();
    }

    // Release the vertex buffer.
    if(m_vertexBuffer)
    {
        m_vertexBuffer.Release();
    }
}


bool DebugWindow::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
    std::vector<VertexType> vertices;
    D3D11_MAPPED_SUBRESOURCE mappedResource = { nullptr, 0, 0 };


    // If the position we are rendering this bitmap to has not changed then don't update the vertex buffer since it
    // currently has the correct parameters.
    if((positionX == m_previousPosX) && (positionY == m_previousPosY))
    {
        return true;
    }
    
    // If it has changed then update the position it is being rendered to.
    m_previousPosX = positionX;
    m_previousPosY = positionY;

    // Calculate the screen coordinates of the left side of the bitmap.
    float left = static_cast<float>(-m_screenWidth / 2 + positionX);

    // Calculate the screen coordinates of the right side of the bitmap.
    float right = left + static_cast<float>(m_bitmapWidth);

    // Calculate the screen coordinates of the top of the bitmap.
    float top = static_cast<float>(m_screenHeight / 2 - positionY);

    // Calculate the screen coordinates of the bottom of the bitmap.
    float bottom = top - static_cast<float>(m_bitmapHeight);

    // Create the vertex array.
    vertices.resize(m_vertexCount);
    if(vertices.empty())
    {
        return false;
    }

    // Load the vertex array with data.
    // First triangle.
    vertices[0].position.set(left, top, 0.0f);  // Top left.
    vertices[0].texture.set(0.0f, 0.0f);

    vertices[1].position.set(right, bottom, 0.0f);  // Bottom right.
    vertices[1].texture.set(1.0f, 1.0f);

    vertices[2].position.set(left, bottom, 0.0f);  // Bottom left.
    vertices[2].texture.set(0.0f, 1.0f);

    // Second triangle.
    vertices[3].position.set(left, top, 0.0f);  // Top left.
    vertices[3].texture.set(0.0f, 0.0f);

    vertices[4].position.set(right, top, 0.0f);  // Top right.
    vertices[4].texture.set(1.0f, 0.0f);

    vertices[5].position.set(right, bottom, 0.0f);  // Bottom right.
    vertices[5].texture.set(1.0f, 1.0f);

    // Lock the vertex buffer so it can be written to.
    HRESULT result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result))
    {
        return false;
    }

    // Copy the data into the vertex buffer.
    memcpy(mappedResource.pData, vertices.data(), (sizeof(VertexType) * m_vertexCount));

    // Unlock the vertex buffer.
    deviceContext->Unmap(m_vertexBuffer, 0);

    return true;
}


void DebugWindow::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    // Set vertex buffer stride and offset.
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;
    
    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}