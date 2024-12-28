mkdir -p ./win
cd win
cmake clean .
rm log.txt
cmake .. > log.txt
cmake --build . >> log.txt && ../bin/game.exe