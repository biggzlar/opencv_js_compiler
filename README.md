# opencv to javascript compiler

### How to Build
1. Get the source code

  ```
  git clone https://github.com/ucisysarch/opencv_js_compiler.git
  cd opencvjs
  git clone https://github.com/opencv/opencv
  cd opencv
  git checkout 3.1.0
  ```
2. Install emscripten. You can obtain emscripten by using [Emscripten SDK](https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html).

  ```
  ./emsdk update
  ./emsdk install sdk-master-64bit --shallow
  ./emsdk activate sdk-master-64bit
  source ./emsdk_env.sh
  ```
3. Rebuild emscripten
  ```
  ./emsdk install sdk-master-64bit --shallow
  ```

4. Compile OpenCV and generate bindings by executing n_make.py script.

  ```
    python n_make.py input.cpp
  ```
