#include "opencv2/opencv.hpp"
#include <algorithm>
#include <functional>
#include <handdetection.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <ctime>

int main(int argc, char** argv)
{
  VideoCapture cap;
  HandDetection *hd = nullptr;

  std::srand(std::time(0));
  if(!cap.open(0))
    return 0;
  
  HandGesture hg;
  Mat pierre = imread("./pierre.png");
  Mat feuille = imread("./feuille.png");
  Mat ciseaux = imread("./ciseaux.png");

  for(;;) {
      Mat frame;
      cap >> frame;


      std::stringstream ss;

      if(hd == nullptr) {
	hd = new HandDetection(frame.rows, frame.cols);
	std::cout << "created HandDetection!" << std::endl;
      }

      hg = hd->detect(frame);
      Mat cpuImage = Mat::zeros( feuille.size(), CV_8UC3 );
      Mat playerImage = Mat::zeros( feuille.size(), CV_8UC3 );

      std::string player, cpu;
      switch(hg) {
      case PIERRE : player = "player = pierre"; playerImage = pierre; cpu = "cpu = feuille"; cpuImage = feuille; break;
      case FEUILLE : player = "player = feuille"; playerImage = feuille; cpu = "cpu = ciseaux"; cpuImage = ciseaux; break;
      case CISEAUX : player = "player = ciseaux"; playerImage = ciseaux; cpu = "cpu = pierre"; cpuImage = pierre; break;
      case ERROR : player = "player = nothing detected"; break;
      }
      putText(frame, cpu, Point(5, frame.rows * 0.98), FONT_HERSHEY_PLAIN, 1,  Scalar(0,0,255,255));
      putText(frame, player, Point(frame.cols - player.size() * 9, frame.rows * 0.98), FONT_HERSHEY_PLAIN, 1,  Scalar(0,0,255,255));

      for (int i = 0; i < feuille.rows; i++) {
	for (int j = 0; j < feuille.cols; j++) {
	  frame.at<Vec3b>(i, j) = cpuImage.at<Vec3b>(i, j);
	}
      }

      for (int i = 0; i < feuille.rows; i++) {
	for (int j = 0; j < feuille.cols; j++) {
	  frame.at<Vec3b>(i, j + frame.cols - playerImage.cols) = playerImage.at<Vec3b>(i, j);
	}
      }

      if(hd->handForUi.rows > 0) {
	cv::resize(hd->handForUi, hd->handForUi, feuille.size());

	for (int i = 0; i < feuille.rows; i++) {
	  for (int j = 0; j < feuille.cols; j++) {
	    Vec3b v = frame.at<Vec3b>(i + frame.rows * 0.3, j + frame.cols - playerImage.cols);
	    v[0] = hd->handForUi.at<uchar>(i, j);
	    v[1] = hd->handForUi.at<uchar>(i, j);
	    v[2] = hd->handForUi.at<uchar>(i, j);
	    frame.at<Vec3b>(i + frame.rows * 0.3, j + frame.cols - playerImage.cols) = v;
	  }
	}
      }


      putText(frame, "CPU", Point(feuille.rows * 0.3, feuille.cols * 1.1), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));
      putText(frame, "PLAYER", Point(frame.rows * 1.07, feuille.cols * 1.1), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,255,255));
    
      imshow("Game Frame", frame);
      	  
      if( waitKey(1) == 27 ) break;
    }
  return 0;
}
