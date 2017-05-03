
#include <iostream>
#include <stdio.h>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <emscripten/bind.h>
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

string mat_to_js_string(const cv::Mat img) {
  // Casts the pixel data of a cv::Mat object to a vector of integers and returns
  // a string representation of that vector
  vector<int> img_vector(img.cols * img.rows * 4);

  for (int i = 0, j = 0; j < img_vector.size() - 4; i += 4, j += 4){
    img_vector[j]     = (int)img.data[i];
    img_vector[j + 1] = (int)img.data[i+1%4];
    img_vector[j + 2] = (int)img.data[i+2%4];
    img_vector[j + 3] = 255;
  }

  stringstream result;
  copy(img_vector.begin(), img_vector.end(), ostream_iterator<int>(result, ","));
  return (result.str()).substr(0, (result.str()).size() - 1);
}

cv::Mat js_string_to_mat(int height, int width, string img_data){
  // Splits up a ',' delimited string into a vector of unsigned chars, then
  // creates a cv::Mat object from it, using the provided height and width
  vector<uchar> img_vector;
  stringstream ss(img_data);

  int i;
  while (ss >> i) {
    img_vector.push_back(i);

    if (ss.peek() == ',')
        ss.ignore();
  }

  cv::Mat img = cv::Mat(height, width, CV_8UC4, img_vector.data());
  return img;
}

void mat_info(const cv::Mat img) {
  // prints some mat info to the console
  cout << "--- MAT INFO ---" << endl;
  cout << "SIZE: " << img.cols << " * " << img.rows << endl;
  cout << "TYPE: " << type2str(img.type()) << endl;
  cout << "CHANNELS: " << img.channels() << endl;
  cout << "---          ---" << endl;
}

string js_threshold(int height, int width, string img_data) {
  // returns a thresholded mat-string
  cv::Mat img = js_string_to_mat(height, width, img_data);
  cv::Mat processed_mat, final_mat;

  cvtColor(img, processed_mat, CV_RGBA2GRAY);
  cvtColor(processed_mat, processed_mat, CV_GRAY2RGBA);
  threshold(processed_mat, final_mat, 97, 255, 0);

  mat_info(final_mat);

  string mat_string = mat_to_js_string(final_mat);
  return mat_string;
}

string js_blur(int height, int width, string img_data) {
  // returns a blurred mt-string
  cv::Mat img = js_string_to_mat(height, width, img_data);
  cv::Mat processed_mat;

  blur(img, processed_mat, cv::Size(16, 16));

  mat_info(processed_mat);

  string mat_string = mat_to_js_string(processed_mat);
  return mat_string;
}

string js_range(int height, int width, string img_data) {
  // returns a color thresholded mat-string
  cv::Mat img = js_string_to_mat(height, width, img_data);
  cv::Mat rgb_img, hsv_img, processed_mat, lower_hue, upper_hue;

  cvtColor(img, rgb_img, CV_RGBA2RGB);
  cvtColor(rgb_img, hsv_img, CV_RGB2HSV);

  cv::inRange(hsv_img, cv::Scalar(0, 100, 100), cv::Scalar(18, 255, 255), lower_hue);
  cv::inRange(hsv_img, cv::Scalar(162, 100, 100), cv::Scalar(180, 255, 255), upper_hue);
  cv::addWeighted(lower_hue, 1.0, upper_hue, 1.0, 0.0, processed_mat);

  cv::Mat output;
  img.copyTo(output, processed_mat);
  // cvtColor(processed_mat, output, CV_GRAY2RGBA);
  mat_info(output);

  string mat_string = mat_to_js_string(output);
  return mat_string;
}

void save_to_idb() {

}

void cool_function() {
  ifstream input_file;
  input_file.open("/persistent_dir/file.txt");
  string input;

  while(!input_file.eof()) {
    input_file >> input;
    cout << input << ' ';
  }

  cout << endl;
  input_file.close();

  ofstream output_file;
  output_file.open("/persistent_dir/file.txt", std::ios_base::app | std::ios_base::out);
  output_file << "ᕕ( ᐛ )ᕗ \n";
  output_file.close();
}


void create_print() {
  // writes a .txt file to the virtual filesystem, then reads it via c++
  EM_ASM (
    try {
      FS.open('/persistent_dir/file.txt');
    } catch(e) {
      console.log('I shouldnt be seeing this more than once.', e);
      FS.writeFile('/persistent_dir/file.txt', 'Hello world! How are you doing today?');
    }
  );

  cool_function();

  EM_ASM (
    Module.syncdone = 0;
    FS.syncfs(false, function(err) {
      console.log('DONE');
      Module.syncdone = 1;
    });
  );
}

void initialize_idbfs() {
  EM_ASM(
    try {
      FS.mkdir('/persistent_dir');
    } catch(e) {
    }
    try {
      FS.mount(IDBFS,{},'/persistent_dir');
    } catch(e) {
    }

    FS.syncfs(true, function(err) {
      console.log('DONE');
    });
  );
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("initialize_idbfs", &initialize_idbfs);
    emscripten::function("create_print", &create_print);
    emscripten::function("js_range", &js_range);
    emscripten::function("js_blur", &js_blur);
    emscripten::function("js_threshold", &js_threshold);
}
