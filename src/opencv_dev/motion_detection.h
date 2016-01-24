
#ifndef MOTION_DETECTION_H_
#define MOTION_DETECTION_H_

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace temple_tips {

	bool detect_motion(cv::Mat** hist,size_t size,unsigned char threshold,
			double detect_threshold);

};

#endif
