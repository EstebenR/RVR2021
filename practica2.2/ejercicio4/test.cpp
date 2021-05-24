#include "Socket.h"
#include "Socket.cc"
#include <iostream>

int main(int argc, char** argv){
    Socket a("127.0.0.1","7878");

    a.bind();

    std::cout << a << std::endl;

    while(true);

    return 0;
}