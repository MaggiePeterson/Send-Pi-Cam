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
    
    Point pt(2,200);
    Point pt2(120,400);
    Point pt3(40,500);
    Point pt4(200,70);
    Point pt5(3,70);
    Point pt6(56,340);
    
    
    vector< vector<Point> > contours = { {pt,pt2,pt3}, {pt3,pt4}, {pt5,pt6} };

    int len = 3;
    int sub_vector;
    int size =0;
    
    
//    OpenVideo myVideo(0);
//    myVideo.setAutoExposure();
    cout << "Capture is opened" << endl;
    
    while(waitKey(10) != 'q')
    {
        
        
        //writeEdges(myVideo.getImage(), contours); //saves image edges to vector contours
        uint32_t number_vectors = contours.size();
        send(new_socket, &number_vectors, sizeof number_vectors, 0);
        
        for (auto const& sub_vector : contours)
        {
            // First send the number of elements
            uint32_t number_elements = sub_vector.size();
            send(new_socket, &number_elements, sizeof number_elements, 0);
            
            // Then send the actual data
            send(new_socket, sub_vector.data(), sub_vector.size() * sizeof sub_vector[0], 0);
        }
        
        
    }
    return 0;
    
}




