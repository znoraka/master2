#ifndef HAND_DETECTION_H
#define HAND_DETECTION_H

#include <vector>
#include <functional>
#include <algorithm>
#include <string>

#include "opencv2/opencv.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

class HandDetection {
 public:
  HandDetection(int rows, int cols);
  void detect(Mat frame);
  void updateBackground(Mat frame);
  void computeBackground();
  void computeBackgroundMask(Mat frame, int thy, int thcb);
  void extractColorSamples(Mat frame, Mat mask);
  bool containsHand(Mat frame);
  void filterColorSamples();
  
 private:
  Mat background;
  Mat backgroundMask;
  Mat tempBackgroundForSum;

  Mat fileBg;
  Mat fileHand;

  CascadeClassifier face_cascade;

  Rect *face;

  int frameCount;
  int n;
  
  std::vector<std::vector<Point> > contours;
  std::vector<Vec4i> hierarchy;
  std::vector<std::vector<Vec4i> > defects;
  std::vector<Scalar> colorSamples;
  std::vector<Scalar> filteredColorSamples;
};

#endif
