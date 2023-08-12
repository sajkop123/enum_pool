#include "enum_pool.h"

#include <sstream>

enum ENUM {
  A,
  B,
  C,
  D,
  ENUM_MAP_MAX_SIZE,
};

inline const char* to_str(ENUM e) {
  switch(e) {
    case A: return "A";
    case B: return "B";
    case C: return "C";
    case D: return "D";
    default : return "UNKNOWN";
  }
};

static inline void print_enum_map(const enum_map<ENUM, int>& map) {
  std::ostringstream log;
  for (const auto it : map) {
    log << "(" << to_str(it.first) << ", " << it.second << ")";
  }
  MY_LOGD("%s", log.str().c_str());
}

int main() {
  enum_map<ENUM, int> map{{ENUM::B,10}, {ENUM::C,2}};
  print_enum_map(map);

  {
    const int c = map.at(ENUM::B);
    print_enum_map(map);
  }

  {
    MY_LOGD("1. add ENUM::A, 99");
    map.insert({ENUM::A, 99});
    const int c = map.at(ENUM::A);
    print_enum_map(map);
  }
  {
    MY_LOGD("2. insert ENUM::D, 654");
    map.insert({ENUM::D, 654});
    const int c = map.at(ENUM::B);
    print_enum_map(map);
  }
  {
    MY_LOGD("3. insert ENUM::B, 8");
    int value = 8;
    map.insert({ENUM::B, value});
    const int c = map.at(ENUM::B);
    print_enum_map(map);
  }
  {
    MY_LOGD("4. emplace pair(ENUM::D, 77777)");
    map.emplace(std::make_pair(ENUM::D, 77777));
    const int c = map.at(ENUM::B);
    print_enum_map(map);
  }
  {
    MY_LOGD("5. erase ENUM::C");
    for (auto it = map.begin(); it != map.end();) {
      if (it->first == ENUM::C) {
        map.erase(it);
        break;
      } else {
        it++;
      }
    }
    print_enum_map(map);
  }
  {
    MY_LOGD("6. swap with another enum_map({C, 777}, {D, 7777})");
    enum_map<ENUM, int> new_a{{ENUM::C,777}, {ENUM::D,7777}};
    map.swap(new_a);
    print_enum_map(map);
  }
}


// https://quick-bench.com/q/h2ZlNfzMFBuhe-A0yjgg2VuMv4g