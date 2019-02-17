#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <sys/time.h>
#include <time.h>
#include <iostream>
#include <string>
#include <vector>
#include "OpenVideo.hpp"

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>

#define PORT 8888
#define BUFFER_SIZE 1*800*600
using namespace cv;
using namespace std;

//TODO - SEND AS COUNTOURS
int returnEdges(Mat img, vector<vector<Point>> &cont ){
    Mat gray, edge, draw;
    cvtColor(img, gray,  CV_BGR2GRAY);
    Canny( img, edge, 50, 150, 3);
    //edge.convertTo(draw, CV_8U);
    vector<Vec4i> hierarchy;
    findContours( edge, cont, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );
    return cont.size();
}

int main()
{
    //tcp settings
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    const char *hello = "Hello from server";
    string text;
    
    //opencv data vars
    Mat currImg;          //output Image
    
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    bind(server_fd, (struct sockaddr *)&address,sizeof(address));
    
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    
    //timing variables to check performace
    clock_t startTime=clock(), currTime=clock();
    
    struct timeval currFrameTime,lastFrameTime;
    gettimeofday(&currFrameTime, NULL);
    int edgeSize = 0;
    
    vector<vector<Point>> contours;
    OpenVideo myVideo(0);
    myVideo.setAutoExposure();
    
    cout << "Capture is opened" << endl;
    while(waitKey(10) != 'q')
    {
        gettimeofday(&currFrameTime, NULL);
        //calculate timing data
        long int ms =(currFrameTime.tv_sec * 1000 + currFrameTime.tv_usec / 1000) - (lastFrameTime.tv_sec * 1000 + lastFrameTime.tv_usec / 1000) ;
        text =   "Proc time: "+ to_string((float)(currTime - startTime) /CLOCKS_PER_SEC) + " FPS: " + to_string((float)(1000.0/ ms));
        lastFrameTime = currFrameTime;
        
        //grab image from camera
        
        edgeSize = returnEdges(myVideo.getImage(), contours); //saves image edges to vector contours
        
        
        if(!contours.empty()){
            cout<<"ERROR: contours is empty"<<endl;
            break;
        }
        
        
        //log the cpu clock after alloc and draw
        
        
        //send size of vector
        //int edgeSize = contours.size();
        int sizeLen =  sizeof(int);
        // cout<<"Size: " + to_string(edgeSize)<<endl;
        
        if(!send(new_socket,&edgeSize,sizeLen,0 )){
            cout<<"ERROR: cannot send data"<<endl;
        }
        
        //int send(int fd, void *buffer, size_t n, int flags)
        
        /*
         string output;
         static int frameNumber =0;
         output =  to_string(imageSize.width) + to_string(imageSize.height) + ":" + to_string(imageSize.width * imageSize.height * 1) + "?";
         cout << "Frame Number: " << frameNumber++ << " " << text <<endl;
         int datalen = imageSize.width * imageSize.height * 1, currPos = 0;
         int packetSize = imageSize.width;
         int currPacket;
         startTime = clock();
         while(currPos < datalen)
         {
         if(currPos + packetSize > datalen)
         currPacket = datalen - currPos;
         else
         currPacket = packetSize;
         
         send(new_socket, currImg.data + currPos, currPacket, 0);
         currPos += currPacket;
         }
         currTime = clock();
         
         } */
        
        
    }
    
    return 0;
}


