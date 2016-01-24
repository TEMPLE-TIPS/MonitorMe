/*
 * motion_detect.cpp
 * To accompany instructions at:
 *      http://sidekick.windforwings.com/2012/12/opencv-motion-detection-based-action.html
 *
 *  Created on: Dec 02, 2012
 *	Author: tan
 *
 */
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

IplImage		*img_read, *img_smooth, *img_color, *img_diff, *img_temp, *img_edge_color, *img_moving_avg;
IplImage		*img_work, *img_gray, *img_edge_gray, *img_contour;
CvMemStorage	*mem_store;
CvSize 			sz_of_img;
int 			depth_of_img, channels_of_img;
bool            initialized = false;

int 			param_display_stage = 7;
int 			param_moving_avg_wt = 1; // default = 2
int 			param_detect_threshold = 20;
int 			param_min_obj_size = 2;
int 			param_dilation_amt = 30;
int 			param_erosion_amt = 10;
int 			param_brightness_factor = 50;
int 			param_contrast_factor = 0;
int 			param_proc_delay = 1000;
const int 		MAX_PROC_DELAY = 1000;
const int 		MIN_PROC_DELAY = 100;


/*****************************************************
 * print what optimization libraries are available
 *****************************************************/
void print_lib_version() {
	const char* libraries;
	const char* modules;
	cvGetModuleInfo(NULL, &libraries, &modules);
	printf("Libraries: %s\nModules: %s\n", libraries, modules);
}

/*****************************************************
 * limit max size of image to be processed to 800x600
 *****************************************************/
void get_approp_size(IplImage *img, CvSize &img_size, int &img_depth, int &img_channels) {
	CvSize ori_size = cvGetSize(img);
	img_depth = img->depth;
	img_channels = img->nChannels;

	printf("Frame size: %d x %d\nDepth: %d\nChannels: %d\n", ori_size.width, ori_size.height, img_depth, img_channels);

	float div_frac_h = 600.0/((float)ori_size.height);
	float div_frac_w = 800.0/((float)ori_size.width);
	float div_frac = div_frac_w < div_frac_h ? div_frac_w : div_frac_h;
	if(div_frac > 1) div_frac = 1;

	img_size.height = ori_size.height * div_frac;
	img_size.width = ori_size.width * div_frac;
}
/*****************************************************
 * init and destroy globals
 *****************************************************/
void init(void) {
	img_work			= cvCreateImage(sz_of_img, depth_of_img, channels_of_img);
	img_color			= cvCreateImage(sz_of_img, depth_of_img, channels_of_img);
	img_diff			= cvCreateImage(sz_of_img, depth_of_img, channels_of_img);
	img_temp			= cvCreateImage(sz_of_img, depth_of_img, channels_of_img);
	img_edge_color		= cvCreateImage(sz_of_img, depth_of_img, channels_of_img);
	img_smooth			= cvCreateImage(sz_of_img, depth_of_img, channels_of_img);
	img_gray 			= cvCreateImage(sz_of_img, IPL_DEPTH_8U, 1);
	img_edge_gray		= cvCreateImage(sz_of_img, IPL_DEPTH_8U, 1);
	img_contour 		= cvCreateImage(sz_of_img, IPL_DEPTH_8U, 1);
	img_moving_avg 		= cvCreateImage(sz_of_img, IPL_DEPTH_32F, 3);
    
    initialized         = true;
	mem_store 			= cvCreateMemStorage(0);
}

void destroy(void) {
    initialized         = false; 
	cvReleaseMemStorage(&mem_store);
	cvReleaseImage(&img_work);
	cvReleaseImage(&img_smooth);
	cvReleaseImage(&img_temp);
	cvReleaseImage(&img_color);
	cvReleaseImage(&img_diff);
	cvReleaseImage(&img_gray);
	cvReleaseImage(&img_contour);
	cvReleaseImage(&img_moving_avg);
	cvReleaseImage(&img_edge_color);
	cvReleaseImage(&img_edge_gray);
}



/*****************************************************
 * display the frames in a window
 *****************************************************/
/**
void display_frame(bool motion_detected) {
	if(motion_detected) 							param_proc_delay = MIN_PROC_DELAY;
	else if(param_proc_delay < MAX_PROC_DELAY)		param_proc_delay += 10;

	if(1 == param_display_stage) cvShowImage(WINDOW_NAME, img_color);
}
**/
/*****************************************************
 * MonitorMe Extension
 * Expected:
 *  - Image is downsized to approriate resolution
 *****************************************************/
int monitorMe_Img(IplImage *img_inpt){
    
	get_approp_size(img_inpt, sz_of_img, depth_of_img, channels_of_img);
    if (initialized = false){
        init(); // Intialize working files
    }
    
    // Make sure input image is in correct size
    //
    //cvResize(img_inpt, img_work);
    resizeWindow(img_inpt, img_work);
    // smoothen the image
    cvSmooth(img_work, img_smooth, CV_BILATERAL, 5, 5, 30, 30);

    // increase contrast and adjust brightness
    cvAddWeighted(img_smooth, 1, img_smooth, 1, param_brightness_factor-50, img_color);

    // increase contrast further if specified
    for(int contrast_idx = 0; contrast_idx < param_contrast_factor; contrast_idx++) {
        cvAddWeighted(img_color, 1, img_color, 1, 0, img_color);
    }

    cvLaplace(img_color, img_edge_color, 3);
    cvCvtColor(img_edge_color, img_edge_gray, CV_RGB2GRAY);
    cvThreshold(img_edge_gray, img_edge_gray, 25+param_detect_threshold, 255, CV_THRESH_BINARY);
    cvCvtColor(img_edge_gray, img_edge_color, CV_GRAY2RGB);
    cvAdd(img_edge_color, img_color, img_color, NULL);

    if (img_moving_avg == NULL) {
        cvConvertScale(img_color, img_moving_avg, 1.0, 0.0);
    }
    else {
        cvRunningAvg(img_color, img_moving_avg, ((float)param_moving_avg_wt)/100.0, NULL);
    }

    cvConvertScale(img_moving_avg, img_temp, 1.0, 0);									// convert the moving avg to a format usable for diff
    cvAbsDiff(img_color, img_temp, img_diff);											// subtract current from moving average.
    cvCvtColor(img_diff, img_gray, CV_RGB2GRAY);										// convert image to gray
    cvThreshold(img_gray, img_gray, 25+param_detect_threshold, 255, CV_THRESH_BINARY);	// convert image to black and white

    // dilate and erode to reduce noise and join irregular blobs
    cvErode(img_gray, img_gray, 0, 2); 							// erode to remove noise
    cvDilate(img_gray, img_gray, 0, param_dilation_amt+2);		// dilate to join and fill blobs
    cvErode(img_gray, img_gray, 0, param_erosion_amt);			// erode again to get some of the original proportion back
    cvConvertScale(img_gray, img_contour, 1.0, 0.0);			// copy image to the contour image for contour detection

    // find the contours of the moving images in the frame.
    cvClearMemStorage(mem_store);
    CvSeq* contour = 0;
    cvFindContours(img_contour, mem_store, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    // process each moving contour in the current frame...
    bool motion_detected = false;
    for (; contour != 0; contour = contour->h_next) {
        CvRect bnd_rect = cvBoundingRect(contour, 0);	// get a bounding rect around the moving object.

        // discard objects smaller than our expected object size
        int obj_size_pct = bnd_rect.width * bnd_rect.height * 100 / (sz_of_img.height * sz_of_img.width);
        if (obj_size_pct < param_min_obj_size) continue;

        // either draw the contours or motion detection marker
        if(6 == param_display_stage) {
            cvDrawContours(img_work, contour, CV_RGB(0,255,0), CV_RGB(0,255,0), 3, CV_FILLED);
        }
        else if(7 == param_display_stage) {
            CvPoint center;
            center.x = bnd_rect.x + bnd_rect.width/2;
            center.y = bnd_rect.y + bnd_rect.height/2;
            int rad = (bnd_rect.width < bnd_rect.height ? bnd_rect.width : bnd_rect.height)/2;

            while (rad > 0) {
                cvCircle(img_work, center, rad, CV_RGB(153,204,50), 1, CV_AA);
                rad -= 8;
            }
        }
        motion_detected = true;
    }
    if(motion_detected) {
        destroy();
        return 1; // Return true there is motion
    }

	return 0;
}

