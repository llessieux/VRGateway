#pragma once

void OutputError(HRESULT hr, const char *file, int line);
void DebugPrint(const char *fmt, ...);
void MyDebugDlg(LPCWSTR msg);