#include "opencv2/opencv.hpp"

using namespace cv;

#define VARIATIONS_DEPTH 2

int cpt = 0;

double distance(Vec3b v1, Vec3b v2) {
   Vec3b v = v1 - v2;
   return cv::norm(v);
}

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

      if(abs(lpd) > abs(tpd)) {
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
  for (int i = 0; i < oldMeans.rows; i++) {
    for (int j = 0; j < oldMeans.cols; j++) {
      Vec<uchar, 3> om = oldMeans.at<Vec<uchar, 3> >(i, j);
      Vec<uchar, 3> cm = currentMeans.at<Vec<uchar, 3> >(i, j);
      if(abs(om[0] - cm[0]) +  abs(om[1] - cm[1]) +  abs(om[2] - cm[2]) < threshold) {
	weights.at<uchar>(i, j) = max(0, weights.at<uchar>(i, j) - 1);
      } else {
	weights.at<uchar>(i, j) = min(255, weights.at<uchar>(i, j) + 10);
	// weights.at<uchar>(i, j) = 255;
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

int main(int argc, char** argv)
{
    VideoCapture cap;
    Mat variations;
    Mat base;
    Mat weights; // matrice de poids des pixels, plus un poids est haut, plus c'est un pixel de confiance càd qu'il varie peu = fait partie du fond
    Mat means; // matrice des moyennes des pixels, moyenne sur un carré de n*n pour réduire le bruit
    Mat tempMeans;
    Mat lastFrame;
    Mat background; // mask pour le background noir = background / blanc = pas background
    
    // open the default camera, use something different from 0 otherwise;
    // Check VideoCapture documentation.
    if(!cap.open(0))
        return 0;

    int i = 0;

    BackgroundSubtractorMOG *pMOG = new BackgroundSubtractorMOG(); //MOG approach
    BackgroundSubtractorMOG2 *pMOG2 = new BackgroundSubtractorMOG2(); //MOG2 approach

    for(;;)
    {
          Mat frame;
          cap >> frame;

	  for (int i = 0; i < frame.rows; i++) {
	    for (int j = 0; j < frame.cols; j++) {
	      auto v = frame.at<Vec<uchar, 3> >(i, j);
	      // frame.at<Vec<uchar, 3> >(i, j)[0] =
	    }
	  }

	  Mat fgMaskMOG; //fg mask generated by MOG method
	  Mat fgMaskMOG2;

	  pMOG->operator()(frame, fgMaskMOG);
	  pMOG2->operator()(frame, fgMaskMOG2);

	  // dilate(frame, frame, Mat());
	  // dilate(frame, frame, Mat());
	  // dilate(frame, frame, Mat());

	  // erode(frame, frame, Mat());

	  // imshow("base frame", frame);


	  Mat computedVariations(frame.rows, frame.cols, CV_8UC1);
	  
	  //initialisation des matrices
	  if(variations.rows == 0) {
	    cvtColor(frame, base, CV_RGB2GRAY);
	    variations = Mat(frame.rows, frame.cols, CV_8UC(VARIATIONS_DEPTH));
	    weights = Mat(frame.rows, frame.cols, CV_8UC1);
	    background = Mat(frame.rows, frame.cols, CV_8UC1);
	    means = Mat(frame.rows, frame.cols, CV_8UC3);
	    tempMeans = Mat(frame.rows, frame.cols, CV_8UC3);
	    std::cout << "created matrices" << std::endl;

	    for (int i = 0; i < frame.rows; i++) {
	      for (int j = 0; j < frame.cols; j++) {
		weights.at<uchar>(i, j) = 0;
	      }
	    }
 	    
	    computeMeans(frame, tempMeans, 0);
	    means = tempMeans;
	    lastFrame = frame;
	    computeWeights(means, frame, weights, 10);
	  }

	  // computeMeans(frame, tempMeans, 1);
	  computeWeights(frame, lastFrame, weights, 50);
	  means = tempMeans;
	  lastFrame = frame;

	  double avg = 0;
	  for (int i = 0; i < variations.rows; i++) {
	    for (int j = 0; j < variations.cols; j++) {
	      avg += weights.at<uchar>(i, j);
	    }
	  }

	  avg /= (variations.rows * variations.cols);

	  Mat outFrame = frame.clone();
	  // avg = mostFrequentColor(weights);
	  for (int i = 0; i < variations.rows; i++) {
	    for (int j = 0; j < variations.cols; j++) {
	      if(weights.at<uchar>(i, j) < 10) {
	  	outFrame.at<Vec3b>(i, j)[0] = 0;
	  	outFrame.at<Vec3b>(i, j)[1] = 0;
	  	outFrame.at<Vec3b>(i, j)[2] = 0;
	      }
	    }
	  }
	
	  
	  updateVariationsMat(frame, variations, (i++) % VARIATIONS_DEPTH);
	  computeVariations(variations, computedVariations);

	  Mat temp;
	  cvtColor(frame, temp, CV_RGB2GRAY); 
	  cpt++;

	  for (int i = 0; i < variations.rows; i++) {
	    for (int j = 0; j < variations.cols; j++) {
	      if(abs(temp.at<uchar>(i, j) - base.at<uchar>(i, j)) < 15) {
	  	outFrame.at<Vec3b>(i, j)[0] = 0;
	  	outFrame.at<Vec3b>(i, j)[1] = 0;
	  	outFrame.at<Vec3b>(i, j)[2] = 0;
	      }
              if(cpt > 2 * VARIATIONS_DEPTH && computedVariations.at<uchar>(i, j) < 35) {
	      	  base.at<uchar>(i, j) = temp.at<uchar>(i, j);
	      }
	    }
	  }
	  // Mat erosion_dst;
	  // // erode( frame, erosion_dst, Mat());
	  // dilate( frame, erosion_dst, Mat());
	  // dilate( erosion_dst, erosion_dst, Mat());
	  // erode( erosion_dst, erosion_dst, Mat());
	  // dilate( erosion_dst, erosion_dst, Mat());
	  // erode( erosion_dst, erosion_dst, Mat());
	  // erode( erosion_dst, erosion_dst, Mat());

	  imshow("computed variations", frame);
	  imshow("FG Mask MOG 2", fgMaskMOG2);
    
	  if( frame.empty() ) break; // end of video stream
          imshow("this is you, smile! :)", outFrame);
	  imshow("this is you, smile! :)1", weights);

          if( waitKey(1) == 27 ) break; // stop capturing by pressing ESC 
    }
    // the camera will be closed automatically upon exit
    // cap.close();
    return 0;
}
