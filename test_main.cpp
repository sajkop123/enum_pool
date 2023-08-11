#include "enum_pool.h"

enum ENUM {
  A,
  B,
  C,
  D,
};

int main() {
  std::map<int, int> b{{1,1}, {2,2}};
  enum_map<ENUM, int> a{{ENUM::B,10}, {ENUM::C,2}};

  const int c = a.at(ENUM::B);
  MY_LOGD("%d", c);
}