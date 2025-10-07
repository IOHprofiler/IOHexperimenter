#include "pch.hpp"

void define_real_suite(nb::module_ &m);
void define_integer_suite(nb::module_ &m);

void define_suites(nb::module_ &m)
{
    define_real_suite(m);
    define_integer_suite(m);
}
