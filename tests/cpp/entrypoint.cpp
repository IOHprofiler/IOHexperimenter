#include "utils.hpp"

void BaseTest::SetUp()
{
    auto &log = clutchlog::logger();
    log.threshold(log_level_);

    if (log_file_.has_value())
        log.file(log_file_.value());

#if CLUTCHLOG_HAVE_UNIX_SYSINFO == 1
    if (log_depth_.has_value())
        log.depth(log_depth_.value());
#endif
}   

int main(int argc, char** argv) {	
    ::testing::InitGoogleTest(&argc, argv);
    // Note: Google Test does remove its options from argv,
    // so here there is no --gtest_* argument, even if they were passed.

    // Allows to adjust the debugging log parameters for all tests.
    // Binaries will expect the log level, then the depth level, then the file scope regexp.
    if(argc > 1) {
        BaseTest::log_level_ = clutchlog::logger().level_of(argv[1]);
        if(argc > 2) {
            // TODO we could also handle function and/or line regexp.
            // Maybe it's useless for tests, so it's not handled for the moment.
            BaseTest::log_file_ = argv[2];
            if(argc > 3) {
                BaseTest::log_depth_ = std::atoi(argv[3]);
                if(argc > 4) {
                    std::cerr << "WARNING: Tests do not handle more than 3 arguments, but you passed " << argc-1 << " arguments." << std::endl;
                    std::cerr << "Usage: <test> [--gtest* options] [level [scope [depth]]" << std::endl;
                    std::cerr << "\tLevel is a string indicating the log threshold, levels are: "
                              << "Critical > Error > Warning > Progress > Note > Info > Debug > XDebug (default: Warning)." << std::endl;
                    std::cerr << "\tScope is an ECMAscript regexp matching the source files (default: '')." << std::endl;
                    std::cerr << "\tDepth is an integer indicating the call stack threshold (default: 0)." << std::endl;
                }
            } // Clutchlog's default already on INTMAX.
        } // Clutchlog's default already on any file.
    } 
    return RUN_ALL_TESTS();
}
