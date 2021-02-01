#include <iostream>
#include "kafface.h"
MGFErrorCode Detector::load(const char *model, int min_face) {
    this->options = MGFDetectorOptions {
        {0,0,0,0},
        (size_t)min_face,
        MGF_UP
    }; 
    MGFDetectorSettings settings { 
        MGFDeviceOption{ MGF_CPU, 0, 0},
        MGFShape2D {0, 0},
    };

    return MGF_make_detector_context_from_file(model, &settings, &ctx);
}

MGFErrorCode Detector::detect(MGFImage image) {
    return MGF_detect(ctx, image, &options, &result);
}

MGFErrorCode Tracker::load(const char *model) {
    MGFTrackerSettings settings;
    /*settings.type= MGF_SYNC_DETECTOR_TRACKER;

    MGFDeviceOption devopt{ MGF_CPU, 0, 0 };
    MGFTrackerDetectorOptions detopt{ MGFRect {0,0,0,0}, 80, MGF_UP, 0.9f };
    MGFSyncDetectorTracker dett{
            devopt,
            detopt,
            3,
            1,
            1,
            3,
    };
    settings.sync = dett;
    settings.sync.detector_options.min_face = 80;*/

    settings.type = MGF_SYNC_DETECTOR_TRACKER;
    settings.sync.tracker_device.dev_id = 0;
    settings.sync.tracker_device.dev_type = MGF_CPU;
    settings.sync.tracker_device.stream_id = 0;
    settings.sync.tracker_device.fast_run = false;
    settings.sync.grid_num_row = 3;
    settings.sync.grid_num_column = 1;
    settings.sync.detector_options.work_load = 0.9;
    settings.sync.detector_options.roi.left = 0;
    settings.sync.detector_options.roi.right= 0;
    settings.sync.detector_options.roi.top= 0;
    settings.sync.detector_options.roi.bottom= 0;
    settings.sync.detector_options.min_face = (size_t)80;
    settings.sync.detector_options.orient = MGF_UP;
    

    settings.sync.max_num_faces = 10;
    settings.sync.missing_tolerance = 3;
    settings.sync.manually_detect = 1;

    return MGF_make_tracker_context_from_file(model, &settings, &ctx);
}

MGFErrorCode Tracker::track(MGFImage image) {
    return MGF_track_frame(ctx, image, &result);
}

MGFErrorCode kagQuality::load(const char *model) {
    MGFQualitySettings settings {
        MGFDeviceOption {MGF_CPU, 0, 0},
    };

    return MGF_make_quality_context_from_file(model, &settings, &ctx);
}

MGFErrorCode kagQuality::predict(MGFImage image, const MGFLandmarkMap *landmark, int batch_size) {
    return MGF_predict_quality(ctx, &image, landmark, batch_size, &result);
}

MGFErrorCode Pose::load(const char *model) {
    MGFPoseSettings settings {
        MGFDeviceOption {MGF_CPU, 0, 0},
    };
    return MGF_make_pose_context_from_file(model, &settings, &ctx);
}

MGFErrorCode Pose::predict(MGFImage image, const MGFLandmarkMap *landmark, int batch_size) {
    return MGF_predict_pose(ctx, &image, landmark, batch_size, &result);
}

MGFErrorCode FMP::load(const char *model) {
    MGLFMPSettings settings{
        MGFDeviceOption {MGF_CPU, 0, 0},
    };
    return MGL_make_fmp_context_from_file(model, &settings, &ctx);
}

MGFErrorCode FMP::classify(MGFImage image, const MGFLandmarkMap *landmark, int batch_size) {
    return MGL_fmp_classify(ctx, &image, landmark, batch_size, &result);
}

MGFErrorCode Rgbir::load(const char *model) {
    MGLRGBIRSettings settings {
        MGFDeviceOption{MGF_CPU, 0, 0},
    };
    return MGL_make_rgb_ir_context_from_file(model, &settings, &ctx);
}

MGFErrorCode Rgbir::classify(MGFImage rgb, const MGFLandmarkMap *rgb_lmks, MGFImage ir, const MGFLandmarkMap *ir_lmks, int batch_size) {
    return MGL_rgb_ir_classify(ctx, &rgb, rgb_lmks, &ir, ir_lmks, batch_size, &result);
}

MGFErrorCode LandmarkAttr::load(const char *model) {
    MGFLandmarkAttrSettings settings {
        MGFDeviceOption{MGF_CPU, 0, 0},
    };
    return MGF_make_landmark_attr_context_from_file(model, &settings, &ctx);
}

MGFErrorCode LandmarkAttr::predict(MGFImage image, const MGFLandmarkMap *landmark, int batch_size) {
    return MGF_predict_landmark_attr(ctx, &image, landmark, batch_size, &result);
}

MGFErrorCode Feature::load(const char *model) {
    this->size = 0;
    MGFFeatureSettings settings{
        MGFDeviceOption{MGF_CPU, 0, 0},
    };
    auto megret = MGF_make_feature_context_from_file(model, &settings, &ctx);
    if(megret==MGF_OK){
        megret = MGF_get_feature_size(ctx, &this->size);
        if(megret!=MGF_OK){
            this->size = 0;
        }
    }
    return megret;
}

MGFErrorCode Feature::extract(MGFImage image, const MGFLandmarkMap *landmark, int batch_size) {
    return MGF_extract(ctx, &image, landmark, batch_size, &result);
}

MGFErrorCode Feature::compare(const MGFFeature *a, const MGFFeature *b) {
    return MGF_compare_features(ctx, a, b, &score_value);
}

MGFComparisonAlgorithm Feature::get_comparison_algorithm() {
    MGFComparisonAlgorithm ret;
	auto code = MGF_get_comparison_algorithm(ctx, &ret);
    if(code!=MGF_OK){
        throw std::bad_exception();
    }
    return ret;
} 

MGFErrorCode Feature::threshold() {
    return MGF_get_feature_score_thresholds(ctx, &threshold_value);
}

MGFErrorCode PoseBlur::load(const char *model) {
    MGFPoseBlurSettings settings {
        MGFDeviceOption{MGF_CPU, 0, 0},
    };
    return MGF_make_pose_blur_context_from_file(model, &settings, &ctx);
}

MGFErrorCode PoseBlur::predict(MGFImage image, const MGFScoreRect* facerect, int batch_size){
    return MGF_predict_pose_blur(ctx, &image, facerect, batch_size, &result);
}
