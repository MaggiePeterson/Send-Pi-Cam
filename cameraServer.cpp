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
    vector<Point> data;
    int contourlen = 0;
    int currdata = 0;
    int start = 0;
    int currPos;
    int len = 0;
    int k =0;
    int l =0;
    
    OpenVideo myVideo(0);
    myVideo.setAutoExposure();
    cout << "Capture is opened" << endl;
    
    while(waitKey(10) != 'q')
    {
        
        
        writeEdges(myVideo.getImage(), contours); //saves image edges to vector contours
        
        for(int i =0; i< contours.size(); i++){
            contourlen +=  sizeof(Point) * contours[i].size();    //data len is the size of the entire contour
        }
        
        while(currPos < contourlen ){                          //while not entirely through the contour...
                
                for(int i = k; i< contours.size(); i++){
                    for( int j =l; j< contours[i].size(); j++){         //make valule for this
                        
                        while (currdata - start <= 1000){
                            data.push_back(contours[i][j]);
                            currdata += sizeof(Point);
                        }
                        l++;
                        break;
                    }
                    k++;
                }
            
                currPos += currdata;
                len = currdata -  start;
                
            }
            
            send(new_socket,&currdata, sizeof(int),0 );
            send(new_socket, &data, len, 0);
            
            start = currdata;
            data.clear();            //erase first chunk of data sent
            
        }
    currPos = 0;
    currdata =0;
    k =0;
    l =0;
        

        
    }
    
    return 0;
}


