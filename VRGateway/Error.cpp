#include "stdafx.h"
#include "Error.h"

void OutputError(HRESULT hr, const char *file, int line)
{
    std::array<char,256> txt;
    sprintf_s(txt.data(), 256, "Error at %s (%d): %X", file, line, hr);
    OutputDebugStringA(txt.data());
}

void DebugPrint(const char *fmt, ...)
{
    va_list args;
    std::array<char,2048> buffer;

    va_start(args, fmt);
    vsprintf_s(buffer.data(),buffer.size(), fmt, args);
    va_end(args);


    OutputDebugStringA(buffer.data());
}

void MyDebugDlg(LPCWSTR msg)
{
    MessageBox(NULL, msg, _T(""), 0);
}
