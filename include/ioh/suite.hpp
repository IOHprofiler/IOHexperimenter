#pragma once

#include "suite/base.hpp"
#include "suite/pbo.hpp"
#include "suite/bbob.hpp"

// What is the use of these things?
static ioh::common::register_in_factory<ioh::problem::pbo::pbo_base, ioh::suite::pbo> regPBO("PBO");
static ioh::common::register_in_factory<ioh::problem::bbob::bbob_base, ioh::suite::bbob> regBBOB("BBOB");
