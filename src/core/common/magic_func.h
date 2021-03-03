#pragma once
#ifdef MAGIC_EXPORT_API_DLL
#define MAGIC_E_FUNC __declspec(dllexport)
#else
#define MAGIC_E_FUNC 
#endif