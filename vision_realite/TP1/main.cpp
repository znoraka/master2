#include <opencv2/core/core.hpp>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include "opencv2/features2d/features2d.hpp"
#include <algorithm>

using namespace cv;
using namespace std;

#define CONTROL_POINTS 18

std::vector<Point2f> pointsG;
std::vector<Point2f> pointsD;

int width;
Mat imageG;
Mat imageD;
Mat fundamental;

void MyLine(Mat& image, Point start, Point end )
{
  int thickness = 2;
  int lineType = 8;
  line( image,
        start,
        end,
        Scalar( 0, 255, 0 ),
        thickness,
        lineType );
}

void drawPoints(Mat& image, std::vector<Point2f> points) {
  for(auto i : points) {
    circle(image, i, 3, Scalar(0, 0, 255), 2);
  }
}

void computeFundamentalMatrix() {
  if(fundamental.rows == 0) {
    for (size_t i = 0; i < pointsG.size(); i++) {
      std::cout << pointsG[i] << " - " << pointsD[i] << std::endl;
    }

    fundamental = findFundamentalMat(Mat(pointsG), Mat(pointsD), FM_8POINT);
    std::cout << fundamental << std::endl;
  }
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata, int whichImage)
{
  auto drawEpilines = [=](Mat F, std::vector<Point2f> pts, Mat image, std::string windowName, int whichImage) {
    F = findFundamentalMat(pointsD, pointsG, CV_FM_LMEDS);

    Point2f p;
    p.x = x; p.y = y;

    std::vector<Point2f> in;
    in.push_back(p);
    std::vector<Vec3f> out;
	 
    computeCorrespondEpilines(in, whichImage, F, out);

    for(int i = 0; i < pointsG.size(); i++)
      {
	// Creating p1, p2, the two points. Please let me know if this can be done in fewer lines.
	cv::Mat p1(3,1, CV_64FC1), p2(3,1, CV_64FC1);

	p1.at<double>(0) = pointsG.at(i).x;
	p1.at<double>(1) = pointsG.at(i).y;
	p1.at<double>(2) = 1.0;

	p2.at<double>(0) = pointsD.at(i).x;
	p2.at<double>(1) = pointsD.at(i).y;
	p2.at<double>(2) = 1.0;

	// Print yFx for each pair of points. This should be 0 for all.
	cout << "yFx = " << p1.t() * F * p2 << endl;
      }

    for(auto i : out) {
      // std::cout << i << std::endl;
      Point p1, p2;
      p1.x = 0; p1.y = -i[2] / i[1];
      p2.x = image.cols; p2.y =  -(i[2]+i[0]*image.cols)/i[1];

      MyLine(image, p1, p2);
    }
    imshow(windowName , image );
  
  };
  
  if  ( event == EVENT_LBUTTONDOWN ) {
    if(whichImage == 1) {
      /** Pour tous les points de control **/
      // if(whichImage == 1) {
      // 	drawEpilines(fundamental, pointsD, imageG, "WindowG", 1);
      // } else {
      // 	drawEpilines(fundamental, pointsG, imageD, "WindowD", 2);
      // }

      drawEpilines(fundamental, pointsG, imageD, "WindowD", 1);
    } else {
      drawEpilines(fundamental, pointsD, imageG, "WindowG", 2);
    }
  }
}


void CallBackFuncG(int event, int x, int y, int flags, void* userdata) {
  if ( event == EVENT_LBUTTONDOWN ) {
    Point2f p;
    p.x = x; p.y = y;
    if(pointsG.size() < CONTROL_POINTS) {
      pointsG.push_back(p);
      std::cout << pointsG.size() << std::endl;
    } else {
      if(pointsD.size() == CONTROL_POINTS && pointsD.size() == CONTROL_POINTS) {
	computeFundamentalMatrix();
      }
      CallBackFunc(event, x, y, flags, userdata, 1);
    }
    drawPoints(imageG, pointsG);
    imshow( "WindowG", imageG );
  }
}

void CallBackFuncD(int event, int x, int y, int flags, void* userdata) {
  if ( event == EVENT_LBUTTONDOWN ) {
    Point2f p;
    p.x = x; p.y = y;
    if(pointsD.size() < CONTROL_POINTS) {
      pointsD.push_back(p);
      std::cout << pointsD.size() << std::endl;
      
    } else {
      if(pointsD.size() == CONTROL_POINTS && pointsD.size() == CONTROL_POINTS) {
	computeFundamentalMatrix();
      }
      CallBackFunc(event, x, y, flags, userdata, 2);
    }
    drawPoints(imageD, pointsD);
    imshow( "WindowD", imageD );
  }
}

int main( int argc, char** argv )
{
    if( argc != 3)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    // Mat image;
    imageG = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    imageD = imread(argv[2], CV_LOAD_IMAGE_COLOR);

    if(! imageG.data && ! imageD.data) {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    pointsG.push_back(Point2f(123, 59)); pointsD.push_back(Point2f(157, 55));
    pointsG.push_back(Point2f(140, 253)); pointsD.push_back(Point2f(121, 255));
    pointsG.push_back(Point2f(152, 191)); pointsD.push_back(Point2f(138, 191));
    pointsG.push_back(Point2f(211, 148)); pointsD.push_back(Point2f(233, 145));
    pointsG.push_back(Point2f(99, 260)); pointsD.push_back(Point2f(86, 259));
    pointsG.push_back(Point2f(41, 149)); pointsD.push_back(Point2f(67, 149));
    pointsG.push_back(Point2f(140, 117)); pointsD.push_back(Point2f(158, 113));
    pointsG.push_back(Point2f(192, 272)); pointsD.push_back(Point2f(179, 268));
    
    namedWindow( "WindowG", WINDOW_AUTOSIZE );
    setMouseCallback("WindowG", CallBackFuncG, NULL);
    // drawPoints(imageG, pointsG);
    imshow( "WindowG", imageG );

    namedWindow( "WindowD", WINDOW_AUTOSIZE );
    setMouseCallback("WindowD", CallBackFuncD, NULL);
    // drawPoints(imageD, pointsD);
    imshow( "WindowD", imageD );
    
    // char k;

    cvWaitKey(0);

    // if(k == 27)
    //   return 0;
}


