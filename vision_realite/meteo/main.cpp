#include "opencv2/opencv.hpp"
using namespace cv;

#define VARIATIONS_DEPTH 5

int cpt = 0;

double distance(Vec3b v1, Vec3b v2) {
   Vec3b v = v1 - v2;
   return cv::norm(v);
}

void computeDifferenceMap(Mat frame, Mat toReturn) {
  auto topPixelDifference = [=] (Mat m, int i, int j, int value) {
    return m.at<uchar>(i - 1, j) - value;
  };

  auto leftPixelDifference = [=] (Mat m, int i, int j, int value) {
    return m.at<uchar>(i, j - 1) - value;
  };

  Mat grayScale;
  cvtColor(frame, grayScale, CV_RGB2GRAY);
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

      if(abs(lpd) < abs(tpd)) {
	value = abs(lpd) * 20 + 127;
      } else {
	value = abs(tpd) * 20 + 127;
      }
      if(value > 255) value = 255;
      if(value < 0) value = 0;
      toReturn.at<uchar>(i, j) = value;
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
    return sqrt(sum / VARIATIONS_DEPTH);
  };
  
  for (int i = 0; i < variations.rows; i++) {
    for (int j = 0; j < variations.cols; j++) {
      toReturn.at<uchar>(i, j) = vecStdDev(variations.at<Vec<uchar, VARIATIONS_DEPTH> >(i, j));
    }
  }
}

int main(int argc, char** argv)
{
    VideoCapture cap;
    Mat variations;
    Mat base;
    // open the default camera, use something different from 0 otherwise;
    // Check VideoCapture documentation.
    if(!cap.open(0))
        return 0;

    int i = 0;
    for(;;)
    {
          Mat frame;
          cap >> frame;
	  Mat computedVariations(frame.rows, frame.cols, CV_8UC1);
	  
	  if(variations.rows == 0) {
	    // base = frame.clone();
	    cvtColor(frame, base, CV_RGB2GRAY);
		  
	    variations = Mat(frame.rows, frame.cols, CV_8UC(VARIATIONS_DEPTH));
	    std::cout << "created variations matrix" << std::endl;
	  }
	  // std::cout << "before" << std::endl;
	  updateVariationsMat(frame, variations, (i++) % VARIATIONS_DEPTH);
	  // std::cout << "after" << std::endl;
	  computeVariations(variations, computedVariations);

	  Mat temp;
	  cvtColor(frame, temp, CV_RGB2GRAY); 
	  // computeDifferenceMap(frame, temp);

	  cpt++;

	  for (int i = 0; i < variations.rows; i++) {
	    for (int j = 0; j < variations.cols; j++) {
	      if(abs(temp.at<uchar>(i, j) - base.at<uchar>(i, j)) < 15) {
	  	frame.at<Vec3b>(i, j)[0] = 0;
	  	frame.at<Vec3b>(i, j)[1] = 0;
	  	frame.at<Vec3b>(i, j)[2] = 0;
	      }
              if(cpt > 2 * VARIATIONS_DEPTH && computedVariations.at<uchar>(i, j) < 15) {
	      	  base.at<uchar>(i, j) = temp.at<uchar>(i, j);
	      }
	      // if(computedVariations.at<uchar>(i, j) < 5) {
	      // 	  for (int k = 0; k < 3; k++) {
	      // 	    frame.at<Vec3b>(i, j)[k] = 0;
              //    }
	      // }
	    }
	  }
	  
          if( frame.empty() ) break; // end of video stream
          imshow("this is you, smile! :)", frame);
          if( waitKey(1) == 27 ) break; // stop capturing by pressing ESC 
    }
    // the camera will be closed automatically upon exit
    // cap.close();
    return 0;
}
