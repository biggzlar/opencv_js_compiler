
#include <iostream>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <emscripten/bind.h>

using namespace std;

void cool(){
    cv::Mat img = cv::imread("image1.jpg");
    cout << "It does compile" << endl;
    cout << "Rows: " << img.rows << endl;
    cout << "Columns: " << img.cols << endl;
    cout << "Channels: " << img.channels() << endl;
}

uint8_t put_picture(){
    cv::Mat img = cv::imread("image1.jpg");
    cout << "type of img.data: " << typeid(img.data).name() << endl;
    return *img.data;
}

string int_array_to_string(int int_array[], int size_of_array) {
  string returnstring = "";
  for (int temp = 0; temp < size_of_array; temp++) {
    returnstring += to_string(int_array[temp]);
    returnstring += ',';
  }
  return returnstring;
}

string mat_to_array(int width, int height, string img_data){
  cout << "string passed to c++: " << img_data << endl;
  vector<uchar> img_vector(img_data.begin(), img_data.end());

  cv::Mat img = cv::Mat(height, width, CV_8UC1, &img_vector);
  int img_arr[img.rows * (img.cols*img.channels())];

  uchar* p;
  cout << "sizeof:" << sizeof(**img.ptr<uchar*>()) << endl;
  cout << typeid(*img.ptr<uchar*>(0,0)).name() << endl;

  cout << "**: " << **img.ptr<uchar*>(1) << endl;
  cout << "*: " << *img.ptr<uchar*>(0) << endl;

  string sName(reinterpret_cast<char*>(*img.ptr<uchar*>(0)));
  cout << sName << endl;

  // cout << "dereference pointer: " << *img.ptr<uchar*>(0, 0) << endl;
  // for (int i = 0; i < img.rows; ++i) {
  //     p = *img.ptr<uchar*>(i);
  //     for (int j = 0; j < img.cols*img.channels(); ++j) {
  //         img_arr[i * img.cols*img.channels() + j] = p[j];
  //         cout << p[j] << " ";
  //         // img_arr[i * img.cols*img.channels() + j + 1] = p[j + 1];
  //         // img_arr[i * img.cols*img.channels() + j + 2] = p[j + 2];
  //     }
  //     cout << endl;
  // }

  cout << "passed img's rows: " << img.rows << endl;
  cout << "passed img's cols: " << img.cols << endl;
  cout << "passed img's channels: " << img.channels() << endl;
  cout << "is continuous: " << img.isContinuous() << endl;
  // for(int i = 0; i < img.rows; i++) {
  //   for(int j = 0; j < img.cols; j++) {
  //     img_arr[(i * img.cols) + j] = (int)*img.at<uchar*>(i, j);
  //   }
  // }
  cout << int_array_to_string(img_arr, sizeof(img_arr)) << endl;
  // string str(img_vector.begin(), img_vector.end());
  return sName; // str;
}

int* mat_array(const cv::Mat img) {
  int input[img.cols * img.rows];
  unsigned char* shinput = (unsigned char *)(img.data);
  cout << "size of input: " << sizeof(input) << " channels: " << img.channels() << endl;
  for (int i = 0, j = 0; i < sizeof(input); i += img.channels(), j +=4){
    input[j] = (int)img.data[i];
    input[j + 1] = (int)img.data[i+1%img.channels()];
    input[j + 2] = (int)img.data[i+2%img.channels()];
    input[j + 3] = 255;
    // cout << input[j] << " " << input[j+1] << " " << input[j+2] << " " << input[j+3] << " " << endl;
  }
  return input;
}

string mat_stuff(int width, int height, string img_data){
  std::vector<uint8_t> img_vector(img_data.begin(), img_data.end());
  cv::Mat img = cv::Mat(height, width, CV_8UC1, &img_vector);
  cv::Mat dst, src_gray;
  //
  // cv::SimpleBlobDetector detector;
  // // Detect blobs.
  // vector<cv::KeyPoint> keypoints;
  // cv::Ptr<cv::SimpleBlobDetector> ptrSBD =cv::SimpleBlobDetector::create();
  // ptrSBD->detect(img, keypoints);
  // cv::Mat im_with_keypoints;
  // drawKeypoints( img, keypoints, im_with_keypoints, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
  // cv::cvtColor( img, src_gray, CV_BGR2GRAY );
  // cv::threshold( img, dst, 0.8, 1.2, 1 );

  ostringstream ss;
  int* img_array = mat_array(img);
  copy(img_array, img_array+(img.rows * img.cols * img.channels()), ostream_iterator<int>(ss, ","));
  return ss.str();
}


EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("cool", &cool);
    emscripten::function("mat_stuff", &mat_stuff);
    emscripten::function("mat_to_array", &mat_to_array, emscripten::allow_raw_pointers());
    emscripten::function("put_picture", &put_picture, emscripten::allow_raw_pointers());
}
