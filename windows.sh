mkdir -p ./win
cd win
cmake clean .
cmake ..
cmake --build . > log.txt
../bin/game.exe