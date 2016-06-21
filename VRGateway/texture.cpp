////////////////////////////////////////////////////////////////////////////////
// Filename: Texture.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Texture.h"
//#include <vector>
//#include "lodepng.h"
//#include <strstream>
#include <string>
#include "WICTextureLoader.h"

Texture::Texture()
{
	m_texture = 0;
}


Texture::Texture(const Texture& other)
{
}


Texture::~Texture()
{
}


bool Texture::Initialize(ID3D11Device*device, const char*filename)
{
    std::string src(filename);
    std::wstring dst;
    for (auto c : src)
        dst.push_back(c);

    return Initialize(device, dst.c_str());
}

bool Texture::Initialize(ID3D11Device* device, const WCHAR* filename)
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


void Texture::Shutdown()
{
	// Release the texture resource.
	if(m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;
}

void Texture::SetTexture(ID3D11ShaderResourceView* texture)
{
    Shutdown();
    m_texture = texture;
    m_texture->AddRef();

}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_texture;
}