#ifndef _LOG_H_
#define _LOG_H_
#include <iostream>

void logger(const char* msg)
{
    std::cout << msg << std::endl;
}
#endif