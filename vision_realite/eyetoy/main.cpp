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

  int i = 0;
  int cpt = 0;
  int firstCpt = 0;
  HandGesture hg;
  
  for(;;) {
      Mat frame;
      cap >> frame;

      std::stringstream ss;

      if(hd == nullptr) {
	hd = new HandDetection(frame.rows, frame.cols);
	std::cout << "created HandDetection!" << std::endl;
      }
      // hd->detect(frame);
      
      // if(i == 0 && cpt == 0) {
      // 	firstCpt++;
      // }

      // if(firstCpt == 100 && cpt == 0) {
      // 	i++;
      // }

      // if(i == 30 && cpt == 0) {
      // 	cpt = 30;
      // }

      // if(cpt > 2) {
      // 	cpt--;
	      
      // 	ss << cpt;
      // 	putText(frame, ss.str(), Point(100, 20), FONT_HERSHEY_PLAIN, 1,  Scalar(0,0,255,255));

      // } else if (cpt == 2) {
      // 	hg = hd->detect(frame);
      // 	std::cout << "detected gesture" << std::endl;
      // 	cpt--;
      // } else if (cpt == 1) {
      // 	hg = hd->detect(frame);
      // 	std::string player, cpu;
      // 	switch(hg) {
      // 	case PIERRE : player = "player = pierre"; cpu = "cpu = feuille"; break;
      // 	case FEUILLE : player = "player = feuille"; cpu = "cpu = ciseaux"; break;
      // 	case CISEAUX : player = "player = ciseaux"; cpu = "cpu = pierre"; break;
      // 	case ERROR : player = "player = nothing detected"; break;
      // 	}
      // 	putText(frame, player, Point(100, 20), FONT_HERSHEY_PLAIN, 1,  Scalar(0,0,255,255));
      // 	putText(frame, cpu, Point(100, 50), FONT_HERSHEY_PLAIN, 1,  Scalar(0,0,255,255));
      // }

      hg = hd->detect(frame);
      std::string player, cpu;
      switch(hg) {
      case PIERRE : player = "player = pierre"; cpu = "cpu = feuille"; break;
      case FEUILLE : player = "player = feuille"; cpu = "cpu = ciseaux"; break;
      case CISEAUX : player = "player = ciseaux"; cpu = "cpu = pierre"; break;
      case ERROR : player = "player = nothing detected"; break;
      }
      putText(frame, player, Point(100, 20), FONT_HERSHEY_PLAIN, 1,  Scalar(0,0,255,255));
      putText(frame, cpu, Point(100, 50), FONT_HERSHEY_PLAIN, 1,  Scalar(0,0,255,255));

      imshow("frame", frame);
      	  
      if( waitKey(1) == 27 ) break;
    }
  return 0;
}
