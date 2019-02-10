//
//  OpenVideo.h
//  cameraSettingsLibUVC
//
//  Created by Margaret Peterson on 11/10/18.
//  Copyright © 2018 Margaret Peterson. All rights reserved.
//

#ifndef OpenVideo_h
#define OpenVideo_h
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"


#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"

#include <iostream>
#include "libuvc/libuvc.h"
#include <stdio.h>
#include <unistd.h>

//C++
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace cv;
using namespace std;

#endif /* OpenVideo_h */

class OpenVideo {
    
public:
    OpenVideo(int camNum);
    
    static void cb(uvc_frame_t *frame, void *ptr);
    Mat getImage();
    void ChangeExposure();
    void setAutoExposure();
    
private:
  
    int webCamIndex;
    VideoCapture *capture;
};

