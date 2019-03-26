#include "opencv2/core.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "OpenVideo.hpp"
#include "OpenFilter.hpp"
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
#include <thread>

#define PORT 9999
#define MAXLINE 1*800*600
using namespace cv;
using namespace std;


int main()
{
    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from server";
    struct sockaddr_in servaddr, cliaddr;
    
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    
    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
    
    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,
              sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    int len, n;
    n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                 MSG_WAITALL, ( struct sockaddr *) &cliaddr,
                 &len);
    buffer[n] = '\0';
    printf("Client : %s\n", buffer);
    sendto(sockfd, (const char *)hello, strlen(hello),
           MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
           len);
    printf("Hello message sent.\n");
    
    return 0;
} /*
    
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
    
    OpenVideo myVideo(0);
    cout << "Capture is opened" << endl;
    
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
cout<<"here"<<endl;
    bool readM = myMetrics.readMetrics(filename2);
	cout<<"sending readM"<<send(new_socket, &readM, sizeof(bool),0)<<endl;;
    if(!readM){
        image = myVideo.getImage();
        edges = brita.edgeDetect(&image);
        imageSize = edges.size();
        datalen = imageSize.width * imageSize.height * 3;
        packetSize = imageSize.width;
	cout<<"cannot read readm"<<endl;
        
        do{
           cout<<"in Metrics while loop"<<endl; 
            while(currPos < datalen)            //send one image to config values on client side
            {
                if(currPos + packetSize > datalen)
                    currPacket = datalen - currPos;
                else
                    currPacket = packetSize;
                
                send(new_socket, edges.data + currPos, currPacket, 0);
                currPos += currPacket;
            }
        
            read(new_socket, &key, sizeof(int));
            if(key == 'z')
                myMetrics.calibrateZero(&edges, dist);
            if (key == 99)
                myMetrics.configValues(&edges, dist);
            
        }while(key != 'q');
            
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
    
} */


