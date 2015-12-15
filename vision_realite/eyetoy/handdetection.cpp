#include "handdetection.hpp"

double distance(Vec3b v1, Vec3b v2) {
  Vec3b v = v1 - v2;
  return cv::norm(v);
}

HandDetection::HandDetection(int rows, int cols) {
  tempBackgroundForSum = Mat(rows, cols, CV_32SC3);
  background = Mat(rows, cols, CV_8UC3);
  backgroundMask = Mat(rows, cols, CV_8UC1);
  n = 0;
  frameCount = 0;
  face = new Rect(0, 0, 0, 0);
  std::cout << face_cascade.load("/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml") << std::endl;

  fileHand = imread("./hands.jpg");
  cvtColor(fileHand, fileHand, CV_RGB2YCrCb);
  cvtColor(fileHand, fileHand, CV_RGB2YCrCb);
  cvtColor(fileHand, fileHand, CV_RGB2YCrCb);
  fileBg = imread("./background.jpg");


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

  // Mat m = frame.clone();
  // for (int i = 0; i < frame.rows; i++) {
  //   for (int j = 0; j < frame.cols; j++) {
	
  //     Vec3b v = m.at<Vec3b>(i, j);

  //     v[0] = avgY;
  //     v[1] = avgCr;
  //     v[2] = avgCb;
  //     m.at<Vec3b>(i, j) = v;
  //   }
  // }
  // std::stringstream ss;
  // ss << this->n;
  // imwrite("./backgrounds/a" + ss.str() + ".jpg", m);

  colorSamples.push_back(Scalar(avgY, avgCr, avgCb));
}

void HandDetection::detect(Mat frame) {
  frameCount = (frameCount + 1) % 30;

  //detection de la tete
  if(frameCount == 1) {
    std::vector<Rect> faces;
    face_cascade.detectMultiScale( frame, faces, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
    if(faces.size() > 0) {
      for( int i = 0; i < faces.size(); i++ ) {
  	Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
  	ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
      }
      face->x = faces[0].x;
      face->y = faces[0].y;
      face->width = faces[0].width;
      face->height = faces[0].height;
    }
  }
  //soustraction de la tete pour ne pas la confondre avec la main
  if(face->width > 0) {
    for (int i = face->y; i < face-> y + face->height; i++) {
      for (int j = face->x; j < face->x + face->width; j++) {
  	Vec3b v = frame.at<Vec3b>(i, j);
  	v[0] = 0;
        v[1] = 0;
        v[2] = 0;
  	frame.at<Vec3b>(i, j) = v;
      }
    }
  }
  std::stringstream ss;
  ss << n;
  // imwrite( "./hands/hands" + ss.str() + ".jpg", frame);

  //le bruit se concentre sur Cr
  Mat temp = frame.clone();
  //lorsque l'on a suffisament d'exemple de couleurs pour la main
  if(colorSamples.size() > 20 && filteredColorSamples.size() == 0) {
    filterColorSamples();
    filteredColorSamples.resize(5);
  }

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

    // temp = fileHand.clone();
    // background = fileBg.clone();
    //calcul du mask
    Mat tempFrame = frame.clone();
    if(filteredColorSamples.size() == 0) {
      computeBackgroundMask(temp.clone(), 14, 14);
    } else {
      computeBackgroundMask(tempFrame, 14, 14);
    }
    Mat tempBackground = backgroundMask.clone();
    
    findContours( backgroundMask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_TC89_L1, Point(0, 0) );
    if(contours.size() > 0) {
      vector<vector<Point> >hull( contours.size() );
      vector<vector<int> >hullI( contours.size() );
      std::sort(contours.begin(), contours.end(), [](std::vector<Point> a, std::vector<Point> b) {
	  return a.size() > b.size();
	});
      //ne garder que le contour avec les plus de points pour éliminer le bruit
      contours.resize(1);

      if(contours[0].size() > 3) {
	for( int i = 0; i < contours.size(); i++ ) {
	  convexHull( Mat(contours[i]), hull[i], false );
	  convexHull( Mat(contours[i]), hullI[i], false ); 
	}

	Mat handMask = Mat::zeros( frame.size(), CV_8UC3 );
	
	//affichage des contours/enveloppes convexes
	for( int i = 0; i< contours.size(); i++ ) {
	  drawContours( frame, contours, i, Scalar(255, 0, 0), 3, 8, vector<Vec4i>(), 0, Point() );
	  drawContours( frame, hull, i, Scalar(255, 255, 255), 2, 8, vector<Vec4i>(), 0, Point() );
	  drawContours( handMask, contours, i, Scalar(255, 255, 255), CV_FILLED, 8, vector<Vec4i>(), 0, Point() );
	}
	defects.clear();
	defects.resize(contours.size());
  
	convexityDefects(Mat(contours[0]), hullI[0], defects[0]);

	if(containsHand(frame) && filteredColorSamples.size() == 0) {
	  // std::cout << "found hand!" << std::endl;
	  extractColorSamples(tempFrame, handMask);
	}

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

	namedWindow( "Hand", CV_WINDOW_AUTOSIZE );
	imshow( "Hand", frame);
	namedWindow( "Hand Mask", CV_WINDOW_NORMAL);
	imshow( "Hand Mask", handMask);
	resizeWindow("Hand Mask", frame.rows * 0.7, frame.rows * 0.7);

	namedWindow( "Background Mask", CV_WINDOW_NORMAL);
	imshow( "Background Mask", tempBackground);
	// imshow( "Background Mask", background);

	resizeWindow("Background Mask", frame.rows * 0.7, frame.rows * 0.7);

	// namedWindow( "y", CV_WINDOW_NORMAL);
	// imshow( "y", y);
	// resizeWindow("y", frame.rows * 0.7, frame.rows * 0.7);
	
	// namedWindow( "cr", CV_WINDOW_NORMAL);
	// imshow( "cr", cr);
	// resizeWindow("cr", frame.rows * 0.7, frame.rows * 0.7);

	// namedWindow( "cb", CV_WINDOW_NORMAL);
	// imshow( "cb", cb);
	// resizeWindow("cb", frame.rows * 0.7, frame.rows * 0.7);

	// extractColorSamples(temp, handMask);

	// imwrite( "./backgrounds/background" + ss.str() + ".jpg", background );
      }
    }
  }  
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
  if(filteredColorSamples.size() == 0) {
    for (int i = 0; i < frame.rows; i++) {
      for (int j = 0; j < frame.cols; j++) {
	Vec<uchar, 3> om = frame.at<Vec<uchar, 3> >(i, j);
	Vec<uchar, 3> cm = background.at<Vec<uchar, 3> >(i, j);
	if(abs(om[0] - cm[0]) < thy || abs(om[2] - cm[2]) < thcb) {
	  backgroundMask.at<uchar>(i, j) = 0;
	} else {
	  backgroundMask.at<uchar>(i, j) = 255;
	}
      }
    }
  } else {
    Mat m = backgroundMask.clone();
    for (int i = 0; i < frame.rows; i++) {
      for (int j = 0; j < frame.cols; j++) {
	backgroundMask.at<uchar>(i, j) = 0;
        m.at<uchar>(i, j) = 0;
	// Scalar s = filteredColorSamples[0];
	for(Scalar s : filteredColorSamples) {
	  Vec3b om = frame.at<Vec3b >(i, j);
	  Vec3b cm; cm[0] = s[0]; cm[1] = s[1]; cm[2] = s[2];
	  float dist = sqrt(pow(om[0] - cm[0], 2) + pow(om[1] - cm[1], 2) + pow(om[2] - cm[2], 2));
	  // if(distance(om, cm) < 35) {
	  if(dist < 25) {
	    // std::cout << distance(om, cm) << std::endl;
	    backgroundMask.at<uchar>(i, j) = 255;
	    // m.at<uchar>(i, j) = 255;
	  }
	}
      }
      // int random = std::rand() % filteredColorSamples.size();

      // for (int i = 0; i < frame.rows; i++) {
      // 	for (int j = 0; j < frame.cols; j++) {
      // 	  Vec3b v = m.at<Vec3b>(i, j);

      // 	  v[0] = filteredColorSamples[random][0];
      // 	  v[1] = filteredColorSamples[random][1];
      // 	  v[2] = filteredColorSamples[random][2];
      // 	  m.at<Vec3b>(i, j) = v;
      // 	}
      // }
      // std::stringstream ss;
      // ss << this->n;
      // imwrite("./backgrounds/out" + ss.str() + ".jpg", m);

    }
    // std::cout << filteredColorSamples.size() << std::endl;
  }
}

bool HandDetection::containsHand(Mat frame) {
  std::vector<Point> filtered;
  std::vector<Point> betweenFingers;
  float angle;
  
  for (int i = 0; i < defects[0].size(); i++) {

    Point p1 = contours[0][defects[0][i][1]];
    Point p2 = contours[0][defects[0][(i+1) % defects[0].size()][2]];
    Point p3 = contours[0][defects[0][i][2]];
  
    Point v1(p1 - p2);
    Point v2(p1 - p3);

    float len1 = sqrt(v1.x * v1.x + v1.y * v1.y);
    float len2 = sqrt(v2.x * v2.x + v2.y * v2.y);

    float dot = v1.x * v2.x + v1.y * v2.y;

    float a = dot / (len1 * len2);

    if (a >= 1.0)
      angle = 0.0;
    else if (a <= -1.0)
      angle = M_PI;
    else
      angle = acos(a) * 57,2958;

    std::ostringstream ss;
    ss << (int)angle;

    putText(frame, ss.str(), p1, FONT_HERSHEY_PLAIN, 1,  Scalar(0,0,255,255), 1);

    if(angle < 40) {
      filtered.push_back(p1);
      filtered.push_back(p2);
      filtered.push_back(p3);

      int thickness = 2;
      int lineType = 8;
      line( frame,
	    p1,
	    p2,
	    Scalar( 0, 255, 0 ),
	    thickness,
	    lineType );
    
      line( frame,
	    p1,
	    p3,
	    Scalar( 0, 255, 0 ),
	    thickness,
	    lineType );

      // cv::circle(frame,p1,3,Scalar(0,0,255),-1);
      cv::circle(frame,p2,3,Scalar(0,255,255),-1);
      cv::circle(frame,p3,5,Scalar(0,0,255),-1);

      // std::cout << filtered.size() << std::endl;
      betweenFingers.push_back(p3);
    }
  }

  if(betweenFingers.size() > 5) {
    RotatedRect r = fitEllipse(betweenFingers);
    cv::circle(frame,r.center,5,Scalar(0,255,255),-1);
  }

  return filtered.size() > 12;
}
