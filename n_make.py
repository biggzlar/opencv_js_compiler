#!/usr/bin/python
import os
import sys
import re
import json
import shutil
from subprocess import Popen, PIPE, STDOUT

# Startup
exec(open(os.path.expanduser('~/.emscripten'), 'r').read())

try:
    EMSCRIPTEN_ROOT
except:
    print "ERROR: Missing EMSCRIPTEN_ROOT (which should be equal to emscripten's root dir) in ~/.emscripten"
    sys.exit(1)

sys.path.append(EMSCRIPTEN_ROOT)
import tools.shared as emscripten

emcc_args = '-O3 --llvm-lto 1 --bind -s ASSERTIONS=2 --memory-init-file 0'.split(
    ' ')


print
print '--------------------------------------------------'
print 'Building opencv.js, build type:', emcc_args
print '--------------------------------------------------'
print

stage_counter = 0


def stage(text):
    global stage_counter
    stage_counter += 1
    text = 'Stage %d: %s' % (stage_counter, text)
    print
    print '=' * len(text)
    print text
    print '=' * len(text)
    print


class bcolors:
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    ENDC = '\033[0m'


def success(text):
    return bcolors.OKGREEN + text + bcolors.ENDC

# Main
try:
    this_dir = os.getcwd()
    os.chdir('opencv')
    if not os.path.exists('build'):
        os.makedirs('build')
    os.chdir('build')
    input_file_path = os.path.abspath(os.path.join(this_dir, sys.argv[1]))

    stage('OpenCV Configuration')
    configuration = ['cmake',
                     '-DCMAKE_BUILD_TYPE=RELEASE',
                     '-DBUILD_DOCS=OFF',
                     '-DBUILD_EXAMPLES=OFF',
                     '-DBUILD_PACKAGE=OFF',
                     '-DBUILD_WITH_DEBUG_INFO=OFF',
                     '-DBUILD_opencv_bioinspired=OFF',
                     '-DBUILD_opencv_calib3d=OFF',
                     '-DBUILD_opencv_cuda=OFF',
                     '-DBUILD_opencv_cudaarithm=OFF',
                     '-DBUILD_opencv_cudabgsegm=OFF',
                     '-DBUILD_opencv_cudacodec=OFF',
                     '-DBUILD_opencv_cudafeatures2d=OFF',
                     '-DBUILD_opencv_cudafilters=OFF',
                     '-DBUILD_opencv_cudaimgproc=OFF',
                     '-DBUILD_opencv_cudaoptflow=OFF',
                     '-DBUILD_opencv_cudastereo=OFF',
                     '-DBUILD_opencv_cudawarping=OFF',
                     '-DBUILD_opencv_gpu=OFF',
                     '-DBUILD_opencv_gpuarithm=OFF',
                     '-DBUILD_opencv_gpubgsegm=OFF',
                     '-DBUILD_opencv_gpucodec=OFF',
                     '-DBUILD_opencv_gpufeatures2d=OFF',
                     '-DBUILD_opencv_gpufilters=OFF',
                     '-DBUILD_opencv_gpuimgproc=OFF',
                     '-DBUILD_opencv_gpuoptflow=OFF',
                     '-DBUILD_opencv_gpustereo=OFF',
                     '-DBUILD_opencv_gpuwarping=OFF',
                     '-BUILD_opencv_hal=OFF',
                     '-DBUILD_opencv_highgui=ON',
                     '-DBUILD_opencv_java=OFF',
                     '-DBUILD_opencv_legacy=OFF',
                     '-DBUILD_opencv_ml=ON',
                     '-DBUILD_opencv_nonfree=OFF',
                     '-DBUILD_opencv_optim=OFF',
                     '-DBUILD_opencv_photo=ON',
                     '-DBUILD_opencv_shape=ON',
                     '-DBUILD_opencv_objdetect=ON',
                     '-DBUILD_opencv_softcascade=ON',
                     '-DBUILD_opencv_stitching=OFF',
                     '-DBUILD_opencv_superres=OFF',
                     '-DBUILD_opencv_ts=OFF',
                     '-DBUILD_opencv_videostab=OFF',
                     '-DENABLE_PRECOMPILED_HEADERS=OFF',
                     '-DWITH_1394=OFF',
                     '-DWITH_CUDA=OFF',
                     '-DWITH_CUFFT=OFF',
                     '-DWITH_EIGEN=OFF',
                     '-DWITH_FFMPEG=OFF',
                     '-DWITH_GIGEAPI=OFF',
                     '-DWITH_GSTREAMER=OFF',
                     '-DWITH_GTK=OFF',
                     '-DWITH_JASPER=OFF',
                     '-DWITH_JPEG=ON',
                     '-DWITH_OPENCL=OFF',
                     '-DWITH_OPENCLAMDBLAS=OFF',
                     '-DWITH_OPENCLAMDFFT=OFF',
                     '-DWITH_OPENEXR=OFF',
                     '-DWITH_PNG=ON',
                     '-DWITH_PVAPI=OFF',
                     '-DWITH_TIFF=OFF',
                     '-DWITH_LIBV4L=OFF',
                     '-DWITH_WEBP=OFF',
                     '-DWITH_PTHREADS_PF=OFF',
                     '-DBUILD_opencv_apps=OFF',
                     '-DBUILD_PERF_TESTS=OFF',
                     '-DBUILD_TESTS=OFF',
                     '-DBUILD_SHARED_LIBS=OFF',
                     '-DWITH_IPP=OFF',
                     '-DCPU_BASELINE_DISABLE=SSE,SSE2,SSE41,SSE42,AVX,AVX2',
                     '-DCMAKE_CXX_FLAGS=%s' % ' '.join(emcc_args),
                     '-DCMAKE_EXE_LINKER_FLAGS=%s' % ' '.join(emcc_args),
                     '-DCMAKE_CXX_FLAGS_DEBUG=%s' % ' '.join(emcc_args),
                     '-DCMAKE_CXX_FLAGS_RELWITHDEBINFO=%s' % ' '.join(emcc_args),
                     '-DCMAKE_C_FLAGS_RELWITHDEBINFO=%s' % ' '.join(emcc_args),
                     '-DCMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO=%s' % ' '.join(emcc_args),
                     '-DCMAKE_MODULE_LINKER_FLAGS_RELEASE=%s' % ' '.join(emcc_args),
                     '-DCMAKE_MODULE_LINKER_FLAGS_DEBUG=%s' % ' '.join(emcc_args),
                     '-DCMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO=%s' % ' '.join(emcc_args),
                     '-DCMAKE_SHARED_LINKER_FLAGS_RELEASE=%s' % ' '.join(emcc_args),
                     '-DCMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO=%s' % ' '.join(emcc_args),
                     '-DCMAKE_SHARED_LINKER_FLAGS_DEBUG=%s' % ' '.join(emcc_args),
                     '..']
    emscripten.Building.configure(configuration)

    stage('Making OpenCV')
    # emcc_args += '-s EXPORT_NAME="cv" -s MODULARIZE=1'.split(' ')
    emscripten.Building.make(['make', '-j4'])

    print success('-> Done')

    stage('Compiling input file to .bc')
    emcc_args += ('-s TOTAL_MEMORY=%d' % (128 * 1024 * 1024)).split(' ')
    emcc_args += '-s ALLOW_MEMORY_GROWTH=1'.split(' ')  # resizable heap
    include_dirs = [
        os.path.join('..', 'include'),
        os.path.join('..', 'build'),
        os.path.join('..', 'modules', 'core', 'include', 'opencv', 'core'),
        os.path.join('..', 'modules', 'core', 'include'),
        os.path.join('..', 'modules', 'flann', 'include'),
        os.path.join('..', 'modules', 'ml', 'include'),
        os.path.join('..', 'modules', 'photo', 'include'),
        os.path.join('..', 'modules', 'shape', 'include'),
        os.path.join('..', 'modules', 'imgproc', 'include'),
        os.path.join('..', 'modules', 'calib3d', 'include'),
        os.path.join('..', 'modules', 'features2d', 'include'),
        os.path.join('..', 'modules', 'video', 'include'),
        os.path.join('..', 'modules', 'objdetect', 'include'),
        os.path.join('..', 'modules', 'imgcodecs', 'include'),
        os.path.join('..', 'modules', 'hal', 'include'),
        os.path.join('..', 'modules', 'highgui', 'include'),
        os.path.join('..', 'modules', 'videoio', 'include')
    ]
    print 'Include paths...'
    for dir in include_dirs:
        print '--', os.path.abspath(dir)
    include_dir_args = ['-I' + item for item in include_dirs]
    emscripten.Building.emcc(
        input_file_path, emcc_args + include_dir_args, 'input.bc')
    assert os.path.exists('input.bc')
    print success('-> Done')

    stage('Linking input bitcode and static libraries')
    input_files = [
        os.path.join('lib', 'libopencv_features2d.a'),
        os.path.join('lib', 'libopencv_core.a'),
        os.path.join('lib', 'libopencv_imgproc.a'),
        os.path.join('lib', 'libopencv_imgcodecs.a'),

        os.path.join('lib', 'libopencv_ml.a'),
        os.path.join('lib', 'libopencv_flann.a'),
        os.path.join('lib', 'libopencv_objdetect.a'),

        os.path.join('lib', 'libopencv_shape.a'),
        os.path.join('lib', 'libopencv_photo.a'),
        os.path.join('lib', 'libopencv_video.a'),

        # external libraries
        os.path.join('3rdparty', 'lib', 'liblibjpeg.a'),
        os.path.join('3rdparty', 'lib', 'liblibpng.a'),
        os.path.join('3rdparty', 'lib', 'libzlib.a'),
        #os.path.join('3rdparty', 'lib', 'liblibtiff.a'),
        #os.path.join('3rdparty', 'lib', 'liblibwebp.a'),
    ]
    print 'Attempting to link library archives...'
    for path in input_files:
        print '--', os.path.basename(path)
    link_flags = ['-Wl,--start-group'] + input_files + ['-Wl,--end-group']
    emscripten.Building.emcc('input.bc', emcc_args +
                             link_flags, 'linked_input.bc')
    assert os.path.exists('linked_input.bc')
    print success('-> Done')

    stage('Building html...')
    if not os.path.exists('../../web_build'):
        os.makedirs('../../web_build')
    js_build_path = os.path.join('..', '..', 'web_build')

    emcc_args += '-s DISABLE_EXCEPTION_CATCHING=0'.split(' ')

    opencv = os.path.join(js_build_path, 'cv.html')
    data = os.path.join('..', '..', 'build', 'cv.data')

    tests = os.path.join('..', '..', 'test')

    emscripten.Building.emcc('linked_input.bc', emcc_args, opencv)

    stage('Report')
    print 'js created:', success(str(os.path.exists(os.path.join(js_build_path, 'cv.js'))))
    print os.path.getsize(os.path.join(js_build_path, 'cv.js')) / 1000000.0, 'mb', '\n'
    print 'html created:', success(str(os.path.exists(os.path.join(js_build_path, 'cv.html'))))
    print os.path.getsize(os.path.join(js_build_path, 'cv.html')) / 1000000.0, 'mb', '\n'
    print 'data created:', success(str(os.path.exists(os.path.join(js_build_path, 'cv.data'))))
    print os.path.getsize(os.path.join(js_build_path, 'cv.data')) / 1000000.0, 'mb', '\n'
    print success('... success!')

finally:
    os.chdir(this_dir)
