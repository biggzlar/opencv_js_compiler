
#include <iostream>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <emscripten/bind.h>

using namespace std;

int cool()
{
      cout << "something" << endl;
      cv::Mat img = cv::imread("image1.jpg");

      cout << "It does compile" << endl;
      cout << img.rows << endl;
      cout << img.cols << endl;
      return img.rows;

       //
      //  Mat gray_image;
      //  cvtColor( img, gray_image, CV_BGR2GRAY );
      //  imwrite("../gray_image.jpg", gray_image);
      //  IplImage opencvimg2 = (IplImage)gray_image;
       //
      //  // SimpleBlobDetector detector;
      //  // Set up the detector with default parameters.
       //
      //  // Detect blobs.
      //  vector<KeyPoint> keypoints;
      //  Ptr<SimpleBlobDetector> ptrSBD = SimpleBlobDetector::create();
      //  ptrSBD->detect(gray_image, keypoints);
       //
      //  cout << "We done!" << endl;
      //  // for (vector<KeyPoint>::const_iterator i = keypoints.begin(); i != keypoints.end(); ++i)
      //  // cout << (float)keypoints.at<float>(i, 0);
      //  return 0;
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("cool", &cool);
}
