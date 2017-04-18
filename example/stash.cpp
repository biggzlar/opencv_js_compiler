
#include <iostream>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
// #include <emscripten/bind.h>

using namespace std;

string mat_to_array(int width, int height, string img_data){
  vector<uchar> img_vector(img_data.begin(), img_data.end());
  cv::Mat img = cv::Mat(height, width, CV_8UC3, &img_vector);

  cout << "CHANNELS: " << img.channels() << endl;
  cout << "SIZE: " << img.size() << endl;
  cout << "TYPE: " << img.type() << endl;
  cout << "*img.ptr: " <<*img.ptr<uchar*>(0) << endl;

  cv::Mat gray_image;
  if(img.empty()){
    cout << "MEH" << endl;
    return img_data;
  } else if (img.channels() > 1){
    cout << "CVTCOLOR" << endl;
    // cvtColor(img, gray_image, cv::COLOR_BGR2GRAY);
    // blur(img, img, img.size());
  } else {
    cout << "FINALLY" << endl;
    gray_image = img;
  }

  cout << "*img.ptr: " <<*img.ptr<uchar*>(0) << endl;
  return img_data; // str;
}

int* mat_array(const cv::Mat img) {
  int input[img.cols * img.rows];
  for (int i = 0, j = 0; i < sizeof(input); i += img.channels(), j +=4){
    input[j] = (int)img.data[i];
    input[j + 1] = (int)img.data[i+1%img.channels()];
    input[j + 2] = (int)img.data[i+2%img.channels()];
    input[j + 3] = 255;
  }
  return input;
}

string mat_string(int width, int height, string img_data){
  std::vector<uint8_t> img_vector(img_data.begin(), img_data.end());
  cv::Mat img = cv::Mat(height, width, CV_8UC1, &img_vector);
  ostringstream ss;
  int* img_array = mat_array(img);
  copy(img_array, img_array+(img.rows * img.cols * img.channels()), ostream_iterator<int>(ss, ","));
  return ss.str();
}

string mat_to_string(cv::Mat img) {
  ostringstream ss;
  int* img_array = mat_array(img);
  copy(img_array, img_array + (img.rows * img.cols * img.channels()), ostream_iterator<int>(ss, ","));
  return ss.str();
}

std::vector<int> mat_vector(const cv::Mat img){
  std::vector<int> input(img.cols * img.rows);
  cout << "SIZE OF INPUT: " << input.size() << endl;
  for (int i = 0, j = 0; j < input.size() - 4; i += img.channels(), j +=4){
    input[j] = (int)img.data[i];
    input[j + 1] = (int)img.data[i+1%img.channels()];
    input[j + 2] = (int)img.data[i+2%img.channels()];
    input[j + 3] = 255;
  }
  return input;
}

string vector_to_string(std::vector<int> img_vector){
  //clumsy
  string returnstring = "";
  for(int i = 0; i < img_vector.size(); i++){
    returnstring += to_string(img_vector[i]);
    returnstring += ',';
  }
  return returnstring;
}

cv::Mat string_to_mat(const int height, const int width, const std::string img_data){
  std:vector<uchar>img_vector( img_data.begin(), img_data.end());
  return cv::Mat(height, width, CV_8UC3, &img_vector);
}

std::string v_to_s(const std::vector<int> img_vector) {
  //this needs to be split by ' ' in js
  std::stringstream result;
  std::copy(img_vector.begin(), img_vector.end(), std::ostream_iterator<int>(result, " "));
  return result.str();
}

int main(){
  cv::Mat img = cv::imread("Users/lbramlage/opencvjs/image1.jpg");
  cout << "It does compile" << endl;
  cout << "Rows: " << img.rows << endl;
  cout << "Columns: " << img.cols << endl;
  cout << "Channels: " << img.channels() << endl;

  std::vector<int> img_int = mat_vector(img);
  std::string vector_string = vector_to_string(img_int);
  cout << "vector to string: " << vector_string << endl;
  cv::Mat return_mat = string_to_mat(img.rows, img.cols, vector_string);

  // std::vector<int> img_array = mat_vector(img);
  // string img_data_string = vector_to_string(img_array);
  // cout << img_data_string << endl;

  // cv::Mat gray_image;
  // cvtColor(img, gray_image, cv::COLOR_BGR2GRAY);

  cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
  cv::imshow( "Display window", return_mat );

  cv::waitKey(0);
  return 0;
}


//
// EMSCRIPTEN_BINDINGS(my_module) {
//     // emscripten::function("main", &main);
//     emscripten::function("mat_string", &mat_string);
//     emscripten::function("mat_to_array", &mat_to_array);
// }
