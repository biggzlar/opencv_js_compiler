
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

string cv_threshold(int height, int width, string img_data) {
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

string cv_blur(int height, int width, string img_data) {
  // returns a blurred mt-string
  cv::Mat img = js_string_to_mat(height, width, img_data);
  cv::Mat processed_mat;

  blur(img, processed_mat, cv::Size(16, 16));

  mat_info(processed_mat);

  string mat_string = mat_to_js_string(processed_mat);
  return mat_string;
}

string cv_range(int height, int width, string img_data) {
  // returns a color thresholded mat-string
  cv::Mat img = js_string_to_mat(height, width, img_data);
  cv::Mat rgb_img, hsv_img, processed_mat, lower_hue, upper_hue, mask;

  cvtColor(img, rgb_img, CV_RGBA2RGB);
  cvtColor(rgb_img, hsv_img, CV_RGB2HSV);

  cv::inRange(hsv_img, cv::Scalar(0, 45, 120), cv::Scalar(30, 200, 255), lower_hue);
  cv::inRange(hsv_img, cv::Scalar(150, 45, 120), cv::Scalar(180, 200, 255), upper_hue);
  cv::addWeighted(lower_hue, 1.0, upper_hue, 1.0, 0.0, processed_mat);

  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(11,11));
  cv::erode(processed_mat, mask, kernel);
  cv::dilate(mask, mask, kernel);
  // cvtColor(processed_mat, processed_rgba, CV_GRAY2RGBA);
  // blur(processed_mat, mask, cv::Size(2, 2));
  cv::Mat output;
  img.copyTo(output, mask);

  mat_info(output);

  string mat_string = mat_to_js_string(output);
  return mat_string;
}

void print_file(string file_path) {
    ifstream input_file;
    input_file.open(file_path);
    string input;

    while(!input_file.eof()) {
      input_file >> input;
      cout << input << ' ';
    }

    cout << endl;
    input_file.close();
}

void text_append(string file_path, string text) {
  ofstream output_file;
  output_file.open(file_path, std::ios_base::app | std::ios_base::out);
  output_file << text << "\n";
  output_file.close();
}

void create_print(string file_path, string text) {
  // writes a .txt file to the virtual filesystem, then reads it via c++
  EM_ASM (
    try {
      FS.open('/persistent_dir/file.txt');
    } catch(e) {
      console.log('I shouldnt be seeing this more than once.', e);
      FS.writeFile('/persistent_dir/file.txt', 'Hello world! How are you doing today?');
    }
  );

  text_append(file_path, text);

  //  'push' our changes to the persistent source
  EM_ASM (
    Module.syncdone = 0;
    FS.syncfs(false, function(err) {
      console.log('Changes synced.');
      Module.syncdone = 1;
    });
  );
}

void initialize_idbfs () {
  EM_ASM (
  	// set up a directory
    FS.mkdir('/persistent_dir');

    // mount filesystem IDBFS at directory 'persistent_dir'
  	FS.mount(IDBFS,{},'/persistent_dir');

  	// first argument 'populate':
  	// true = stored->current; // populates emscripten filesystem data with filesystem data from persistent source
  	// false= current->stored; // store emscripten filesystem data at persistent source
  	// true here to initialize our filesystem with the newly created folder
    FS.syncfs(true, function(err) {
  	   assert(!err);
       console.log('IDBFS initialized.')
    });
  );
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("print_file", &print_file);
    emscripten::function("initialize_idbfs", &initialize_idbfs);
    emscripten::function("process_appendix", &create_print);
    emscripten::function("cv_range", &cv_range);
    emscripten::function("cv_blur", &cv_blur);
    emscripten::function("cv_threshold", &cv_threshold);
}
