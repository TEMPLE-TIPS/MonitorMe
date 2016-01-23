#ifndef MOTION_DETECT_H
#define MOTION_DETECT_H
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <libconfig.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

// Header Function Declaration
//
void save_settings();
void load_settings();
void print_usage(int do_exit);
void print_lib_version();
void get_approp_size(CvCapture *input, CvSize &img_size, int &img_depth, int &img_channels);
CvCapture *capture_input(int argc, char **argv);
void init(bool is_silent_mode);
void destroy(bool is_silent_mode);
char display_frame(bool motion_detected);

int monitorMe_Img(IplImage* curFrame, CvArr* img_moving_avg = NULL){

#endif // MOTION_DETECT_H
