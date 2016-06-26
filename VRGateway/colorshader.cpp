////////////////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "colorshader.h"
#include "D3DCompiler.h"

#pragma comment (lib, "d3dcompiler.lib")

ColorShader::ColorShader()
{
    m_vertexShader = 0;
    m_pixelShader = 0;
    m_layout = 0;
    m_matrixBuffer = 0;

    m_sampleState = 0;
}

ColorShader::~ColorShader()
{
}


bool ColorShader::Initialize(ID3D11Device* device, HWND hwnd)
{
    bool result;


    // Initialize the vertex and pixel shaders.
    result = InitializeShader(device, hwnd, L"texture.vs", L"texture.ps");
    //result = InitializeShader(device, hwnd, L"shaders.shader", L"shaders.shader");
    if(!result)
    {
        return false;
    }

    return true;
}


void ColorShader::Shutdown()
{
    // Shutdown the vertex and pixel shaders as well as the related objects.
    ShutdownShader();
}


bool ColorShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
                              D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
    // Set the shader parameters that it will use for rendering.
    bool result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
    if(!result)
    {
        return false;
    }

    // Now render the prepared buffers with the shader.
    RenderShader(deviceContext, indexCount);

    return true;
}


bool ColorShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
    HRESULT result = E_FAIL;
    CComPtr<ID3D10Blob> errorMessage;
    CComPtr<ID3D10Blob> vertexShaderBuffer;
    CComPtr<ID3D10Blob> pixelShaderBuffer;
    std::vector<D3D11_INPUT_ELEMENT_DESC> polygonLayout(3);
    unsigned int numElements = 0;

    // Compile the vertex shader code.
    //result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
    //                               &vertexShaderBuffer, &errorMessage, NULL);
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, 
                                   &vertexShaderBuffer, &errorMessage);
    if(FAILED(result))
    {
        // If the shader failed to compile it should have writen something to the error message.
        if(errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        }
        // If there was  nothing in the error message then it simply could not find the shader file itself.
        else
        {
            MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // Compile the pixel shader code.
    result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, 
                                   &pixelShaderBuffer, &errorMessage);
    if(FAILED(result))
    {
        // If the shader failed to compile it should have writen something to the error message.
        if(errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        }
        // If there was nothing in the error message then it simply could not find the file itself.
        else
        {
            MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
    if(FAILED(result))
    {
        return false;
    }

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if(FAILED(result))
    {
        return false;
    }

    // Create the vertex input layout description.
    // This setup needs to match the VertexType stucture in the Model and in the shader.
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    polygonLayout[2].SemanticName = "NORMAL";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = 0;
    polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout.
    numElements = gsl::narrow_cast<UINT>(polygonLayout.size());

    //MessageBox(hwnd, L"Are you OK?", L"Are you OK?", MB_OK);
    // Create the vertex input layout.
    result = device->CreateInputLayout(polygonLayout.data(), numElements, vertexShaderBuffer->GetBufferPointer(), 
                                       vertexShaderBuffer->GetBufferSize(), &m_layout);
    if(FAILED(result))
    {
        return false;
    }
    //MessageBox(hwnd, L"Are you OK?", L"Are you OK?", MB_OK);

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    D3D11_BUFFER_DESC matrixBufferDesc = { sizeof(MatrixBufferType) , D3D11_USAGE_DYNAMIC , D3D11_BIND_CONSTANT_BUFFER , D3D11_CPU_ACCESS_WRITE, 0, 0};

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if(FAILED(result))
    {
        return false;
    }

    // Create a texture sampler state description.

    D3D11_SAMPLER_DESC samplerDesc = {
        D3D11_FILTER_MIN_MAG_MIP_LINEAR,
        D3D11_TEXTURE_ADDRESS_WRAP,D3D11_TEXTURE_ADDRESS_WRAP,D3D11_TEXTURE_ADDRESS_WRAP,
        0.0f, 1, D3D11_COMPARISON_ALWAYS, 0,0,0,0,
        0, D3D11_FLOAT32_MAX
    };

    // Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
    if (FAILED(result))
    {
        return false;
    }

    return true;
}


void ColorShader::ShutdownShader()
{
    // Release the sampler state.
    if (m_sampleState)
    {
        m_sampleState->Release();
        m_sampleState = 0;
    }

    // Release the matrix constant buffer.
    if(m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = 0;
    }

    // Release the layout.
    if(m_layout)
    {
        m_layout->Release();
        m_layout = 0;
    }

    // Release the pixel shader.
    if(m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = 0;
    }

    // Release the vertex shader.
    if(m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = 0;
    }
}

void ColorShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
    ofstream fout;

    // Get a pointer to the error message text buffer.
    gsl::span<char> msg((char*)(errorMessage->GetBufferPointer()), (size_t)errorMessage->GetBufferSize());

    // Open a file to write the error message to.
    fout.open("shader-error.txt");

    // Write out the error message.
    for (auto c : msg)
    {
        fout << c;
    }

    // Close the file.
    fout.close();

    // Pop a message up on the screen to notify the user to check the text file for compile errors.
    MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}


bool ColorShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
                                           D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    // Transpose the matrices to prepare them for the shader.
    worldMatrix.transpose();
    viewMatrix.transpose();
    projectionMatrix.transpose();

    // Lock the constant buffer so it can be written to.
    HRESULT result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result))
    {
        return false;
    }

    // Get a pointer to the data in the constant buffer.
    MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

    // Copy the matrices into the constant buffer.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer, 0);

    // Set the position of the constant buffer in the vertex shader.
    const unsigned int bufferNumber = 0;

    // Now set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    // Set shader texture resource in the pixel shader.
    deviceContext->PSSetShaderResources(0, 1, &texture);

    return true;
}


void ColorShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // Set the vertex input layout.
    deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // Set the sampler state in the pixel shader.
    deviceContext->PSSetSamplers(0, 1, &m_sampleState);

    // Render the triangle.
    deviceContext->DrawIndexed(indexCount, 0, 0);
}