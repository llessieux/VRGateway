#include "stdafx.h"
#include "Error.h"

void OutputError(HRESULT hr, const char *file, int line)
{
    char txt[256];
    sprintf_s(txt, 256, "Error at %s (%d): %X", file, line, hr);
    OutputDebugStringA(txt);
}

void dprintf(const char *fmt, ...)
{
    va_list args;
    char buffer[2048];

    va_start(args, fmt);
    vsprintf_s(buffer, fmt, args);
    va_end(args);


    OutputDebugStringA(buffer);
}

void MyDebug(LPCWSTR msg)
{
    MessageBox(NULL, msg, _T(""), 0);
}
