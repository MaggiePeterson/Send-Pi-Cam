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
    
    //timing variables to check performace
    
    int contoursSize = 0;
    int nestedVecSize = 0;
    int sizeLen =  sizeof(int);
    
    vector< vector<Point> > contours;
    OpenVideo myVideo(0);
    myVideo.setAutoExposure();
    int size = 0;
    
    cout << "Capture is opened" << endl;
    while(waitKey(10) != 'q')
    {
        
        writeEdges(myVideo.getImage(), contours); //saves image edges to vector contours
        contoursSize = contours.size();
        ostringstream vts;
        string s;
        
        if (!contours.empty())
        {
            // Convert all but the last element to avoid a trailing ","
            copy(contours.begin(), contours.end()-1,
                 std::ostream_iterator< vector <Point> >(vts, "/n "));
            
            // Now add the last element with no delimiter
            vts << contours.back();
        }
        
        //cout << vts.str() << std::endl;
        s = vts.str();
        
        
      /*  if(!send(new_socket, &size, sizeof(int),0)){
            cout<<"ERROR: cannot send size of string"<<endl;
        }
        cout<<"Size of string: "<<size<<endl;
        
        if(!send(new_socket, s.c_str(), s.size(), 0)){
            cout<<"ERROR: cannot send string"<<endl;
        }
        cout<<"String: "<<s<<endl; */
        
       
        
        uint32_t dataLength = htonl(s.size()); // Ensure network byte order
        // when sending the data length
        
        send(sd,&dataLength ,sizeof(uint32_t) ,0); // Send the data length
        send(sd,dataToSend.c_str(),dataToSend.size(),0);
    }
    
    return 0;
}


