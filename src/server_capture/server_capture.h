/*
 * server_capture.h
 *
 *  Created on: Jan 23, 2016
 *      Author: Andrew Powell
 */

#ifndef SERVER_CAPTURE_H_
#define SERVER_CAPTURE_H_

#include "opencv2/core/core.hpp"
#include <string.h>
#include <algorithm>

namespace temple_tips {

	class server_capture {
	public:
		const static int DATA_MAX_SIZE=(1<<24);
		const static int CONTROL_SIZE=24;
		server_capture();
		~server_capture();
		void dump(unsigned char* data,int data_size);
		bool get(cv::Mat& output);
	private:
		const static int CONTROL_SYNCH = 55;
		const static int CONTROL_INDEX_SYNCH = 0;
		const static int CONTROL_INDEX_TYPE = 1;
		const static int CONTROL_INDEX_MAT_WIDTH = 2;
		const static int CONTROL_INDEX_MAT_HEIGHT = 3;
		const static int CONTROL_INDEX_MAT_SIZE = 4;
		const static int CONTROL_INDEX_MAT_CHECKSUM = 5;
		const static int CONTROL_TYPE_MAT = 0;
		unsigned char* buffer;
		unsigned char* end_ptr;
		unsigned char* last_ptr;
		static void yuv_nv21_to_rgb(unsigned char* data,int width,int height,cv::Mat& rgb);
	};
};

#endif /* SERVER_CAPTURE_H_ */
