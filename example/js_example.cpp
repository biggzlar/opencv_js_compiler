
#include <iostream>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <emscripten/bind.h>

using namespace std;

string int_array_to_string(int int_array[], int size_of_array) {
  string returnstring = "";
  for (int temp = 0; temp < size_of_array; temp++) {
    returnstring += to_string(int_array[temp]);
    returnstring += ',';
  }
  return returnstring;
}

cv::Mat test_img(const int height, const int width) {
  cv::Mat img = cv::Mat::zeros(height, width, CV_8UC3);
  for(int i = 0; i < height; i++){
    for(int j = 0; j < height; j++){
      img.at<cv::Vec3b>(i, j)[2] = 255;
    }
  }
  return img;
}

std::vector<int> mat_vector(const cv::Mat img){
  // assumed it should be (img.cols * img.rows * img.channels()) but js wants a
  // 4th value (255 for alpha channel?) this could be problematic with mat computations
  // we might need to add this in at a later point (string generation maybe)

  // this is where image data is submitted from context.createimagedata(), so it
  // probably, actually has 4 channels?
  std::vector<int> input(img.cols * img.rows * 4);
  // cout << "IMAGE COLS: " << img.cols << endl;
  // cout << "IMAGE ROWS: " << img.rows << endl;
  // cout << "IMAGE CHANNELS: " << img.channels() << endl;
  // cout << "SIZE OF INPUT: " << input.size() << endl;

  // this will be the most difficult to get straight with the varying number of
  // channels
  for (int i = 0, j = 0; j < input.size() - 4; i += 4, j += 4){
    input[j] = (int)img.data[i];
    input[j + 1] = (int)img.data[i+1%4];
    input[j + 2] = (int)img.data[i+2%4];
    input[j + 3] = 255;
  }
  return input;
}

cv::Mat string_to_mat(int height, int width, std::string img_data){
  // splits up a ',' delimited string into a vector of unsigned chars, then
  // attempts to create a cv::Mat from it

  // img_data / height / width should be the number of input channels, no?
  vector<uchar> img_vector;
  std::stringstream ss(img_data);

  int i;
  while (ss >> i) {
    img_vector.push_back(i);

    if (ss.peek() == ',')
        ss.ignore();
  }

  int number_of_channels = img_vector.size() / height / width;
  cout << "IMG_VECTOR.SIZE(): " << img_vector.size() << endl;
  cout << "CHANNEL GUESS: " << number_of_channels << endl;

  cv::Mat img = cv::Mat(height, width, CV_8UC4, img_vector.data());
  return img;
}

std::string vector_to_string(const std::vector<int> img_vector) {
  // this needs to be split by ',' in js
  std::stringstream result;
  std::copy(img_vector.begin(), img_vector.end(), std::ostream_iterator<int>(result, ","));
  return (result.str()).substr(0, (result.str()).size() - 1);
}

string type2str(int type) {
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

std::string eternal_return(int height, int width, std::string img_data) {
  cv::Mat img = string_to_mat(height, width, img_data);
  cv::Mat processed_mat;

  blur(img, processed_mat, cv::Size(16,16));
  // cvtColor(img, processed_mat, CV_RGBA2GRAY);
  // cvtColor(processed_mat, img, CV_GRAY2RGBA);
  cout << "PROCESSED MAT" << endl;
  cout << "TYPE: " << type2str(processed_mat.type()) << endl;
  cout << "CHANNELS: " << processed_mat.channels() << endl;

  std::string mat_string = vector_to_string(mat_vector(processed_mat));

  return mat_string;
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("eternal_return", &eternal_return);
}
