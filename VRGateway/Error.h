#pragma once

void OutputError(HRESULT hr, const char *file, int line);
void dprintf(const char *fmt, ...);
void MyDebug(LPCWSTR msg);