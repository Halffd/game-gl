rem call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
mkdir build-windows
cd build-windows
del cmake.txt
del build.txt
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release > cmake.txt
msbuild game.sln /p:Configuration=Release > build.txt
cd ..
