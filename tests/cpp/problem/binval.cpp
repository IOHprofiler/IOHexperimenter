#include "../utils.hpp"

#include "ioh/problem/dynamic_bin_val.hpp"

#define LOG_FILE_NAME "test.log"

#define LOG(message)                                                           \
  do {                                                                         \
    std::ofstream debug_log(LOG_FILE_NAME, std::ios::app);                     \
    auto now = std::chrono::system_clock::now();                               \
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);          \
    debug_log << "["                                                           \
              << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") \
              << "] " << message << std::endl;                                 \
    debug_log.close();                                                         \
  } while (0)

int main()
{
  const auto &dyn_problem_factory = ioh::problem::ProblemRegistry<ioh::problem::DynamicBinValUniform>::instance();
  auto d = dyn_problem_factory.create(10001, 1, 5);

  // Create a list of bitstrings
  std::vector<std::vector<int>> bitstrings = {
    {1, 0, 1, 1, 1},
    {0, 1, 0, 1, 1},
    {1, 1, 0, 0, 1},
    {0, 0, 0, 1, 0},
    {1, 0, 0, 0, 1}
  };

  // Log the original order of bitstrings
  LOG("Original order of bitstrings:");
  for(const auto &bits : bitstrings) {
    std::string bitstring;
    for(int bit : bits) {
      bitstring += std::to_string(bit);
    }
    LOG(bitstring);
  }

  // Sort the bitstrings
  d->rank(bitstrings);

  // Log the sorted order of bitstrings
  LOG("Sorted order of bitstrings:");
  for(const auto &bits : bitstrings) {
    std::string bitstring;
    for(int bit : bits) {
      bitstring += std::to_string(bit);
    }
    LOG(bitstring);
  }

  return 0;
}
