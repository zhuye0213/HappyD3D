#pragma once

#ifdef _DEBUG
#include <iostream>
#define DEBUG_LOG(msg) std::cout << msg << std::endl
#else
#define DEBUG_LOG(msg)
#endif