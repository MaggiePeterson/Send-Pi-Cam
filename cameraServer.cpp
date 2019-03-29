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
#include <sstream>

#define PORT 8888
#define BCAST_PORT 9999
#define BUFFER_SIZE 1*800*600
using namespace cv;
using namespace std;

int main()
{
    int angle;
    int dist;
    int key =  waitKey(100);
    Filter brita;
    Metrics myMetrics(1280,69);
    const string filename = "HSV.txt";                      //need to get this to save
    const string filename2 = "Metrics.txt";
    ostringstream oss;
    
    //set up UDP
    int sock;                         /* Socket */
    struct sockaddr_in broadcastAddr; /* Broadcast address */
    char *broadcastIP;                /* IP broadcast address */
    char *sendString;                 /* String to broadcast */
    int broadcastPermission = 1;          /* Socket opt to set permission to broadcast */
    unsigned int sendStringLen;       /* Length of string to broadcast */
    
    cout << "bcast thread start" << endl;
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        perror("UDP socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, sizeof(broadcastPermission)) < 0)
    {
        perror("Could not set UDP permissions");
        exit(EXIT_FAILURE);
    }
    /*    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &broadcastPermission, sizeof(broadcastPermission)) < 0)
     {
     perror("Could not reuse UDP port");
     exit(EXIT_FAILURE);
     }*/
    
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;/* Broadcast IP address */
    broadcastAddr.sin_port = htons(BCAST_PORT);         /* Broadcast port */
    cout << "Starting beacon" << endl;
    
    OpenVideo myVideo(0);
    cout << "Capture is opened" << endl;
    
    
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

while(waitKey(100) != 'q'){     //sends distance and angle
    cout<<"senidng metrics"<<endl;
    image = myVideo.getImage();
    edges = brita.edgeDetect(&image);
    
    myMetrics.drawBoundingBox(&edges);
    angle = myMetrics.angle();
    dist = myMetrics.distance();
    
    oss<< angle <<" "<<dist;    //save data to string
    
    sendto(sock, oss, sizeof(string), 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr));
    
    }
    return 0;
    
    }
    
