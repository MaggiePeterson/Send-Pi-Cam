//
//  Metrics.cpp
//  Metrics
//
//  Created by Margaret Peterson on 4/19/19.
//  Copyright © 2019 Margaret Peterson. All rights reserved.
//

#include "Metrics.hpp"

//constructor
Metrics::Metrics(){}

void Metrics:: TargetInit(Mat *img){

   int haswhite = 0;
   int prevhaswhite = 0;
   int target_index = 0;
   vector<VerticalLine>vertical_lines;
   vector<HorizontalLine>horizontal_lines;
   vector<Target>target_list;
   VerticalLine x;
   HorizontalLine y;
   Target vision_target;
   TargetPair pair;

   pair_list.clear();

   int timeStart = clock();

   //vertical
   for(int i =0; i < img->cols; i++){

      haswhite =0;
      for(int j =0; j< img->rows; j++){ //each column

         if(img->data[j* img->cols *img->channels() + i*img->channels()] == 255){
            haswhite++;
         }
      }

      if((haswhite>=15) && !prevhaswhite){   //if current is white but prev is black
         prevhaswhite =1;
         x.start = i;
      }
      else if ((haswhite<=15) && prevhaswhite){
         prevhaswhite =0;

         x.end =i;
         vertical_lines.push_back(x);

      }
   }

   prevhaswhite = 0;
   target_index = 0;

   //horizontal
   for(int j =0; j< img->rows; j++){
      haswhite =0;

      for(int i =0; i < img->cols; i++){

         if(img->data[j* img->cols *img->channels() + i*img->channels()] == 255){  //if at least one is white
            haswhite++;
         }
      }

      if((haswhite>=5) && !prevhaswhite){   //if current is white but prev is black

         prevhaswhite =1;
         y.start = j;
      }
      else if ((haswhite<=5) && prevhaswhite){

         prevhaswhite =0;
         y.end = j;
         horizontal_lines.push_back(y);
         horizontal_lines.push_back(y);

      }
   }

   //group bounding lines to individual vision targets
   for(int i =0; i< vertical_lines.size(); i++){

      vision_target.x_line = vertical_lines[i];
      vision_target.y_line = horizontal_lines[i];
      vision_target.height = vision_target.y_line.end - vision_target.y_line.start;
      vision_target.width = vision_target.x_line.end - vision_target.x_line.start;
      vision_target.HW_Ratio = double(vision_target.height)/double(vision_target.width);

      target_list.push_back(vision_target);

   }

   //find target pairs by comparing to ratio
   for(int i=0; i< target_list.size(); i++){

      if(target_list[i].HW_Ratio - target_list[i+1].HW_Ratio <= TargetPair::TOLERANCE){
         pair.target1 =target_list[i]; //add target to pair
         pair.target2 =target_list[i+1];
         pair.length = pair.target1.width + pair.target2.width;
         pair_list.push_back(pair);

         i++; //skip checking next target

      }
   }

   cout<<"processing time for metrics::target pairing: "<<clock()-timeStart<<endl;

   if(pair_list.size() < 1){
      cout<<"ERROR: no pairs found"<<endl;
      m_noTarget = true;
   }
   else
      m_noTarget = false;


}

int Metrics::findClosetTarget(){

   // find closest target by getting largest length
   int i = 0;
   int temp = i;
   int max = 0;

   while(i< pair_list.size())
   {
      if (max < pair_list[i].length)
      {
         max = pair_list[i].length;
         temp = i;
      }
      i++;
   }

   return temp;
}

int Metrics::findClosestTargetLength(){

   int max = 0;

   for (TargetPair pair : pair_list){
      if(pair.length > max){
         max = pair.length;
      }
   }

   return max;
}

int Metrics:: getAngle(){

   if(!m_noTarget){
   int index = findClosetTarget();

   pair_list[index].center = (pair_list[index].target1.x_line.start + pair_list[index].target2.x_line.end)/2;

   int angle = DEG_PER_PIXEL * ((FRAME_WIDTH/2) - pair_list[index].center);
   return angle;
   }
   else 
    return 0;
   
}

void Metrics::lineOfRegression(){

   if(target_pair_length.size() != distance.size())
      cerr<< "ERROR: Vectors are not the same size"<<endl;

   double n = target_pair_length.size();
   double xsum = 0.0, x2sum = 0.0, ysum = 0.0, xysum = 0.0; //variables for sums/sigma of xi,yi,xi^2,xiyi etc

   for (int i=0;i<n;i++)
   {
      xsum += target_pair_length[i];                        //calculate sigma(xi)
      ysum += distance[i];                       //calculate sigma(yi)
      x2sum += pow(target_pair_length[i],2);                //calculate sigma(x^2i)
      xysum += target_pair_length[i]*distance[i];                    //calculate sigma(xi*yi)
   }
   this->slope = (n*xysum-xsum*ysum)/(n*x2sum-xsum*xsum);            //calculate slope
   this->yIntercept = (x2sum*ysum-xsum*xysum)/(x2sum*n-xsum*xsum);            //calculate intercept
}

double Metrics::getDistance(int length){

   if(!m_noTarget){
   int distance = slope*length + yIntercept;
   return distance;
   }
   else
    return -1;
}

string Metrics::getAngleAndDistance(){

   ostringstream oss;

   oss<< getAngle() <<" "<<getDistance(findClosestTargetLength());    //save data to string
   return oss.str();

}

void Metrics:: writeMetrics(string textFile){

   ofstream myFile;
   myFile.open(textFile);

   myFile << target_pair_length.size() <<endl;
   for(int i =0; i< target_pair_length.size(); i++){
      myFile<< target_pair_length[i] <<endl<<  distance[i] << endl;
   }
   myFile.close();

}

bool Metrics:: readMetrics(string textFile){

   ifstream inFile;
   inFile.open(textFile);
   int size;

   if(inFile>> size){

      target_pair_length.resize(size);
      distance.resize(size);

      for(int i=0; i< size; i++){
         inFile >> target_pair_length[i] >> distance[i];
      }

      lineOfRegression(); //calculate slope and yInt
      return true;
   }

   return false;
}



