#g++ -o $1            $1.cpp            -L ../lib -l cyusb -l pthread `pkg-config --libs --cflags libusb-1.0`
g++ -o $1            $1.cpp            -L ../lib -l cyusb -l pthread `pkg-config --libs --cflags libusb-1.0` -O3 -g 

