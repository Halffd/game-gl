mkdir -p build/linux
mkdir -p build/windows
mkdir -p build/zip
./linux.sh
mv build/game.zip build/zip/linux.zip
mv bin/* zip
./windows.sh
mv build-windows/game.zip build/zip/windows.zip
mv bin/* windows
mv build/zip bin
mv build/windows bin
mv build/linux bin
ln -s bin/linux/game bin/game
