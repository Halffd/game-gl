rm -rf bin
mkdir -p build
cd build
cmake ..
cmake --build .
cd ..
zip game.zip bin/*
