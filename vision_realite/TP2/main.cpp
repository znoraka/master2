#include <opencv2/core/core.hpp>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include "opencv2/features2d/features2d.hpp"
#include <algorithm>
#include<dirent.h>

using namespace cv;
using namespace std;

std::vector<Mat> images;
std::vector<Point2f> prevPts;
std::vector<Point2f> nextPts;

#define CONTROL_POINTS 20
#define MAX_COUNT 500
bool ready = false;

int currentIndex = 0;

void drawPoints(Mat& image, std::vector<Point2f> points) {
  for(auto i : points) {
    circle(image, i, 3, Scalar(0, 0, 255), 2);
  }
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
  Mat status, err;
  if(ready) {
    calcOpticalFlowPyrLK(
			 images[currentIndex],
			 images[currentIndex + 1],
			 prevPts,
			 nextPts,
			 status,
			 err);
    drawPoints(images[currentIndex+1], nextPts);
    imshow( "Window", images[currentIndex+1]);
    currentIndex = (currentIndex + 1) % (images.size() - 1);
    prevPts = nextPts;

      // std::cout << status << std::endl;
      // std::cout << err << std::endl;
      
  } else if ( event == EVENT_LBUTTONDOWN ) {
    if(!ready && prevPts.size() < CONTROL_POINTS) {
      Point2f p;
      p.x = x; p.y = y;
      prevPts.push_back(p);
      drawPoints(images[currentIndex], prevPts);
      imshow( "Window", images[currentIndex]);
    } else {
      ready = true;
    }
  } else {
    
  }
}

int main( int argc, char** argv )
{
  if( argc != 4) {
    cout <<" Usage: imagecount base_name extension" << endl;
    return -1;
  }

  std::string path = argv[1];
  std::string base = argv[2];
  std::string ext = argv[3];

  DIR *dir;
  dir = opendir(path.c_str());
  int fileCount = 0;
  string imgName;
  struct dirent *ent;
  if (dir != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      fileCount++;
    }
    closedir (dir);
  } else {
    cout<<"not present"<<endl;
  }

  fileCount -= 2;

  auto f = [=] (int n) {
    stringstream ss;
    ss << n;
    if(ss.str().length() == 1) return "00" + ss.str();
    else if(ss.str().length() == 2) return "0" + ss.str();
    else return ss.str();
  };

  for (int i = 0; i < fileCount; i++) {
    images.push_back(imread(path + base + f(i) + ext, CV_LOAD_IMAGE_GRAYSCALE));
  }


  std::cout << images.size() << std::endl;
    
  namedWindow( "Window", WINDOW_AUTOSIZE );
  setMouseCallback("Window", CallBackFunc, NULL);
  imshow( "Window", images[0]);

  goodFeaturesToTrack(images[0], prevPts, MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
  ready = true;
  cvWaitKey(0);

  // if(k == 27)
  //   return 0;
}


