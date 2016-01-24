/*
 * server_capture.cpp
 *
 *  Created on: Jan 23, 2016
 *      Author: Andrew Powell
 */


#include "server_capture.h"

namespace temple_tips {

	server_capture::server_capture() : buffer(new unsigned char[DATA_MAX_SIZE]), end_ptr(buffer),
			last_ptr(buffer+DATA_MAX_SIZE) {

	}

	server_capture::~server_capture() {
		delete[] buffer;
	}

	void server_capture::dump(unsigned char* data,int data_size) {

		// Align data according to the synch.
		if (buffer==end_ptr) {
			while (((int*)data)[CONTROL_INDEX_SYNCH]!=CONTROL_SYNCH
					&& data_size>0) {
				data++;
				data_size--;
			}
		}

		// Don't write any data is the data_size is too low.
		if (data_size<=0) {
			return;
		}

		// Write data if there's enough space.
		if ((end_ptr+data_size)<last_ptr) {
			memcpy(end_ptr,data,data_size);
			end_ptr += data_size;
		}
	}

	bool server_capture::get(cv::Mat& output) {
		int type = ((int*)buffer)[CONTROL_INDEX_TYPE];
		if (type==CONTROL_TYPE_MAT) {
			int width = ((int*)buffer)[CONTROL_INDEX_MAT_WIDTH];
			int height = ((int*)buffer)[CONTROL_INDEX_MAT_HEIGHT];
			int size = ((int*)buffer)[CONTROL_INDEX_MAT_SIZE];
			int last_byte_checksum = ((int*)buffer)[CONTROL_INDEX_MAT_CHECKSUM];
			unsigned char* raw_data = buffer+CONTROL_SIZE;
			if ((end_ptr<=(buffer+size))) {
				return false;
			}
			end_ptr = buffer;
			if (((unsigned char)last_byte_checksum!=raw_data[size-1])) {
				return false;
			}
			yuv_nv21_to_rgb(raw_data,width,height,output);
			return true;
		} else {
			return false;
		}
	}

	void server_capture::yuv_nv21_to_rgb(unsigned char* data,int width,int height,cv::Mat& rgb) {

		rgb = cv::Mat(height,width,CV_8UC3);
		int frameSize = width * height;

		int ii = 0;
		int ij = 0;
		int di = +1;
		int dj = +1;

		for (int i = 0, ci = ii; i < height; ++i, ci += di) {
			unsigned char* rgb_ptr = rgb.ptr<unsigned char>(i);
			for (int j = 0, cj = ij; j < width; ++j, cj += dj) {
				int y = (0xff & ((int) data[ci * width + cj]));
				int v = (0xff & ((int) data[frameSize + (ci >> 1) * width + (cj & ~1) + 0]));
				int u = (0xff & ((int) data[frameSize + (ci >> 1) * width + (cj & ~1) + 1]));
				y = y < 16 ? 16 : y;

				int r = (int) (1.164f * (y - 16) + 1.596f * (v - 128));
				int g = (int) (1.164f * (y - 16) - 0.813f * (v - 128) - 0.391f * (u - 128));
				int b = (int) (1.164f * (y - 16) + 2.018f * (u - 128));

				r = r < 0 ? 0 : (r > 255 ? 255 : r);
				g = g < 0 ? 0 : (g > 255 ? 255 : g);
				b = b < 0 ? 0 : (b > 255 ? 255 : b);

				//int ja = (j+10)%
				rgb_ptr[j*3+0] = b;
				rgb_ptr[j*3+1] = g;
				rgb_ptr[j*3+2] = r;
				//argb[a++] = 0xff000000 | (r << 16) | (g << 8) | b;
			}
		}

	}
};


