
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

  cout << "*img.ptr: " << *img.ptr<uchar*>(0) << endl;
  return img_data; // str;
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
  std::vector<int> input(img.cols * img.rows * img.channels());
  cout << "SIZE OF INPUT: " << input.size() << endl;
  for (int i = 0, j = 0; j < input.size() - img.channels(); i += img.channels(), j += img.channels()){
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
  //   cout << static_cast<unsigned>(img_vector[j]) << " ";
  // }
  cout << endl;
  cv::Mat img = cv::Mat(height, width, CV_8UC3, img_vector.data());
  return img;
}

std::string vector_to_string(const std::vector<int> img_vector) {
  // this needs to be split by ' ' in js
  // it should be js compatible
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

int main(){
  cv::imwrite("img_in.jpg", test_img(100, 100));
  cv::Mat img = cv::imread("image1.jpg", CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);

  cout << "Rows: " << img.rows << endl;
  cout << "Columns: " << img.cols << endl;
  cout << "Type: " << type2str(img.type()) << endl;
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
  blur(processed_mat, gray_image, cv::Size(16,16));

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
