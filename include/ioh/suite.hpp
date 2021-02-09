#pragma once

#include "suite/base.hpp"
#include "suite/bbob.hpp"
#include "suite/pbo.hpp"


namespace ioh::suite {
    static common::RegisterInFactory<
        base<problem::pbo::pbo_base>, pbo, std::vector<int>, std::vector<int>,
        std::vector<int>
    > reg_pbo("PBO");
    static common::RegisterInFactory<
        base<problem::bbob::bbob_base>, bbob, std::vector<int>, std::vector<int>,
        std::vector<
            int>> reg_bbob("BBOB");
}
