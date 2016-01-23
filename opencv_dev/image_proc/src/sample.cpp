/*
 *
 *  Created on: Jan 23, 2016
 *      Author: http://docs.opencv.org/2.4/doc/tutorials/...
 *      introduction/linux_eclipse/linux_eclipse.html...
 *      #linux-eclipse-usage
 */

#include <cv.h>
#include <highgui.h>

using namespace cv;

int main( int argc, char** argv )
{
  Mat image;
  image = imread( argv[1], 1 );

  if( argc != 2 || !image.data )
    {
      printf( "No image data \n" );
      return -1;
    }

  namedWindow( "Display Image", CV_WINDOW_AUTOSIZE );
  imshow( "Display Image", image );

  waitKey(0);

  return 0;
}



