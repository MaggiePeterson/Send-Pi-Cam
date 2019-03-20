#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "OpenVideo.hpp"
#include "OpenFilter.hpp"
#include "Metrics.hpp"
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
    
    Mat image(600,800,CV_8UC3);
    Mat edges(600,800,CV_8UC3);
    int datalen = 0,
    packetSize = 0,
    currPos =0,
    currPacket=0,
    radius1,
    radius2;
    Point2f center1, center2;
    Rect rect1, rect2;
    int angle;
    int dist;
    int key =  waitKey(100);
    
    Size imageSize;
    Filter brita;
    Metrics myMetrics(1280,69);
    const string filename = "HSV.txt";                      //need to get this to save
    const string filename2 = "Metrics.txt";  
    
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    OpenVideo myVideo(1);
    cout << "Capture is opened" << endl;
cout<<brita.readHSV(filename)<<endl;    
    if (!brita.readHSV(filename))            //if HSV file is not created
    {
        image = myVideo.getImage();
        imageSize = image.size();
        datalen = imageSize.width * imageSize.height * 3;
        packetSize = imageSize.width;
        
        
        while(currPos < datalen)            //send one image to config values on client side
        {
            if(currPos + packetSize > datalen)
                currPacket = datalen - currPos;
            else
                currPacket = packetSize;
            
            send(new_socket, image.data + currPos, currPacket, 0);
            currPos += currPacket;
        }
        
        read(new_socket, &brita.h_min, sizeof(int));    //get HSV values from trackbar on client side
        read(new_socket, &brita.h_max, sizeof(int));
        read(new_socket, &brita.s_min, sizeof(int));
        read(new_socket, &brita.s_max, sizeof(int));
        read(new_socket, &brita.v_min, sizeof(int));
        read(new_socket, &brita.v_max, sizeof(int));
        
        cout<<"H MIN  "<< brita.h_min <<endl;
        cout<<"H Max  "<< brita.h_max <<endl;
        cout<<"S MIN  "<< brita.s_min <<endl;
        cout<<"S MAX  "<< brita.s_max <<endl;
        cout<<"V MIN  "<< brita.v_min <<endl;
        cout<<"V MAX  "<< brita.v_max <<endl;
        
        brita.writeHSV(filename);
        
    }
    
    if(!myMetrics.readMetrics()){
        image = myVideo.getImage();
        edges = brita.edgeDetect(&image);
        imageSize = edge.size();
        datalen = imageSize.width * imageSize.height * 3;
        packetSize = imageSize.width;
        
        do{
            
            while(currPos < datalen)            //send one image to config values on client side
            {
                if(currPos + packetSize > datalen)
                    currPacket = datalen - currPos;
                else
                    currPacket = packetSize;
                
                send(new_socket, edges.data + currPos, currPacket, 0);
                currPos += currPacket;
            }
        
            read(new_socket, &key, sizeof(int),0);
            if(waitKey(30) == 'z')
                myMetrics.calibrateZero(&edges, dist);
            if (waitKey(30) == 99)
                myMetrics.configValues(&edges, dist);
            
        }while(key != 'q')
            
        myMetrics.writeMetrics(filename2);
        
    }
    
    while(waitKey(100) != 'q'){
	cout<<"senidng metrics"<<endl;        
        image = myVideo.getImage();
        edges = brita.edgeDetect(&image);
        
        myMetrics.drawBoundingBox(&edges);
        angle = myMetrics.angle();
        dist = myMetrics.distance();
        send(new_socket, &angle, sizeof(int),0);
        send(new_socket, &dist, sizeof(int),0); 
    }
    return 0;
    
}

