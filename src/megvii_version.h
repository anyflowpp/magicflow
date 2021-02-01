#pragma once

#define _MEGVII_VERSION v1_0_0

#define _tostrMEGVII_VERSION(v) #v
#define tostrMEGVII_VERSION(v) _tostrMEGVII_VERSION(v)
#define MEGVII_VERSION tostrMEGVII_VERSION(_MEGVII_VERSION)

#define _MEGVII_DefVersion(vt, x) void vt##x
#define MEGVII_DefVersion(x) _MEGVII_DefVersion(megvii_version_func_,x)
extern "C" __declspec(dllexport) MEGVII_DefVersion(_MEGVII_VERSION)();

#define _MEGVII_commit_id(vt, x) void vt##x
#define MEGVII_commit_id(x) _MEGVII_commit_id(megvii_commit_id_,x)
extern "C" __declspec(dllexport) MEGVII_commit_id(HEAD_COMMIT_ID)();