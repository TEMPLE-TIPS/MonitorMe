#ifndef MOTION_DETECT_H
#define MOTION_DETECT_H
#include <iostream>
#include <fstream>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <time.h>
#include <dirent.h>
#include <sstream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
using namespace cv;

extern const string myDIR; // directory where the images will be stored
extern const string myEXT; // extension of the images
extern const int DELAY; // in mseconds, take a picture every 1/2 second
extern const string LOGFILE;

// Header Function Declaration
//
inline int detectMotion(const Mat & motion, Mat & result, int x_start, int x_stop, int y_start, int y_stop, int max_deviation, Scalar & color);

int monitorMe_findMotion(Mat next_frame, Mat current_frame, Mat prev_frame);

#endif // MOTION_DETECT_H
