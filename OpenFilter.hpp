//
//  OpenFilter.hpp
//  Add Filters
//
//  Created by Vivian Zhu on 11/17/18.
//  Copyright © 2018 Vivian Zhu. All rights reserved.
//

#ifndef OpenFilter_hpp
#define OpenFilter_hpp

#include <stdio.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <fstream>

#define WINDOW_NAME "Setup"
using namespace cv;
using namespace std;

class Filter {

public:

   Filter();
   void config(Mat *img);
   void createHash();
   void writeHSV(string the_file);
   bool readHSV(const string &the_file);
   Mat edgeDetect(Mat *img);
   Mat edgeDetectConfig(Mat *img);
   Mat* getH();
   Mat* getS();
   Mat* getV();

private:

   Mat *h, *s, *v;
   int h_min, h_max, s_min, s_max, v_min, v_max; // for regular threshold
   const static int MAX_BINARY_VALUE = 255;
   const static int BINARY            = 0; // set to 255 if greater than min value
   const static int TO_ZERO           = 3; // set to 0 if less than min value
   const static int TO_ZERO_INVERTED  = 4; // set to 0 if greater than max value
   int h_hash[255] ={0};
   int s_hash[255] ={0};
   int v_hash[255] ={0};
   void createHSV(Mat *img);
   void hashThresh(Mat *img);
   void thresh(Mat *img, int min_value, int max_value);

   struct px{
      uint8_t h;
      uint8_t s;
      uint8_t v;
   };

};

#endif /* OpenFilter_hpp */
