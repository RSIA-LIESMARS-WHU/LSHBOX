Compile pylshbox
================

If you want to compile this part by yourself, please delete the comment of the last line in file `CMakeLists.txt` in the first directory of this project, and you will find the compiling progress of this part must rely on Boost::python.

If the Boost library installed on your machine, you can use cmake to compile. Otherwise, you have two choices:

A. Install the Boost library (link: http://www.boost.org/), and than to complete the compilation.

B. Download the part of Boost library that need to use (link: https://github.com/RSIA-LIESMARS-WHU/LSHBOX-3rdparty), extracting file `3rdparty.zip` and put the file `3rdparty` in the first directory of this project, than, you should replace the content of `CMakeLists.txt` with `CMakeLists2.txt`, finally, to complete the compilation.