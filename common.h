#include <iostream>
#include <memory>
#include <string.h>
#include <cassert>

#define LOG_ON 1

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define MY_LOGD(fmt, arg...) if (LOG_ON) { printf("[%s/%d][%s] " fmt"\n", __FILENAME__, __LINE__, __func__, ##arg); }
#define assertm(exp, msg) assert(((void)msg, exp))