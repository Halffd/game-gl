rm -rf bin
mkdir -p build-windows ; cd build-windows && mingw64-cmake ..
make
cd ..
zip game.zip bin/*
