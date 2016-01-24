/*
 * motion_detection.cpp
 *
 *  Created on: Jan 24, 2016
 *      Author: robertirwin
 */

#include "motion_detection.h"

namespace temple_tips {

	bool detect_motion(cv::Mat** hist,size_t size,unsigned char threshold,
			double detect_threshold) {

		cv::Mat abs_diff;
		cv::absdiff(*hist[0],*hist[1],abs_diff);
		cv::Mat avg = abs_diff.clone()/size;

		for (size_t each_mat=1;each_mat<size-1;each_mat++) {
			cv::absdiff(*hist[each_mat],*hist[each_mat+1],abs_diff);
			avg += abs_diff/size;
		}
		cv::cvtColor(avg,avg,CV_BGR2GRAY);
		cv::threshold(avg, avg, threshold, 255, cv::THRESH_BINARY);
		double nsum = cv::sum(avg)[0]/(abs_diff.cols*abs_diff.rows)/256;

		return nsum>=detect_threshold;
	}

};
