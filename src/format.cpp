#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
std::string Format::ElapsedTime(long seconds) {
  long HH = seconds / 3600;
  seconds %= 3600;
  long MM = seconds / 60;
  seconds %= 60;

  // Use a string stream to format the output with leading zeros
  std::ostringstream elapsed_time;
  elapsed_time << std::setw(2) << std::setfill('0') << HH << ":" << std::setw(2)
               << std::setfill('0') << MM << ":" << std::setw(2)
               << std::setfill('0') << seconds;

  return elapsed_time.str();
}