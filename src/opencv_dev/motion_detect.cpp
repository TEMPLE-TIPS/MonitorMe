//
//  Created by Cedric Verstraeten on 18/02/14.
//  Copyright (c) 2014 Cedric Verstraeten. All rights reserved.
//

#include <iostream>
#include <fstream>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <dirent.h>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
using namespace cv;

// Check if there is motion in the result matrix
// count the number of changes and return.
inline int detectMotion(const Mat & motion, Mat & result,
                 int x_start, int x_stop, int y_start, int y_stop,
                 int max_deviation,
                 Scalar & color)
{
    // calculate the standard deviation
    Scalar mean, stddev;
    meanStdDev(motion, mean, stddev);
    // if not to much changes then the motion is real (neglect agressive snow, 
    // temporary sunlight)
    if(stddev[0] < max_deviation)
    {
        int number_of_changes = 0;
        int min_x = motion.cols, max_x = 0;
        int min_y = motion.rows, max_y = 0;
        // loop over image and detect changes
        for(int j = y_start; j < y_stop; j+=2){ // height
            for(int i = x_start; i < x_stop; i+=2){ // width
                // check if at pixel (j,i) intensity is equal to 255
                // this means that the pixel is different in the sequence
                // of images (prev_frame, current_frame, next_frame)
                if(static_cast<int>(motion.at<uchar>(j,i)) == 255)
                {
                    number_of_changes++;
                    if(min_x>i) min_x = i;
                    if(max_x<i) max_x = i;
                    if(min_y>j) min_y = j;
                    if(max_y<j) max_y = j;
                }
            }
        }
        if(number_of_changes){
            //check if not out of bounds
            if(min_x-10 > 0) min_x -= 10;
            if(min_y-10 > 0) min_y -= 10;
            if(max_x+10 < result.cols-1) max_x += 10;
            if(max_y+10 < result.rows-1) max_y += 10;
            // draw rectangle round the changed pixel
            Point x(min_x,min_y);
            Point y(max_x,max_y);
            Rect rect(x,y);
            rectangle(result,rect,color,1);
        }
        return number_of_changes;
    }
    return 0;
}

int monitorMe_findMotion(Mat next_frame, Mat current_frame, Mat prev_frame)
{
    cvtColor(current_frame, current_frame, CV_RGB2GRAY);
    cvtColor(prev_frame, prev_frame, CV_RGB2GRAY);
    cvtColor(next_frame, next_frame, CV_RGB2GRAY);
    
    Mat result = next_frame.clone();

    // Motion Detection Settings
    //
    int number_of_changes, number_of_sequence = 0;
    
    // Get the curret Matrix Size
    int y_size = current_frame.rows;
    int x_size = current_frame.cols;
    
    int x_start = 10, x_stop = current_frame.cols-11;
    int y_start = 350, y_stop = 530;
    
    // If more than 'there_is_motion' pixels are changed, we say there is motion
    // and store an image on disk
    int there_is_motion = 5;
    
    // Maximum deviation of the image, the higher the value, the more motion is 
    // allowed
    int max_deviation = 20;
    
    // Erode kernel
    Mat kernel_ero = getStructuringElement(MORPH_RECT, Size(2,2));

    // d1 and d2 for calculating the differences
    // result, the result of and operation, calculated on d1 and d2
    // number_of_changes, the amount of changes in the result matrix.
    // color, the color for drawing the rectangle when something has changed.
    Mat d1, d2, motion;

    Scalar mean_, color(0,255,255); // yellow
    cvtColor(next_frame, next_frame, CV_RGB2GRAY);
    
    // Calc differences between the images and do AND-operation
    // threshold image, low differences are ignored (ex. contrast change due to 
    // sunlight)
    absdiff(prev_frame, next_frame, d1);
    absdiff(next_frame, current_frame, d2);
    bitwise_and(d1, d2, motion);
    threshold(motion, motion, 35, 255, CV_THRESH_BINARY);
    erode(motion, motion, kernel_ero);
    
    number_of_changes = detectMotion(motion, result, x_start, \
        x_stop, y_start, y_stop, max_deviation, color);
    
    // If a lot of changes happened, we assume something changed.
    if(number_of_changes>=there_is_motion)
    {
        if(number_of_sequence>0){ 
            return 1; // There is motion in the frame
        }
        number_of_sequence++;
    }
    else
    {
        number_of_sequence = 0;
    }
    return 0;    
}
