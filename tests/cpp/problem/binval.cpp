#include "../utils.hpp"

#include "ioh/problem/dynamic_bin_val/dynamic_bin_val.hpp"

/**
 * @def DEBUG
 * Enables logging functionality throughout the code when defined.
 */
#define DEBUG

#ifdef DEBUG

/**
 * @def LOG_FILE_NAME
 * Defines the name of the log file where debug messages will be written to.
 */
#define LOG_FILE_NAME "cec_test_log.txt"

/**
 * @def LOG
 * Macro to log messages to a file with a timestamp.
 * @param message The message to log.
 */
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

#else

/**
 * @def LOG
 * Stub for LOG macro when DEBUG is not defined.
 * @param message The message parameter is ignored.
 */
#define LOG(message) // Nothing
#endif

int main()
{
  const auto &dyn_problem_factory = ioh::problem::ProblemRegistry<ioh::problem::DynamicBinVal>::instance();
  auto d = dyn_problem_factory.create(10001, 1, 5);
  auto x = std::vector<int>{1, 0, 1, 1, 1};
  double value = (*d)(x);

  LOG("[DynamicBinVal]" << "d->timestep: " << d->timestep << " | Value: " << value);
  for(int i = 0; i < d->weights.size(); ++i)
  {
    LOG("[DynamicBinVal]" << "d->weights[" << i << "]: " << d->weights[i]);
  }

  d->step();
  value = (*d)(x);
  LOG("[DynamicBinVal]" << "d->timestep: " << d->timestep << " | Value: " << value);
  for(int i = 0; i < d->weights.size(); ++i)
  {
    LOG("[DynamicBinVal]" << "d->weights[" << i << "]: " << d->weights[i]);
  }

  d->step();
  value = (*d)(x);
  LOG("[DynamicBinVal]" << "d->timestep: " << d->timestep << " | Value: " << value);
  for(int i = 0; i < d->weights.size(); ++i)
  {
    LOG("[DynamicBinVal]" << "d->weights[" << i << "]: " << d->weights[i]);
  }

  d->step();
  value = (*d)(x);
  LOG("[DynamicBinVal]" << "d->timestep: " << d->timestep << " | Value: " << value);
  for(int i = 0; i < d->weights.size(); ++i)
  {
    LOG("[DynamicBinVal]" << "d->weights[" << i << "]: " << d->weights[i]);
  }
}
