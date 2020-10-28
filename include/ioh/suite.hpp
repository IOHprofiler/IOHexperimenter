#pragma once

#include "suite/base.hpp"
#include "suite/pbo.hpp"
#include "suite/bbob.hpp"


// What is the use of these things?
namespace ioh
{
	static common::register_in_factory<suite::base<problem::pbo::pbo_base>, suite::pbo, std::vector<int>, std::vector<int>, std::vector<int>> pbo("PBO");
	static common::register_in_factory<suite::base<problem::bbob::bbob_base>, suite::bbob, std::vector<int>, std::vector<int>, std::vector<int>> bbob("BBOB");
}
