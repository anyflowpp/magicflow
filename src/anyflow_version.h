#pragma once

#define _anyflow_VERSION v1_0_0

#define _tostranyflow_VERSION(v) #v
#define tostranyflow_VERSION(v) _tostranyflow_VERSION(v)
#define anyflow_VERSION tostranyflow_VERSION(_anyflow_VERSION)

#define _anyflow_DefVersion(vt, x) void vt##x
#define anyflow_DefVersion(x) _anyflow_DefVersion(anyflow_version_func_,x)
extern "C" __declspec(dllexport) anyflow_DefVersion(_anyflow_VERSION)();

#define _anyflow_commit_id(vt, x) void vt##x
#define anyflow_commit_id(x) _anyflow_commit_id(anyflow_commit_id_,x)
extern "C" __declspec(dllexport) anyflow_commit_id(HEAD_COMMIT_ID)();