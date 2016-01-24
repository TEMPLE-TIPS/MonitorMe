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
// Header Varible Declaration
//

using namespace std;
using namespace cv;

extern IplImage		*img_read, *img_smooth, *img_color, *img_diff, *img_temp, *img_edge_color;
extern IplImage		*img_work, *img_gray, *img_edge_gray, *img_contour;
extern CvMemStorage	*mem_store;
extern CvSize			sz_of_img;
extern int 			depth_of_img, channels_of_img;
extern bool         initialized;
extern int 			param_display_stage;
extern int 			param_moving_avg_wt; // default = 2
extern int 			param_detect_threshold;
extern int 			param_min_obj_size;
extern int 			param_dilation_amt;
extern int 			param_erosion_amt;
extern int 			param_brightness_factor;
extern int 			param_contrast_factor;
extern int 			param_proc_delay;
extern const int 		    MAX_PROC_DELAY;
extern const int 		    MIN_PROC_DELAY;



// Header Function Declaration
//
void print_lib_version();
void get_approp_size(IplImage *img, CvSize &img_size, int &img_depth, int &img_channels); 
void init(void);
void destory(void);
//char display_frame(bool motion_detected);
int monitorMe_Img(IplImage *img_inpt, IplImage *img_moving_avg = NULL);

#endif // MOTION_DETECT_H
