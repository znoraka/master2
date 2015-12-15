#include "opencv2/opencv.hpp"
#include <algorithm>
#include <functional>
#include <handdetection.hpp>
#include <cstdlib>
#include <iostream>
#include <ctime>

using namespace cv;

#define VARIATIONS_DEPTH 10

int cpt = 0;
RNG rng(12345);

// double distance(Vec3b v1, Vec3b v2) {
//   Vec3b v = v1 - v2;
//   return cv::norm(v);
// }

uchar mostFrequentColor(Mat img) {
  std::vector<int> hist;
  hist.resize(256);
  for (int i = 0; i < img.rows; i++) {
    for (int j = 0; j < img.cols; j++) {
      hist[img.at<uchar>(i, j)]++;
    }
  }
  int index = 0;
  for (int i = 0; i < hist.size(); i++) {
    if(hist[i] > hist[index])
      index = i;
  }
  return index;
}

void computeDifferenceMap(Mat frame, Mat toReturn) {
  auto topPixelDifference = [=] (Mat m, int i, int j, int value) {
    return m.at<uchar>(i - 1, j) - value;
  };

  auto leftPixelDifference = [=] (Mat m, int i, int j, int value) {
    return m.at<uchar>(i, j - 1) - value;
  };

  Mat grayScale = frame;
  // cvtColor(frame, grayScale, CV_RGB2GRAY);
  for (int i = 0; i < frame.rows; i++) {
    for (int j = 0; j < frame.cols; j++) {
      int tpd = 255;
      int lpd = 255;

      int value = grayScale.at<uchar>(i, j);

      if (i == 0) {
	//prendre le pixel du dessus
	tpd = topPixelDifference(grayScale, i, j, value);
      } else if (j == 0) {
	//prendre le pixel de gauche
	lpd = leftPixelDifference(grayScale, i, j, value);
      } else {
	lpd = leftPixelDifference(grayScale, i, j, value);
	tpd = topPixelDifference(grayScale, i, j, value);
      }

      if(abs(lpd) > abs(tpd)) {
	value = abs(lpd);
      } else {
	value = abs(tpd);
      }
      if(value > 255) value = 255;
      if(value < 0) value = 0;
      toReturn.at<uchar>(i, j) = value;
    }
  }
}

void computeMeans(Mat frame, Mat outArray, int windowSize = 1) {
  auto areaMean = [=](int x, int y) {
    double Rmean = 0, Gmean = 0, Bmean = 0;
    int n = 0;
    for (int i = x - windowSize; i <= x + windowSize; i++) {
      for (int j = y - windowSize; j <= y + windowSize; j++) {
	Vec<uchar, 3> v = frame.at<Vec<uchar, 3> >(i, j);
	Rmean += v[0];
	Gmean += v[1];
	Bmean += v[2];
	n++;
      }
    }
    Vec<uchar, 3> v(Rmean / n, Gmean / n, Bmean / n);
    return v;
  };
  
  for (int i = 0; i < frame.rows; i++) {
    for (int j = 0; j < frame.cols; j++) {
      auto v = areaMean(i, j);
      outArray.at<Vec<uchar, 3> >(i, j)[0] = v[0];
      outArray.at<Vec<uchar, 3> >(i, j)[1] = v[1];
      outArray.at<Vec<uchar, 3> >(i, j)[2] = v[2];
    }
  }
}

void computeWeights(Mat oldMeans, Mat currentMeans, Mat weights, int threshold = 10) {
  int thy = 7;
  int thcr = 40;
  int thcb = 7;
  for (int i = 0; i < oldMeans.rows; i++) {
    for (int j = 0; j < oldMeans.cols; j++) {
      Vec<uchar, 3> om = oldMeans.at<Vec<uchar, 3> >(i, j);
      Vec<uchar, 3> cm = currentMeans.at<Vec<uchar, 3> >(i, j);
      // if(abs(om[0] - cm[0]) < thy && abs(om[1] - cm[1]) < thcr && abs(om[2] - cm[2]) < thcr) {
      if(abs(om[0] - cm[0]) < thy || abs(om[2] - cm[2]) < thcb) {
	// weights.at<uchar>(i, j) = max(0, weights.at<uchar>(i, j) - 1);
	weights.at<uchar>(i, j) = 0;
      } else {
	// weights.at<uchar>(i, j) = min(255, weights.at<uchar>(i, j) + 1);
	weights.at<uchar>(i, j) = 255;
      }
    }
  }
}

void updateVariationsMat(Mat newFrame, Mat variations, int currentIndex) {
  Mat grayScale;
  cvtColor(newFrame, grayScale, CV_RGB2GRAY);
  computeDifferenceMap(newFrame, grayScale);

  for (int i = 0; i < variations.rows; i++) {
    for (int j = 0; j < variations.cols; j++) {
      variations.at<Vec<uchar, VARIATIONS_DEPTH> >(i, j)[currentIndex] = grayScale.at<uchar>(i, j);
    }
  }
}

void computeVariations(Mat variations, Mat toReturn) {
  auto vecMean = [](Vec<uchar, VARIATIONS_DEPTH> v) {
    double sum = 0;
    for (int i = 0; i < VARIATIONS_DEPTH; i++) {
      sum += v[i];
    }
    return sum / VARIATIONS_DEPTH;
  };

  auto vecStdDev = [=] (Vec<uchar, VARIATIONS_DEPTH> v) {
    double sum = 0;
    double mean = vecMean(v);
    for (int i = 0; i < VARIATIONS_DEPTH; i++) {
      sum += pow(v[i] - mean, 2);
    }
    // return sqrt(sum / VARIATIONS_DEPTH);
    return mean;
  };
  
  for (int i = 0; i < variations.rows; i++) {
    for (int j = 0; j < variations.cols; j++) {
      toReturn.at<uchar>(i, j) = vecStdDev(variations.at<Vec<uchar, VARIATIONS_DEPTH> >(i, j));
    }
  }
}

void updateBackground(Mat frame, Mat background) {
  for (int i = 0; i < frame.rows; i++) {
    for (int j = 0; j < frame.cols; j++) {
      Vec<uchar, 3> f = frame.at<Vec<uchar, 3> >(i, j);
      background.at<Vec<int, 3> >(i, j)[0] += f[0];
      background.at<Vec<int, 3> >(i, j)[1] += f[1];
      background.at<Vec<int, 3> >(i, j)[2] += f[2];
    }
  }
}

void computeBackground(Mat background, Mat outArray, int n) {
  for (int i = 0; i < background.rows; i++) {
    for (int j = 0; j < background.cols; j++) {
      Vec<int, 3> bg = background.at<Vec<int, 3> >(i, j);
      outArray.at<Vec<uchar, 3> >(i, j)[0] = bg[0] / n;
      outArray.at<Vec<uchar, 3> >(i, j)[1] = bg[1] / n;
      outArray.at<Vec<uchar, 3> >(i, j)[2] = bg[2] / n;
    }
  }
}

int main(int argc, char** argv)
{
  VideoCapture cap;
  Mat variations;
  Mat background;
  Mat outFrame;
  Mat computedVariations;
  HandDetection *hd = nullptr;

  std::srand(std::time(0));
  // open the default camera, use something different from 0 otherwise;
  // Check VideoCapture documentation.
  if(!cap.open(0))
    return 0;

  int i = 0;

  for(;;) {
      Mat frame;
      cap >> frame;

      if(hd == nullptr) {
	hd = new HandDetection(frame.rows, frame.cols);
	std::cout << "created HandDetection!" << std::endl;
      }

      hd->detect(frame);
      	  
      if( waitKey(1) == 27 ) break;
    }
  return 0;
}
