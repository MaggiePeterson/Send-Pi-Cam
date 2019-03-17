//
//  Metrics.hpp
//  VisionProcessing2019
//
//  Created by Margaret Peterson on 1/14/19.
//  Copyright © 2019 Margaret Peterson. All rights reserved.
//

#ifndef Metrics_hpp
#define Metrics_hpp

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <stdio.h>
#include <numeric>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string>

using namespace std;
using namespace cv;

class Metrics{
public:
    Metrics(int frameWidth, int degree);
    void drawBoundingBox(Mat *img);
    void calibrateZero(Mat *img, float dist);
    void configValues(Mat *img, float dist);
    int getAngle();
    float getDistance();
    void saveToFile(string textFile);
    
private:
    vector<float>radiusX;
    vector<float>distY;
    float lineOfRegression(float rad1, float rad2);
    int frameW;
    int FOV;
    float zeroDist;
    Mat imgAtZero;
    int angle;
    float radius1, radius2;
    
};

#endif /* Metrics_hpp */
