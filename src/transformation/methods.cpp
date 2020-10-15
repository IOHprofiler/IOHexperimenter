/// \file transformation.cpp
/// \brief Cpp file for the class transformation.
///
/// A detailed file description.
///
/// \author Furong Ye

#include "ioh/problem/transformation.hpp"

using namespace ioh::problem::transformation;


int methods::xor_compute(const int x1, const int x2)
{
	return static_cast<int>(x1 != x2);
}

void methods::transform_vars_xor(std::vector<int>& x, const int seed)
{
	std::vector<double> random_x;
	int n = x.size();
	common::random::uniform_rand(n, seed, random_x);

	for (auto i = 0; i < n; ++i)
	{
		int xor_value = static_cast<int>(2 * floor(1e4 * random_x[i]) / 1e4);
		x[i] = xor_compute(x[i], xor_value);
	}
}

int methods::sigma_compute(const std::vector<int>& x, const int pos)
{
	return x[pos];
}

void methods::transform_vars_sigma(std::vector<int>& x, const int seed)
{
	std::vector<int> copy_x;
	std::vector<int> index;
	std::vector<double> random_x;
	int N = x.size(), t, temp;

	copy_x = x;

	index.reserve(N);
	for (int i = 0; i != N; ++i)
	{
		index.push_back(i);
	}

	common::random::uniform_rand(N, seed, random_x);
	for (int i = 0; i != N; ++i)
	{
		t = static_cast<int>(floor(random_x[i] * N));
		temp = index[0];
		index[0] = index[t];
		index[t] = temp;
	}
	for (int i = 0; i < N; ++i)
	{
		x[i] = sigma_compute(copy_x, index[i]);
	}
}

void methods::transform_obj_scale(std::vector<double>& y, const int seed)
{
	std::vector<double> scale;
	common::random::uniform_rand(1, seed, scale);
	scale[0] = scale[0] * 1e4 / 1e4 * 4.8 + 0.2;
	for (size_t i = 0; i < y.size(); ++i)
	{
		y[i] = y[i] * scale[0];
	}
}

void methods::transform_obj_shift(std::vector<double>& y, const int seed)
{
	std::vector<double> shift;
	common::random::uniform_rand(1, seed, shift);
	shift[0] = shift[0] * 1e4 / 1e4 * 2000 - 1000;
	for (size_t i = 0; i < y.size(); ++i)
	{
		y[i] = y[i] + shift[0];
	}
}

void methods::transform_obj_scale(double& y, const int seed)
{
	std::vector<double> scale;
	common::random::uniform_rand(1, seed, scale);
	scale[0] = scale[0] * 1e4 / 1e4 * 4.8 + 0.2;
	y = y * scale[0];
}

void methods::transform_obj_shift(double& y, const int seed)
{
	std::vector<double> shift;
	common::random::uniform_rand(1, seed, shift);
	shift[0] = shift[0] * 1e4 / 1e4 * 2000 - 1000;
	y = y + shift[0];
}
