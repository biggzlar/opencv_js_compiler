
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <emscripten.h>

using namespace std;

cv::Mat test_img(const int height, const int width) {
  // Creates a test image of a given size height*width
  cv::Mat img = cv::Mat::zeros(height, width, CV_8UC3);
  for(int i = 0; i < height; i++){
    for(int j = 0; j < height; j++){
      img.at<cv::Vec3b>(i, j)[2] = 255;
    }
  }
  return img;
}

string type2str(int type) {
  // Returns a human readable string of the input image type
  string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }
  r += "C";
  r += (chans+'0');

  return r;
}

void mat_info(const cv::Mat img) {
  // prints some mat info to the console
  cout << "--- MAT INFO ---" << endl;
  cout << "SIZE: " << img.cols << " * " << img.rows << endl;
  cout << "TYPE: " << type2str(img.type()) << endl;
  cout << "CHANNELS: " << img.channels() << endl;
  cout << "---          ---" << endl;
}

extern "C"
{
  void EMSCRIPTEN_KEEPALIVE cv_threshold(int height, int width, char* src, char* dst)
  {
    cv::Mat img(height, width, CV_8UC4, src);
    cv::Mat processed_mat;
    cv::Mat output(height, width, CV_8UC4, dst);

    cvtColor(img, processed_mat, CV_RGBA2GRAY);
    cvtColor(processed_mat, processed_mat, CV_GRAY2RGBA);
    threshold(processed_mat, output, 97, 255, 0);
    // mat_info(output);
  }

  // returns a blurred mat-string
  void EMSCRIPTEN_KEEPALIVE cv_blur(int height, int width, char* src, char* dst)
  {
    cv::Mat img(height, width, CV_8UC4, src);
    cv::Mat output(height, width, CV_8UC4, dst);
    blur(img, output, cv::Size(16, 16));
    // mat_info(output);
  }

  void EMSCRIPTEN_KEEPALIVE cv_range(int height, int width, char* src, char* dst)
  {
    // returns a color thresholded mat-string
    cv::Mat rgb_img, hsv_img, processed_mat, lower_hue, upper_hue, mask;
    cv::Mat img(height, width, CV_8UC4, src);

    cvtColor(img, rgb_img, CV_RGBA2RGB);
    cvtColor(rgb_img, hsv_img, CV_RGB2HSV);

    cv::inRange(hsv_img, cv::Scalar(0, 45, 120), cv::Scalar(30, 200, 255), lower_hue);
    cv::inRange(hsv_img, cv::Scalar(150, 45, 120), cv::Scalar(180, 200, 255), upper_hue);
    cv::addWeighted(lower_hue, 1.0, upper_hue, 1.0, 0.0, processed_mat);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(11,11));
    cv::erode(processed_mat, mask, kernel);
    cv::dilate(mask, mask, kernel);
    blur(mask, mask, cv::Size(4, 4));

    cv::Mat output(height, width, CV_8UC4, dst);
    img.copyTo(output, mask);
    output.setTo(cv::Scalar(0,0,0,255), ~mask);
    // mat_info(output);
  }
}
