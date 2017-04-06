
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
    cv::Mat img = cv::imread("image1.jpg");
    cout << "It does compile" << endl;
    cout << "Rows: " << img.rows << endl;
    cout << "Columns: " << img.cols << endl;
    return img.rows;
}

uint8_t put_picture(){
    cv::Mat img = cv::imread("image1.jpg");
    cout << "type of img.data: " << typeid(img.data).name() << endl;
    return *img.data;
}

string mat_stuff(int width, int height, string img_data){
    std::vector<uint8_t> img_vector(img_data.begin(), img_data.end());
    cv::Mat img = cv::Mat(height, width, CV_8UC1, &img_vector);

    cv::SimpleBlobDetector detector;
    // Detect blobs.
    vector<cv::KeyPoint> keypoints;
    cv::Ptr<cv::SimpleBlobDetector> ptrSBD =cv::SimpleBlobDetector::create();
    ptrSBD->detect(img, keypoints);
    cv::Mat im_with_keypoints;
    drawKeypoints( img, keypoints, im_with_keypoints, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
    return img_data;
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("cool", &cool);
    emscripten::function("mat_stuff", &mat_stuff);
    emscripten::function("put_picture", &put_picture, emscripten::allow_raw_pointers());
}
