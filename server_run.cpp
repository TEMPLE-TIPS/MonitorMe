# include "src/opencv_dev/motion_detect.h"
# include "src/server_capture/server_capture.h"

# include <stdio.h>
# include <stdlib.h>
# include <netdb.h>
# include <netinet/in.h>

# include <unistd.h>
# include <ctime>
# include <dirent.h>

# include <sys/types.h>
# include <sys/stat.h>
# include <sstream>
# include <iostream>
# include <fstream>

//#include <highgui.h>
# include "opencv2/highgui/highgui.hpp"
# include "opencv2/core/core.hpp"

using namespace cv;
using namespace std;

// Main server-side software runner for MonitorMe

// Import Libraries

// String Constatns
//
// Command to use to send an email
const string myDIR = "/home/devin-trejo/projects/MonitorMe/pics/"; // directory where the images will be stored
const string myEXT = ".jpg"; // extension of the images

int send_alert(){
    system("bash ./src/email/send_MonitorMe.sh \"dtrejod@gmail.com\"");
    return 1;
}

// Check if the directory exists, if not create it
// This function will create a new directory if the image is the first
// image taken for a specific day
void directoryExistsOrCreate(const char* pzPath)
{
    DIR *pDir;
    // directory doesn't exists -> create it
    if ( pzPath == NULL || (pDir = opendir (pzPath)) == NULL){
        printf("Creating directory %s\n", pzPath);
        mkdir(pzPath, 0777);
    }
    // if directory exists we opened it and we
    // have to close the directory again.
    else if(pDir != NULL)
        (void) closedir (pDir);
}

// When motion is detected we write the image to disk
//    - Check if the directory exists where the image will be stored.
//    - Build the directory and image names.
int incr = 0;
bool saveImg(Mat image, const string DIRECTORY, 
        const string EXTENSION, const char * DIR_FORMAT, 
        const char * FILE_FORMAT)
{
    stringstream ss;
    time_t seconds;
    struct tm * timeinfo;
    char TIME[80];
    time (&seconds);
    // Get the current time
    timeinfo = localtime (&seconds);
    
    // Create name for the date directory
    strftime (TIME,80,DIR_FORMAT,timeinfo);
    ss.str("");
    ss << DIRECTORY << TIME;
    directoryExistsOrCreate(ss.str().c_str());


    // Create name for the image
    strftime (TIME,80,FILE_FORMAT,timeinfo);
    ss.str("");

    if(incr < 100) incr++; // quick fix for when delay < 1s && > 10ms, (when delay <= 10ms, images are overwritten)
    else incr = 0; ss << DIRECTORY << TIME << EXTENSION;
    printf("Saving pic to file :'%s'\n", ss.str().c_str());
    //cout << "M = "<< endl << " "  << image << endl << endl;
    return imwrite(ss.str().c_str(), image);
}

int main(int argc, char* argv[]) {

    // Intialize Program
    //
    // MonitorMe Image Processing
    Mat prev_frame, current_frame, next_frame;
    
    // Create server_capture
    //
    temple_tips::server_capture android_client;
    
    // Get current date and time
    //
    string DIR_FORMAT = "%d%h%Y"; // 1Jan1970
    string FILE_FORMAT = DIR_FORMAT + "/" + "%d%h%Y_%H%M%S"; // 1Jan1970/1Jan1970_12153
    
    // Open socket to listen to
    //
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    unsigned char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int  n;

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
      perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 7356;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    /* Now start listening for the clients, here process will
    *   * go in sleep mode and will wait for the incoming connection
    *   */
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    /* Accept actual connection from the client */
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

    if (newsockfd < 0) {
        perror("ERROR on accept");
        exit(1);
    }
    
    /* If connection is established then start communicating */
    bzero(buffer,256);
    
    // Run program until no more data is bring sent
    //
    while((n=read(newsockfd, buffer, 255)) > 0)
    {
        // Get frames
        //
        buffer[n+1] = '\0';
        android_client.dump(buffer, n);
        if(android_client.get(next_frame) == false) continue;
        //saveImg(next_frame,myDIR,myEXT,DIR_FORMAT.c_str(),\
        //    FILE_FORMAT.c_str());
        //continue;
        // Get new images
        //
        if (current_frame.empty()){
            current_frame = next_frame.clone();
            continue;
        }
        prev_frame = current_frame.clone();
        current_frame = next_frame.clone();
        // Compare the last three frames and see if there is movement
        //
        if (monitorMe_findMotion(next_frame, current_frame, \
                    prev_frame) == true)
        {
            // First save the images
            //
            saveImg(current_frame,myDIR,myEXT,DIR_FORMAT.c_str(),\
                    FILE_FORMAT.c_str());
            //send_alert();
        }

    }
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    puts("Client disconnected");
    return 0;
}

