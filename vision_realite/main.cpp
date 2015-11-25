#include <opencv2/core/core.hpp>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

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
    circle(image, i, 5, Scalar(0, 0, 255), 3);
  }
}

void computeFundamentalMatrix() {
  if(fundamental.rows == 0) {
    for (size_t i = 0; i < pointsG.size(); i++) {
      std::cout << pointsG[i] << " - " << pointsD[i] << std::endl;
    }

    fundamental = findFundamentalMat(Mat(pointsG), Mat(pointsD), CV_FM_8POINT);
    std::cout << fundamental << std::endl;
  }
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata, int whichImage)
{
  auto drawEpilines = [=](Mat F, Mat pts, Mat image, std::string windowName, int whichImage) {
    Point2f p;
    p.x = x; p.y = y;

    std::vector<Point2f> in;
    in.push_back(p);
    std::vector<Vec3f> out;
	 
    computeCorrespondEpilines(pts, whichImage, F, out);

    for(auto i : out) {
      // std::cout << i << std::endl;
      Point2d p1, p2;
      if(whichImage == 1) {
	p1.x = 0; p1.y = -i[2] / i[1];
	p2.x = image.cols; p2.y =  -(i[2]+i[0]*image.cols)/i[1];
      } else {
	p1.x = image.cols; p1.y = -i[2] / i[1];
	p2.x = 0; p2.y =  -(i[2]+i[0]*image.cols)/i[1];
      }

      MyLine(image, p1, p2);
    }
    imshow(windowName , image );
  
  };
  
  if  ( event == EVENT_LBUTTONDOWN ) {
    if(whichImage == 1) {
      drawEpilines(fundamental, Mat(pointsD), imageG, "WindowG", 2);
    } else {
      drawEpilines(fundamental.t(), Mat(pointsG), imageD, "WindowD", 1);
    }
  }
}


void CallBackFuncG(int event, int x, int y, int flags, void* userdata) {
  if ( event == EVENT_LBUTTONDOWN ) {
    Point2f p;
    p.x = x; p.y = y;
    if(pointsG.size() < 8) {
      pointsG.push_back(p);
      std::cout << pointsG.size() << std::endl;
    } else {
      if(pointsD.size() == 8 && pointsD.size() == 8) {
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
    if(pointsD.size() < 8) {
      pointsD.push_back(p);
      std::cout << pointsD.size() << std::endl;
      
    } else {
      if(pointsD.size() == 8 && pointsD.size() == 8) {
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

    // pointsG.push_back(Point2f(137, 255));pointsD.push_back(Point2d(119, 254));
    // pointsG.push_back(Point2f(158, 255));pointsD.push_back(Point2d(138, 255));
    // pointsG.push_back(Point2f(153, 194));pointsD.push_back(Point2d(139, 194));
    // pointsG.push_back(Point2f(123, 63));pointsD.push_back(Point2d(155, 61));
    // pointsG.push_back(Point2f(69, 146));pointsD.push_back(Point2d(91, 147));
    // pointsG.push_back(Point2f(236, 153));pointsD.push_back(Point2d(258, 152));
    // pointsG.push_back(Point2f(20, 140));pointsD.push_back(Point2d(52, 138));
    // pointsG.push_back(Point2f(142, 318));pointsD.push_back(Point2d(126, 317));


    // pointsG.push_back(Point2f(118, 256)); pointsD.push_back(Point2f(137, 255));
    // pointsG.push_back(Point2f(138, 256)); pointsD.push_back(Point2f(160, 256));
    // pointsG.push_back(Point2f(86, 261)); pointsD.push_back(Point2f(99, 264));
    // pointsG.push_back(Point2f(175, 270)); pointsD.push_back(Point2f(191, 271));
    // pointsG.push_back(Point2f(128, 183)); pointsD.push_back(Point2f(132, 186));
    // pointsG.push_back(Point2f(157, 182)); pointsD.push_back(Point2f(160, 182));
    // pointsG.push_back(Point2f(88, 141)); pointsD.push_back(Point2f(69, 143));
    // pointsG.push_back(Point2f(233, 146)); pointsD.push_back(Point2f(211, 148));
    // computeFundamentalMatrix();
    
    namedWindow( "WindowG", WINDOW_AUTOSIZE );
    setMouseCallback("WindowG", CallBackFuncG, NULL);
    // drawPoints(imageG, pointsG);
    imshow( "WindowG", imageG );

    namedWindow( "WindowD", WINDOW_AUTOSIZE );
    setMouseCallback("WindowD", CallBackFuncD, NULL);
    // drawPoints(imageD, pointsD);
    imshow( "WindowD", imageD );
    
    char k;

    k=cvWaitKey(0);

    if(k == 27)
      return 0;
}


