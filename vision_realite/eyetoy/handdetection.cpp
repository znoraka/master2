#include "handdetection.hpp"

double distance(Vec3b v1, Vec3b v2) {
  Vec3b v = v1 - v2;
  return cv::norm(v);
}

double vectorMean(std::vector<double> v) {
  double sum = 0;
  for(auto i : v) {
    sum += i;
  }
  return sum / v.size();
}

double vectorStdDev(std::vector<double> v) {
  double mean = vectorMean(v);
  double variance = 0;
  for(auto i : v) {
    variance += pow(i - mean, 2);
  }

  return sqrt(variance / v.size());
}

HandDetection::HandDetection(int rows, int cols) {
  tempBackgroundForSum = Mat(rows, cols, CV_32SC3);
  background = Mat(rows, cols, CV_8UC3);
  backgroundMask = Mat(rows, cols, CV_8UC1);
  n = 0;
}

void HandDetection::updateBackground(Mat frame) {
  for (int i = 0; i < frame.rows; i++) {
    for (int j = 0; j < frame.cols; j++) {
      Vec<uchar, 3> f = frame.at<Vec<uchar, 3> >(i, j);
      tempBackgroundForSum.at<Vec<int, 3> >(i, j)[0] += f[0];
      tempBackgroundForSum.at<Vec<int, 3> >(i, j)[1] += f[1];
      tempBackgroundForSum.at<Vec<int, 3> >(i, j)[2] += f[2];
    }
  }
}

void HandDetection::extractColorSamples(Mat frame, Mat mask) {
  long avgY = 0, avgCr = 0, avgCb = 0;
  int n = 0;
  for (int i = 0; i < frame.rows; i++) {
    for (int j = 0; j < frame.cols; j++) {
      Vec3b color = mask.at<Vec3b>(i, j);
      Vec3b pixel = frame.at<Vec3b>(i, j);
      if(color[0] + color[1] + color[2] != 0) {
	n++;
	avgY += pixel[0];
	avgCr += pixel[1];
      	avgCb += pixel[2];
      }
    }
  }

  avgY /= n;
  avgCr /= n;
  avgCb /= n;

  Mat m = frame.clone();
  for (int i = 0; i < frame.rows; i++) {
    for (int j = 0; j < frame.cols; j++) {
	
      Vec3b v = m.at<Vec3b>(i, j);

      v[0] = avgY;
      v[1] = avgCr;
      v[2] = avgCb;
      m.at<Vec3b>(i, j) = v;
    }
  }
  std::stringstream ss;
  ss << this->n;
  imwrite("./backgrounds/a" + ss.str() + ".jpg", m);

  colorSamples.push_back(Scalar(avgY, avgCr, avgCb));
}

HandGesture HandDetection::detect(Mat frame) {
  //le bruit se concentre sur Cr
  Mat temp = frame.clone();
  cvtColor(temp, temp, CV_RGB2YCrCb);
  cvtColor(temp, temp, CV_RGB2YCrCb);
  cvtColor(temp, temp, CV_RGB2YCrCb);
  
  if(n < 10) {
    //construction du background modele
    n++;
    updateBackground(temp);
  } else if (n == 10) {
    //calcul du background
    computeBackground();
    n++;
  } else {
    n++;
    //calcul du mask
    Mat tempFrame = frame.clone();
    if(filteredColorSamples.size() == 0) {
      computeBackgroundMask(temp.clone(), 8, 8);
    } else {
      computeBackgroundMask(tempFrame.clone(), 14, 14);
    }
    Mat tempBackground = backgroundMask.clone();
    erode(backgroundMask, backgroundMask, Mat());
    erode(backgroundMask, backgroundMask, Mat());
    erode(backgroundMask, backgroundMask, Mat());

    dilate(backgroundMask, backgroundMask, Mat());
    dilate(backgroundMask, backgroundMask, Mat());
    dilate(backgroundMask, backgroundMask, Mat());

    findContours( backgroundMask, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_L1, Point(0, 0) );
    
    if(contours.size() > 0) {
      vector<vector<Point> >hull( contours.size() );
      vector<vector<int> >hullI( contours.size() );
      
      std::sort(contours.begin(), contours.end(), [](std::vector<Point> a, std::vector<Point> b) {
	  return a.size() > b.size();
	});

      Mat handMask = Mat::zeros( frame.size(), CV_8UC1 );
      extractHand(tempFrame);

      dilate(handMask, handMask, Mat());
      dilate(handMask, handMask, Mat());
      dilate(handMask, handMask, Mat());

      drawContours( handMask, contours, -1, Scalar(255, 255, 255), CV_FILLED, 8, vector<Vec4i>(), 0, Point() );
      
      findContours( handMask, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_L1, Point(0, 0) );
      contours.resize(1);

      if(contours[0].size() < 100) return ERROR;
      
      if(contours[0].size() > 3) {
	for( int i = 0; i < contours.size(); i++ ) {
	  convexHull( Mat(contours[i]), hull[i], false );
	  convexHull( Mat(contours[i]), hullI[i], false ); 
	}

	std::stringstream ss;
	ss << contours[0].size();
	putText(frame, ss.str(), Point(10, 30), FONT_HERSHEY_PLAIN, 1,  Scalar(0,0,255,255));

	if(hull.size() < 2) return ERROR;
	
	//affichage des contours/enveloppes convexes
	drawContours( frame, contours, -1, Scalar(255, 0, 0), 3, 8, vector<Vec4i>(), 0, Point() );
	drawContours( frame, hull, -1, Scalar(255, 255, 255), 2, 8, vector<Vec4i>(), 0, Point() );
	drawContours( handMask, contours, -1, Scalar(255, 255, 255), CV_FILLED, 8, vector<Vec4i>(), 0, Point() );
	defects.clear();
	defects.resize(contours.size());
  
	convexityDefects(Mat(contours[0]), hullI[0], defects[0]);

	int fingerCount = extractFingerCount(frame);

	std::string s;

	Mat y(frame.rows, frame.cols, CV_8UC1);
	Mat cr(frame.rows, frame.cols, CV_8UC1);
	Mat cb(frame.rows, frame.cols, CV_8UC1);

	for (int i = 0; i < frame.rows; i++) {
	  for (int j = 0; j < frame.cols; j++) {
	    y.at<uchar>(i, j) = temp.at<Vec3b>(i, j)[0];
	    cr.at<uchar>(i, j) = temp.at<Vec3b>(i, j)[1];
	    cb.at<uchar>(i, j) = temp.at<Vec3b>(i, j)[2];
	  }
	}

	handForUi = handMask;
	// namedWindow( "Hand", CV_WINDOW_AUTOSIZE );
	// imshow( "Hand", frame);
	// namedWindow( "Hand Mask", CV_WINDOW_NORMAL);
	// imshow( "Hand Mask", handMask);
	// resizeWindow("Hand Mask", frame.rows * 0.7, frame.rows * 0.7);

	// namedWindow( "Background Mask", CV_WINDOW_NORMAL);
	// imshow( "Background Mask", tempBackground);

	// resizeWindow("Background Mask", frame.rows * 0.7, frame.rows * 0.7);

	// namedWindow( "y", CV_WINDOW_NORMAL);
	// imshow( "y", y);
	// resizeWindow("y", frame.rows * 0.7, frame.rows * 0.7);
	
	// namedWindow( "cr", CV_WINDOW_NORMAL);
	// imshow( "cr", cr);
	// resizeWindow("cr", frame.rows * 0.7, frame.rows * 0.7);

	// namedWindow( "cb", CV_WINDOW_NORMAL);
	// imshow( "cb", cb);
	// resizeWindow("cb", frame.rows * 0.7, frame.rows * 0.7);
	
	if(fingerCount <= 1) {
	  return PIERRE;
	} else if (fingerCount > 3) {
	  return FEUILLE;
	} else {
	  return CISEAUX;
	}

      }
    }
  }
  return ERROR;
}

void HandDetection::computeBackground() {
  for (int i = 0; i < tempBackgroundForSum.rows; i++) {
    for (int j = 0; j < tempBackgroundForSum.cols; j++) {
      Vec<int, 3> bg = tempBackgroundForSum.at<Vec<int, 3> >(i, j);
      background.at<Vec<uchar, 3> >(i, j)[0] = bg[0] / n;
      background.at<Vec<uchar, 3> >(i, j)[1] = bg[1] / n;
      background.at<Vec<uchar, 3> >(i, j)[2] = bg[2] / n;
    }
  }
}

void HandDetection::filterColorSamples() {
  long avgY = 0, avgCr = 0, avgCb = 0;
  
  for(Scalar i : colorSamples) {
    avgY += i[0];
    avgCr += i[1];
    avgCb += i[2];
  }

  avgY /= colorSamples.size();
  avgCr /= colorSamples.size();
  avgCb /= colorSamples.size();

  for(Scalar i : colorSamples) {
    if(sqrt(pow(i[0] - avgY, 2)) + sqrt(pow(i[0] - avgY, 2)) + sqrt(pow(i[0] - avgY, 2)) < 30) {
      filteredColorSamples.push_back(i);
    }
  }
}


void HandDetection::computeBackgroundMask(Mat frame, int thy = 10, int thcb = 10) {
  for (int i = 0; i < frame.rows; i++) {
    for (int j = 0; j < frame.cols; j++) {
      Vec<uchar, 3> om = frame.at<Vec<uchar, 3> >(i, j);
      Vec<uchar, 3> cm = background.at<Vec<uchar, 3> >(i, j);
      if(abs(om[0] - cm[0]) < thy && abs(om[2] - cm[2]) < thcb) {
	backgroundMask.at<uchar>(i, j) = 0;
      } else {
	backgroundMask.at<uchar>(i, j) = 255;
      }
    }
  }
}

int HandDetection::extractFingerCount(Mat frame) {
  int fingerCount = 0;
  std::vector<Point> pts;
  float angle;

  Point center;
  int n = 1;
  
  for (int i = 0; i < defects[0].size(); i++) {

    Vec4i v = defects[0][i];
    int startIndex = v[0];
    int endIndex = v[1];
    int farIndex = v[2];
    float depth = v[3] / 256;

    Point p1 = contours[0][startIndex];
    Point p2 = contours[0][endIndex];
    Point p3 = contours[0][farIndex];

    center.x += (p1.x + p2.x + p3.x);
    center.y += (p1.y + p2.y + p3.y);
    
    n += 3;

    pts.push_back(p1);
    pts.push_back(p2);
    pts.push_back(p3);

    Vec2i v1 = p3 - p1;
    Vec2i v2 = p3 - p2;

    float dot = v1[0] * v2[0] + v1[1] * v2[1];
    float det = v1[0] * v2[1] + v1[1] * v2[0];

    float angle = atan2(det, dot) * 57.2958;
    Vec2i biggest;
    if(cv::norm(v1) > cv::norm(v2)) {
      biggest = v1;
    } else {
      biggest = v2;
    }

    // circle( frame, p1, 4, Scalar(100,0,255), 2 );
    // circle( frame, p2, 4, Scalar(100,0,255), 2 );
    // circle( frame, p3, 4, Scalar(100,0,255), 2 );

    // if(abs(cv::norm(v1) - cv::norm(v2)) < cv::norm(biggest) * 0.3) {
    if(abs(angle) < 60 && abs(cv::norm(v1) - cv::norm(v2)) < cv::norm(biggest) * 0.3) {
    // if(depth > 10 && depth < 180) {
	line( frame, p1, p3, CV_RGB(0,255,0), 2 );
	line( frame, p2, p3, CV_RGB(0,255,0), 2 );
	circle( frame, p3, 4, Scalar(100,0,255), 2 );
	std::stringstream ss;
	ss << (int) angle;
	fingerCount += 1;
	putText(frame, ss.str(), p3, FONT_HERSHEY_PLAIN, 1,  Scalar(0,0,255,255));

    }
  }

  fingerCount += (fingerCount != 0);

  center.x /= n;
  center.y /= n;
  std::stringstream ss;
  ss << fingerCount;
  
  putText(frame, ss.str(), Point(20, 20), FONT_HERSHEY_PLAIN, 1,  Scalar(0,0,255,255));

    
  return fingerCount;
}

void HandDetection::extractHand(Mat frame) {
  int contoursWithSmallerStdDev = 0;
  double stdDev = 255;
  
  for( int i = 0; i < 2; i++ ) {
    Mat mask = Mat::zeros( frame.size(), CV_8UC3 );
    if(contours.size() <= i) return;
    drawContours( mask, contours, i, Scalar(255, 255, 255), CV_FILLED, 8, vector<Vec4i>(), 0, Point() );
    std::vector<double> vr;
    std::vector<double> vg;
    std::vector<double> vb;

    for (int i = 0; i < frame.rows; i++) {
      for (int j = 0; j < frame.cols; j++) {
	Vec3b color = mask.at<Vec3b>(i, j);
	Vec3b pixel = frame.at<Vec3b>(i, j);
	if(color[0] + color[1] + color[2] != 0) {
	  vr.push_back(pixel[0]);
	  vg.push_back(pixel[1]);
	  vb.push_back(pixel[2]);
	}
      }
    }
    double tmpStdDev = 0;
    tmpStdDev += vectorStdDev(vr);
    tmpStdDev += vectorStdDev(vg);
    tmpStdDev += vectorStdDev(vb);

    tmpStdDev /= vr.size();

    if(tmpStdDev < stdDev) {
      stdDev = tmpStdDev;
      contoursWithSmallerStdDev = i;
    }
  }

  contours[0] = contours[contoursWithSmallerStdDev];
  contours.resize(1);
}
