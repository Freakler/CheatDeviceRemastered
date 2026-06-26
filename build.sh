mkdir build
cd build

psp-cmake .. && make

echo "CheatDevice Remastered is $(wc -c CheatDeviceRemastered.prx | cut -d " " -f1) bytes"