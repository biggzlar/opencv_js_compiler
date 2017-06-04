# opencv project to javascript compiler

Forked from [opencvjs](https://github.com/ucisysarch/opencvjs).

### How to Build
1. Get the source code

  ```
  git clone https://github.com/biggzlar/opencv_js_compiler.git
  cd opencv_js_compiler
  git clone https://github.com/opencv/opencv
  ```
2. Install emscripten. You can obtain emscripten by using [Emscripten SDK](https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html).

  ```
  ./emsdk update
  ./emsdk install latest
  ./emsdk activate latest
  source ./emsdk_env.sh
  ```
3. Compile OpenCV and your source file.

  ```
    python n_make.py example/example.cpp
  ```
4. Add the compiled javascript module to your ´.html´ file.

´´´
  <script type='text/javascript'>
    var Module = {
      preRun: [],
      postRun: []
    };
  </script>
  <script async src='example.js'></script>
´´´

### ToDos:
* Skip the building of OpenCV once it has already been built or allow path specification?
* Allow for multiple input files.
