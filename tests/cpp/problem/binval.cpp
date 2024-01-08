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
  const auto &dyn_problem_factory = ioh::problem::ProblemRegistry<ioh::problem::DynamicBinValRanking>::instance();
  auto d = dyn_problem_factory.create(10004, 1, 5);

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
  std::vector<std::vector<int>> sorted_bitstrings = d->rank(bitstrings);
  std::vector<int> indices = d->rank_indices(bitstrings);

  // Log the sorted order of sorted_bitstrings
  LOG("Sorted order of sorted_bitstrings:");
  for (size_t i = 0; i < sorted_bitstrings.size(); ++i) {
      const auto& bits = sorted_bitstrings[i];
      std::string bitstring;
      for (int bit : bits) {
          bitstring += std::to_string(bit);
      }

      // Log the corresponding index from the indices vector
      LOG("Bitstring: " + bitstring + " | Original Index: " + std::to_string(indices[i]));
  }

  return 0;
}
