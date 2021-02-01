#pragma once
#ifndef __KAG_KAFUTILS_H__
#define __KAG_KAFUTILS_H__

#include <memory>

#ifndef MGF_ENABLE_KAG_LIMIT
#define MGF_ENABLE_KAG_LIMIT 0
#endif

#ifdef KAG_DLL
#define KAGAPI __declspec(dllexport)
#else
#define KAGAPI
#endif

#ifndef LOGF
#define LOGF(format, ...) printf(format"\n", ##__VA_ARGS__)
#endif

#if __cplusplus == 201103L // c++ 11
namespace std {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}
#endif 
class Detector;
class Feature;
class Tracker;
class kagQuality;
class Pose;
class Rgbir;
class LandmarkAttr;
class PoseBlur;
class FMP;

struct MGFFeature;
struct MGFLandmarkMap;
struct MGFRect;

#define M_PI   3.14159265358979323846
#define RADIX 57.29577951308232087685

#define KAGLOADMODLE(c,t,m) {\
		if (!c) {\
			c = std::make_unique<t>();\
			MGFErrorCode ret = c->load(m);\
			if (ret != MGF_OK) {\
				return -1;\
			}\
		}\
	}

#define KAGLOADMODLE_1(c,t,m,a1) {\
		if (!c) {\
			c = std::make_unique<t>();\
			MGFErrorCode ret = c->load(m,a1);\
			if (ret != MGF_OK) {\
				return -1;\
			}\
		}\
	}

#endif //__KAG_KAFUTILS_H__
