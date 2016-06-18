////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "textureclass.h"
//#include <vector>
//#include "lodepng.h"
//#include <strstream>
#include <string>
#include "WICTextureLoader.h"

TextureClass::TextureClass()
{
	m_texture = 0;
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}


bool TextureClass::Initialize(ID3D11Device*device, const char*filename)
{
    std::string src(filename);
    std::wstring dst;
    for (auto c : src)
        dst.push_back(c);

    return Initialize(device, dst.c_str());
}

bool TextureClass::Initialize(ID3D11Device* device, const WCHAR* filename)
{
	HRESULT result;
	std::wstring wsFileName(filename);

	result = DirectX::CreateWICTextureFromFile(device, wsFileName.c_str(), nullptr, &m_texture);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}


void TextureClass::Shutdown()
{
	// Release the texture resource.
	if(m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;
}

void TextureClass::SetTexture(ID3D11ShaderResourceView* texture)
{
    Shutdown();
    m_texture = texture;
    m_texture->AddRef();

}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_texture;
}