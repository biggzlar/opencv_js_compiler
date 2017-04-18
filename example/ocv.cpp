
#include <iostream>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
// #include <emscripten/bind.h>

using namespace std;

string int_array_to_string(int int_array[], int size_of_array) {
  string returnstring = "";
  for (int temp = 0; temp < size_of_array; temp++) {
    returnstring += to_string(int_array[temp]);
    returnstring += ',';
  }
  return returnstring;
}

string mat_to_array(int width, int height, string img_data){
  vector<uchar> img_vector;
  std::stringstream ss(img_data);
  int i;
  while (ss >> i) {
      img_vector.push_back(i);

      if (ss.peek() == ',')
          ss.ignore();
  }
  cout << img_vector[2] << endl;
  cv::Mat img = cv::Mat(height, width, CV_8UC3, &img_vector);

  // cout << "CHANNELS: " << img.channels() << endl;
  // cout << "SIZE: " << img.size() << endl;
  // cout << "TYPE: " << img.type() << endl;
  cout << "*img.ptr: " << *img.ptr<uchar*>(0) << endl;
  //
  // cv::Mat gray_image;
  // if(img.empty()){
  //   cout << "MEH" << endl;
  //   return img_data;
  // } else if (img.channels() > 1){
  //   cout << "CVTCOLOR" << endl;
  //   // cvtColor(img, gray_image, cv::COLOR_BGR2GRAY);
  //   // blur(img, img, img.size());
  // } else {
  //   cout << "FINALLY" << endl;
  //   gray_image = img;
  // }
  return img_data; // str;
}

// int* mat_array(const cv::Mat img) {
//   int input[img.cols * img.rows];
//   for (int i = 0, j = 0; i < sizeof(input); i += img.channels(), j +=4){
//     input[j] = (int)img.data[i];
//     input[j + 1] = (int)img.data[i+1%img.channels()];
//     input[j + 2] = (int)img.data[i+2%img.channels()];
//     input[j + 3] = 255;
//   }
//   return input;
// }
//
// string mat_string(int width, int height, string img_data){
//   std::vector<uint8_t> img_vector(img_data.begin(), img_data.end());
//   cv::Mat img = cv::Mat(height, width, CV_8UC1, &img_vector);
//   ostringstream ss;
//   int* img_array = mat_array(img);
//   copy(img_array, img_array+(img.rows * img.cols * img.channels()), ostream_iterator<int>(ss, ","));
//   return ss.str();
// }
//
// string mat_to_string(cv::Mat img) {
//   ostringstream ss;
//   int* img_array = mat_array(img);
//   copy(img_array, img_array + (img.rows * img.cols * img.channels()), ostream_iterator<int>(ss, ","));
//   return ss.str();
// }

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
  std::vector<int> input(img.cols * img.rows);
  cout << "SIZE OF INPUT: " << input.size() << endl;
  for (int i = 0, j = 0; j < input.size() - 3; i += img.channels(), j +=3){
    input[j] = (int)img.data[i];
    input[j + 1] = (int)img.data[i+1%img.channels()];
    input[j + 2] = (int)img.data[i+2%img.channels()];
  }
  return input;
}

cv::Mat string_to_mat(int height, int width, std::string img_data){
  vector<uchar> img_vector;
  std::stringstream ss(img_data);
  int i;
  while (ss >> i) {
    img_vector.push_back(i);

    if (ss.peek() == ',')
        ss.ignore();
  }
  // for(int j = 0; j < img_vector.size(); j++){
  //   cout << (int)img_vector[j] << endl;
  // }
  cv::Mat img = cv::Mat(height, width, CV_8UC3, img_vector.data());
  // memcpy(img.data, img_vector.data(), img_vector.size()*sizeof(uchar));
  return img;
}

std::string vector_to_string(const std::vector<int> img_vector) {
  // this needs to be split by ' ' in js
  // it should be js compatible
  std::stringstream result;
  std::copy(img_vector.begin(), img_vector.end(), std::ostream_iterator<int>(result, ","));
  return (result.str()).substr(0, (result.str()).size() - 1);
}

int main(){
  cv::imwrite("img_in.jpg", test_img(1000, 1000));
  cv::Mat img = cv::imread("img_in.jpg", CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);

  cout << "Rows: " << img.rows << endl;
  cout << "Columns: " << img.cols << endl;
  cout << "Channels: " << img.channels() << endl;

  // img to vector
  std::vector<int> img_vector = mat_vector(img);

  // vector to string
  string img_data_string = vector_to_string(img_vector);

  // string to mat
  cv::Mat processed_mat = string_to_mat(img.rows, img.cols, img_data_string);

  // save mat to file
  cv::imwrite("img_out.jpg", processed_mat);

  cv::Mat gray_image;
  cvtColor(processed_mat, gray_image, CV_BGR2GRAY);

  cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
  cv::imshow( "Display window", gray_image );

  cv::waitKey(0);
  return 0;
}

//
// EMSCRIPTEN_BINDINGS(my_module) {
//     // emscripten::function("main", &main);
//     emscripten::function("mat_string", &mat_string);
//     emscripten::function("mat_to_array", &mat_to_array);
//     emscripten::function("put_picture", &put_picture, emscripten::allow_raw_pointers());
// }
