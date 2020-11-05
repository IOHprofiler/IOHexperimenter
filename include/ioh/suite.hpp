#pragma once

#include "suite/base.hpp"
#include "suite/pbo.hpp"
#include "suite/bbob.hpp"


namespace ioh::suite
{
	static common::register_in_factory<base<pbo::pbo_base>, pbo, std::vector<int>, std::vector<int>, std::vector<int>> reg_pbo("PBO");
	static common::register_in_factory<base<bbob::bbob_base>, bbob, std::vector<int>, std::vector<int>, std::vector<int>> reg_bbob("BBOB");
}
