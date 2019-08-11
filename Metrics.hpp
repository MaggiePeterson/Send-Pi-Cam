//
//  Metrics.hpp
//  Metrics
//
//  Created by Margaret Peterson on 4/19/19.
//  Copyright © 2019 Margaret Peterson. All rights reserved.
//

#ifndef Metrics_hpp
#define Metrics_hpp

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <stdio.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <cmath>
#include <sstream>

using namespace std;
using namespace cv;

class Metrics{

public:

   Metrics(void);
   void TargetInit(Mat *img);
   int getAngle();
   double getDistance(int length);
   void writeMetrics(string textFile);
   bool readMetrics(string filename);
   string getAngleAndDistance(); //returns in combined text

private:
   //structs
   struct VerticalLine {
      int start = 0;
      int end = 0;
   };

   struct HorizontalLine {
      int start = 0;
      int end = 0;
   };
 
   struct Target {
      HorizontalLine y_line;
      VerticalLine x_line;
      int width = 0;
      int height = 0;
      double HW_Ratio = 0.0;
   };

   struct TargetPair {
      Target target1;
      Target target2;
      int center = 0;
      int length = 0;
      static constexpr double TOLERANCE = 2;
   };

   vector<int>target_pair_length;
   vector<double>distance;
   vector<TargetPair>pair_list;
   int findClosetTarget();
   int findClosestTargetLength();
   void lineOfRegression();
   double slope;
   double yIntercept;
   const int FRAME_WIDTH = 640;
   const double FIELD_OF_VIEW  = 68.5;
   const double DEG_PER_PIXEL = FIELD_OF_VIEW/FRAME_WIDTH;
   bool m_noTarget;

};

#endif /* Metrics_hpp */
