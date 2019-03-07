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
    
    int sizeC =0;
    int sizeD =24;
    vector< vector<Point> > contours;
    vector< vector< vector<Point> > >data(1000);
    
    OpenVideo myVideo(0);
    myVideo.setAutoExposure();
    cout << "Capture is opened" << endl;
    
    while(waitKey(10) != 'q')
    {
        
        writeEdges(myVideo.getImage(), contours); //saves image edges to vector contours
        
        for(int i =0; i< contours.size(); i++){
            sizeC += 24 + (sizeof(Point) * contours[i].size());    //size of contours == bit size of Point * size of
        }
        /*
         wrote edges
         size c size
         if size d size c...
            data.push countours
            data size += size
         
         
         */
        
        while(sizeD + sizeC <= 1000){
            //need to have it add the new contours
             data.push_back(contours);
             sizeD += sizeC + 24; //is this how it works?? do i need to add sizeof(vector)
            writeEdges(myVideo.getImage(), contours); //saves image edges to vector contours
            
            for(int i =0; i< contours.size(); i++){
                sizeC += 24 + (sizeof(Point) * contours[i].size());    //size of contours == bit size of Point * size of
            }

        }
        cout<<"size of data: "<<sizeD<<endl;
        
        if(!send(new_socket, &sizeD, sizeof(int),0)){            //send number of elements over
            cout<<"ERROR: cannot send size of array"<<endl;
        }
        
        if(!send(new_socket, &data, sizeD, 0)){
            cout<<"ERROR: cannot send data array"<<endl;
        }
        
        //cout<<data<<endl;
        
        data.erase(data.begin(), data.end());           //clear data for next contours
        sizeD =24;
        sizeC =0;
    
    }
    
    return 0;
}


