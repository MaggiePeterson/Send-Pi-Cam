//
//  OpenVideo.hpp
//  VisionProcessing2019
//
//  Created by Margaret Peterson on 1/14/19.
//  Copyright © 2019 Margaret Peterson. All rights reserved.
//

#ifndef OpenVideo_hpp
#define OpenVideo_hpp

#include <stdio.h>


#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"

#include <iostream>
#include "libuvc/libuvc.h" //has to be libuvc/libuvc.h in pi
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
    Mat getImage();
    
private:
    void ChangeExposure();
    int webCamIndex;
    static void cb(uvc_frame_t *frame, void *ptr);
    VideoCapture *capture;
};
