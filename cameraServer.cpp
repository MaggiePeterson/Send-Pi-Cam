#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "OpenVideo.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>

#define PORT 8888
#define BUFFER_SIZE 1*800*600
using namespace cv;
using namespace std;

void writeEdges(Mat img, vector<vector<Point>> &contours ){
    Mat gray, edge, draw;
    cvtColor(img, gray,  CV_BGR2GRAY);
    Canny( img, edge, 50, 150, 3);
    edge.convertTo(draw, CV_8U);
    vector<Vec4i> hierarchy;
    findContours( draw, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE ); //add contours data to vector contours
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
   
    vector< vector<Point> > contours;
    vector<Point> bag;
    int currdata =0;
    int currPos =0;
    int datalen =0;
    int len =0;
    int sub =0;
    
    OpenVideo myVideo(0);
    myVideo.setAutoExposure();
    cout << "Capture is opened" << endl;
    
    while(waitKey(10) != 'q')
    {
        
        
        writeEdges(myVideo.getImage(), contours); //saves image edges to vector contours
        //len = contours.size();
        
      //  send(new_socket, &len, sizeof(int),0);      //send length of contour
        
        for(int i=0; i< contours.size(); i++){
            datalen += contours[i].size() * sizeof(Point);
        }
        
        send(new_socket, &datalen, sizeof(int),0);
        
        
            for(const auto &line : contours){
                for (const auto &points : lines){
                    if(currdata + sizeof(Point) < 1000 - sizeof(Point)){
                        bag.push_back(points);
                        currdata += sizeof(Point);
                    }
                    else {
                        bag.push_back(points);
                        currdata += sizeof(Point);      //add that point
                        send(new_socket,&currdata, sizeof(int),0);
                         send(new_socket, contours.data(), currdata,0);      //send data
                         //currPos += currdata;
                         currdata= 0;
                         bag.clear();
                    }
                }
            }
        
  /*      for(int i=0; i<contours.size();i++){
            
            sub = contours[i].size()
            send(new_socket, &sub, sizeof(int),0);

        }*/
         //currPos =0;
                 
 
        
        
    }
    return 0;
    
}




