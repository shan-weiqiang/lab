#include <ctime>
#include <string>

#ifndef DAYTIME
#define DAYTIME

inline std::string make_daytime_string() {
  std::time_t now = time(0);
  return ctime(&now);
}
#endif