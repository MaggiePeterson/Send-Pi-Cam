//
//  Metrics.cpp
//  VisionProcessing2019
//
//  Created by Margaret Peterson on 1/14/19.
//  Copyright © 2019 Margaret Peterson. All rights reserved.
//

#include "Metrics.hpp"
#include <stdio.h>
#include <numeric>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
using namespace std;
using namespace cv;

Metrics::Metrics(int frameWidth, int degree){
    this->frameW = frameWidth;
    this->FOV = degree;
    vector<float>radiusX = {0};
    vector<float>distY = {0};
}

void Metrics:: drawBoundingBox(Mat *image){
    RNG rng(12345);
    this->radius1 =0;
    this->radius2 =0;
    float midpoint =0;
    float center1 =0, center2=0;
    
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    
    /// Find contours
    findContours(*image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    
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
    
    /// Draw polygonal contour + bonding rects + circles
    Mat drawing = Mat::zeros(image->size(), CV_8UC3 ); //
    for( int i = 0; i< contours.size(); i++ )
    {
        if (radius[i]> radius1)             //gets 2 largest radii
        {
            this->radius2 = radius1;
            this->radius1 = radius[i];
            center1 = center[i].x;
        }
        else if (radius[i] > radius2){
            this->radius2 = radius[i];
            center2 = center[i].x;
        }
        
       
        
    }
    midpoint = abs(center1-center2)/2;           //midpoint of 2 targets
    this->angle = this->FOV * (abs((this->frameW/2) - midpoint))/this->frameW; //angle = field of view * x offset of the midpoint of targets, divided by the total frame pixel width
    
}

void Metrics:: calibrateZero(Mat *img, float dist){
    
    this->radiusX.push_back(radius1);
    this->radiusX.push_back(radius2);
    this->distY.push_back(0);
    this->distY.push_back(0);
    
    this->zeroDist = dist;
}

void Metrics::configValues(Mat *img, float dist){
    
    this->radiusX.push_back(radius1);
    this->radiusX.push_back(radius2);
    this->distY.push_back(dist);
    this->distY.push_back(dist);
}

float Metrics:: lineOfRegression(float rad1, float rad2){
    
    if(this->radiusX.size() != this->distY.size()){
        cerr<< "ERROR: Vectors are not the same size"<<endl;
    }
    float n = this->radiusX.size();
    
    float avgX = accumulate(this->radiusX.begin(), this->radiusX.end(), 0.0) / n;
    float avgY = accumulate(this->distY.begin(), this->distY.end(), 0.0) / n;
    
    float numerator = 0.0;
    float denominator = 0.0;
    
    for(int i=0; i<n; ++i){
        numerator += (this->radiusX[i] - avgX) * (this->radiusX[i] - avgY);
        denominator += (this->radiusX[i] - avgX) * (this->radiusX[i] - avgX);
    }
    
    if(denominator==0){
        cerr << "ERROR: Undefined slope"<<endl;
    }
    
    float radius = (rad1+rad2)/2; //average radius
    float slope = numerator / denominator;
    float dist = slope*radius + this->zeroDist;
    return dist;
}

float Metrics::getDistance(){
    
    return lineOfRegression(radius1, radius2);
}

int Metrics:: getAngle(){
    return this->angle;
}

void Metrics:: saveToFile(string textFile){
    
    ofstream myFile;
    myFile.open(textFile);
    
    myFile<< "Radius" << "," << "Distance"<<endl;
    for(int i =0; i< this->radiusX.size(); i++){
        myFile<< this->radiusX[i] << "," << this->distY[i] << endl;
    }
    myFile.close();
    
    
}






