#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float mem_utilization = 0.0;
  std::string line;
  std::string key;
  std::string value;
  std::ifstream filestream{kProcDirectory + kMeminfoFilename};
  if (filestream.is_open()) {
    float mem_total = 0.0;
    float mem_free = 0.0;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> key >> value) {
        if (key == "MemTotal:") {
          mem_total = std::stof(value);
        }
        if (key == "MemFree:") {
          mem_free = std::stof(value);
        }
        if (mem_free > 0.0 && mem_total > 0.0) {
          break;
        }
      }
    }
    if (mem_total > 0.0) {
      float mem_used = mem_total - mem_free;
      mem_utilization = mem_used / mem_total;
    }
  }
  return mem_utilization;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long up_time = 0.0;
  std::string line;
  std::ifstream filestream{kProcDirectory + kUptimeFilename};
  if (filestream.is_open()) {
    std::string t_up;
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> t_up;
    up_time = std::stol(t_up);
  }
  return up_time;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long num_jiffies = 0;
  const auto v_jiffies = CpuUtilization();
  for (const auto& jiffie : v_jiffies) {
    num_jiffies += std::stol(jiffie);
  }
  return num_jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  long active_jiffies = 0;
  std::string line;
  std::ifstream filestream{kProcDirectory + std::to_string(pid) +
                           kStatFilename};
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::string value;
    size_t count = 0;
    while (linestream >> value) {
      if (count == 13) {
        active_jiffies += std::stol(value);
      } else if (count == 14) {
        active_jiffies += std::stol(value);
        break;
      }
      count++;
    }
  }
  return active_jiffies;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  long active_jiffies = 0;
  const auto v_jiffies = CpuUtilization();
  active_jiffies =
      std::stol(v_jiffies[kUser_]) + std::stol(v_jiffies[kNice_]) +
      std::stol(v_jiffies[kSystem_]) + std::stol(v_jiffies[kIRQ_]) +
      std::stol(v_jiffies[kSoftIRQ_]) + std::stol(v_jiffies[kSteal_]);
  return active_jiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long idle_jiffies = 0;
  const auto v_jiffies = CpuUtilization();
  idle_jiffies = std::stol(v_jiffies[kIdle_]) + std::stol(v_jiffies[kIOwait_]);
  return idle_jiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::vector<std::string> cpu_utilization{};
  std::string line;
  std::ifstream filestream{kProcDirectory + kStatFilename};
  if (filestream.is_open()) {
    std::string key;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      if (linestream >> key) {
        if (key == "cpu") {
          std::string jiffie;
          while (linestream >> jiffie) {
            cpu_utilization.push_back(jiffie);
          }
          break;
        }
      }
    }
  }
  return cpu_utilization;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int processes = 0;
  std::string line;
  std::ifstream filestream{kProcDirectory + kStatFilename};
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      std::string key;
      if (linestream >> key) {
        if (key == "processes") {
          std::string value;
          linestream >> value;
          processes = std::stoi(value);
          break;
        }
      }
    }
  }
  return processes;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int running_processes = 0;
  std::string line;
  std::ifstream filestream{kProcDirectory + kStatFilename};
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      std::string key;
      if (linestream >> key) {
        if (key == "procs_running") {
          std::string value;
          linestream >> value;
          running_processes = std::stoi(value);
          break;
        }
      }
    }
  }
  return running_processes;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::string line;
  std::ifstream filestream{kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename};
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::string pid_ram;
  std::string line;
  std::ifstream filestream{kProcDirectory + std::to_string(pid) +
                           kStatusFilename};
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      std::string key;
      if (linestream >> key) {
        if (key == "VmSize") {
          std::string value;
          linestream >> value;
          pid_ram = std::to_string((std::stol(value) / 1024));
          break;
        }
      }
    }
  }
  return pid_ram;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::string uid;
  std::string line;
  std::ifstream filestream{kProcDirectory + std::to_string(pid) +
                           kStatusFilename};
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      std::string key;
      if (linestream >> key) {
        if (key == "Uid:") {
          std::string value;
          linestream >> value;
          uid = value;
          break;
        }
      }
    }
  }
  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::string uid = Uid(pid);
  std::string user;
  std::string line;
  std::ifstream filestream{kPasswordPath};
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      std::string key;
      std::string token;
      std::string value;
      if (linestream >> key >> token >> value) {
        if ((token == "x") && (value == uid)) {
          user = key;
          break;
        }
      }
    }
  }
  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  long up_time = 0;
  std::string line;
  std::ifstream filestream{kProcDirectory + std::to_string(pid) +
                           kStatFilename};
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::string value;
    size_t count = 0;
    while (linestream >> value) {
      if (count == 21) {
        up_time = (std::stol(value) / sysconf(_SC_CLK_TCK));
        break;
      }
      count++;
    }
  }
  return up_time;
}
