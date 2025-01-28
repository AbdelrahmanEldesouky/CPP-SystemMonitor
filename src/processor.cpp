#include "processor.h"

#include "linux_parser.h"
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  auto total_t = LinuxParser::Jiffies();
  auto active_t = LinuxParser::ActiveJiffies();

  return float(active_t) / float(total_t);
}