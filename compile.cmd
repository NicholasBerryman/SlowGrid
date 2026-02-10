cmake -DCMAKE_MAKE_PROGRAM=ninja -G Ninja -B build -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
pause