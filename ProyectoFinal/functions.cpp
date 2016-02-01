/* 
 * File:   functions.cpp
 * Author: cris
 *
 * Created on 01 February 2016, 16:01
 */



#include <vector>
#include "functions.h"
#include <math.h> 
#include <opencv2/imgproc.hpp>

vector<Mat> getRGBMatrix(Mat image){

    vector<Mat> channels;
    cv::split(image,channels);
    
    cout<<"NUM CANALES "<<channels.size()<<endl;
//    cout<<"RED"<<channels.at(0)<<endl;
//    cout<<"GREEN"<<channels.at(1)<<endl;
//    cout<<"BLUE"<<channels.at(2)<<endl;
    return channels;
}