/// \file IOHprofiler_all_problems.cpp
/// \brief cpp file for class IOHprofiler_all_problems.
///
/// A head file includes all problems of IOHprofiler.
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _IOHPROFILER_ALL_SUITES_HPP
#define _IOHPROFILER_ALL_SUITES_HPP

#include "IOHprofiler_PBO_suite.hpp"

#include "../IOHprofiler_class_generator.hpp"

static registerInFactory<IOHprofiler_suite<int>,PBO_suite> regPBO("PBO");

#endif //_IOHPROFILER_ALL_SUITES_HPP