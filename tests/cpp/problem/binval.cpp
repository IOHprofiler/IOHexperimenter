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
  auto x = std::vector<int>{1, 0, 1, 1, 1};
  double value = (*d)(x);

  LOG("[DynamicBinVal]" << "d->timestep: " << d->timestep << " | Value: " << value);
  for(long unsigned int i = 0; i < d->weights.size(); ++i)
  {
    LOG("[DynamicBinVal]" << "d->weights[" << i << "]: " << d->weights[i]);
  }

  d->step();
  value = (*d)(x);
  LOG("[DynamicBinVal]" << "d->timestep: " << d->timestep << " | Value: " << value);
  for(long unsigned int i = 0; i < d->weights.size(); ++i)
  {
    LOG("[DynamicBinVal]" << "d->weights[" << i << "]: " << d->weights[i]);
  }

  d->step();
  value = (*d)(x);
  LOG("[DynamicBinVal]" << "d->timestep: " << d->timestep << " | Value: " << value);
  for(long unsigned int i = 0; i < d->weights.size(); ++i)
  {
    LOG("[DynamicBinVal]" << "d->weights[" << i << "]: " << d->weights[i]);
  }

  d->step();
  value = (*d)(x);
  LOG("[DynamicBinVal]" << "d->timestep: " << d->timestep << " | Value: " << value);
  for(long unsigned int i = 0; i < d->weights.size(); ++i)
  {
    LOG("[DynamicBinVal]" << "d->weights[" << i << "]: " << d->weights[i]);
  }
}
