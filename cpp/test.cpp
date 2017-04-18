#include <iostream>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;

using namespace std;

int main () {
   int  var = 20;   // actual variable declaration.
   int  *ip;        // pointer variable
   ip = &var;       // store address of var in pointer variable
    
    cv::imread("_aaa.jpg");

   cout << " var: " << var << endl;
   cout << "&var: " << &var << endl;
   cout << " ip : " << ip << endl;
   cout << "*ip : " << *ip << endl;
    
    

   return 0;
}
