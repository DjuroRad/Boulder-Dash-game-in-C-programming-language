#! /bin/bash
tar -xvf SDL2-2.0.10.tar.gz
tar -xvf SDL2_image-2.0.10.tar.gz
tar -xvf SDL2_mixer-2.0.10.tar.gz
tar -xvf SDL2_tff-2.0.10.tar.gz
cd SDL2-2.0.10
./configure
make all
make install
cd ../
make
./program
