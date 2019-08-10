//Maggie Peterson 2019

#include "OpenVideo.hpp"
#include "OpenFilter.hpp"
#include "Metrics.hpp"
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <thread>
#include <time.h>

#define PORT 5801
#define BUFFER_SIZE 1*800*600
using namespace cv;
using namespace std;

float procTime(timeval start, timeval end)
{
    return ((float)(end.tv_sec-start.tv_sec) +(float)((end.tv_usec-start.tv_usec)/1000000.0));
}

int main()
{
    Mat* raw_img = new Mat;
    Mat* target_img = new Mat;

    Filter myFilter;
    Metrics myMetrics;
            
    const string HSV_file = "HSV.txt";
    const string Metrics_file = "Metrics.txt";
    String data;
    
    //set up UDP
    int sock;                         /* Socket */
    struct sockaddr_in broadcastAddr; /* Broadcast address */
    int broadcastPermission = 1;          /* Socket opt to set permission to broadcast */

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
    
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;   /* Broadcast IP address */
    broadcastAddr.sin_port = htons(PORT);               /* Broadcast port */
    cout << "Starting beacon" << endl;
    
    OpenVideo stream(0); //opens camera stream
    cout << "Capture is opened" << endl;

	//read from files
    myFilter.readHSV(HSV_file);
    myMetrics.readMetrics(Metrics_file); 
    
    struct timeval realTime, startTime, stop1, stop2, stop3, stop4;
    float imgGetTime, edgeTime, metricsTime;
    gettimeofday(&startTime,NULL);

   vector<vector<Point> > contours;
   vector<Vec4i> hierarchy;

   while(1){     //sends distance and angle
    
        gettimeofday(&stop1,NULL);
       
        *raw_img = stream.getImage();
        gettimeofday(&stop2,NULL);

Mat yuck =   myFilter.edgeDetectConfig(raw_img);
        *target_img = myFilter.edgeDetect(raw_img);
        gettimeofday(&stop3,NULL);

       myMetrics.TargetInit(target_img);  
//       data = myMetrics.getAngleAndDistance();
       gettimeofday(&stop4,NULL);

//-----------------------------------------------
      /// Find contours
      int startTime = clock();
      findContours(yuck, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); //external
float radius1 =0, radius2 =0;
float center1 =0, center2=0;
      // Approximate contours to polygons + get bounding rects and circles
      vector<vector<Point> > contours_poly( contours.size() );
      vector<Rect> boundRect( contours.size() );
      vector<Point2f>center( contours.size() );
      vector<float>radius( contours.size() );

      for( int i = 0; i < contours.size(); i++ )
      { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
         boundRect[i] = boundingRect( Mat(contours_poly[i]) );
         minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
      }

    Mat drawing = Mat::zeros(yuck.size(), CV_8UC3 ); //
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( 233, 100,100 );
        drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
        rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
        circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
        
        if (radius[i]> radius1)             //gets 2 largest radii
        {
            radius2 = radius1;
            radius1 = radius[i];
            center1 = center[i].x;
        }
        else if (radius[i] > radius2){
            radius2 = radius[i];
            center2 = center[i].x;
        }
        
        
}

      cout<<"Processing time for bounding box: "<<clock() - startTime<<endl;
//-----------------------------------------------


/*       sendto(sock, data.c_str(), data.length(), 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr));
       gettimeofday(&realTime,NULL);
       cout << ">Total Time: "<< procTime(stop1, realTime)<< endl << "-->Img Get Time: "<< procTime(stop1, stop2)<< endl  << 
            "-->Img Thres Time: "<< procTime(stop2, stop3)<< endl  << "-->Metrics Time: " << procTime(stop3, stop4) <<endl <<
            "-->Data:"<< data << endl;	*/

      }

    return 0;

}
    
