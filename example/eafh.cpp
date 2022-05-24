#include <ioh.hpp>
#include <clutchlog/clutchlog.h>

/******************************************************************************
 * This command line interface aims at comparing the performances of the
 * Empirical Attainment loggers.
 * Namely the true function VS the histogram, with varying parameters.
 *****************************************************************************/
using namespace ioh;

enum Error {not_enough_args, wrong_number_of_args, type_not_supported, scale_not_supported};

template<class L>
void run(L& logger, const size_t samples, const size_t runs)
{
    suite::BBOB suite({1, 2}, {1, 2}, {10, 30});

    IOH_DBG(debug,"Attach suite " << suite.name() << " to logger");
    suite.attach_logger(logger);

    for(const auto& pb : suite) {
        IOH_DBG(progress, "pb:" << pb->meta_data().problem_id << ", dim:" << pb->meta_data().n_variables << ", ins:" << pb->meta_data().instance);
        for(size_t r = 0; r < runs; ++r) {
            IOH_DBG(progress, "> run:" << r);
            for(size_t s = 0; s < samples; ++s) {
                (*pb)(common::random::doubles(pb->meta_data().n_variables));
            }
            pb->reset();
        }
    }
}

void fail(const Error error, const std::string& msg = "")
{
    std::cerr << "ERROR: " << msg << std::endl;
    std::cerr << "Usage: eafh <samples> <runs> <type> [buckets scale]" << std::endl;
    std::cerr << "\ttype = 'EAH' or 'EAF' (the default)" << std::endl;
    std::cerr << "\tscale (only for EAH) = 'log2', 'log10' or 'linear'" << std::endl;
    std::cerr << "\tbuckets (only for EAH) = number of buckets" << std::endl;
    exit(error);
}

int main(int argc, char** argv)
{
    if(argc < 4) { fail(not_enough_args, "Not enough arguments"); }

    size_t samples = 0;
    size_t runs = 0;
    const double bbob_max = 6e7;
    
    if(argc >= 4) {
        samples = std::atoi(argv[1]);
        runs = std::atoi(argv[2]);
    }

    std::string type(argv[3]);
    if(type == "EAF") {
        if(argc != 4) { fail(wrong_number_of_args, "EAF type should come with 3 arguments"); }
        std::clog << "EAF: samples=" << samples << ", runs=" << runs << std::endl;
        
        logger::EAF logger;
        run(logger, samples, runs);
        std::cout << logger::eaf::stat::volume(logger, 0, bbob_max, 0, samples, runs) << std::endl;

    } else if(type == "EAH") {
        if(argc != 6) { fail(wrong_number_of_args, "EAF type should come with 5 arguments"); }
        
        size_t buckets = std::atoi(argv[4]);

        std::string scale(argv[5]);
        if(scale == "linear") {
            std::clog << "EAH: samples=" << samples << ", runs=" << runs << ", buckets=" << buckets << ", scale=" << scale << std::endl;
            logger::eah::LinearScale<double> sv(0,bbob_max,buckets);
            logger::eah::LinearScale<size_t> st(0,samples,buckets);
            logger::EAH logger(sv, st);
            run(logger, samples, runs);
            std::cout << logger::eah::stat::under_curve::volume(logger) << std::endl;

        } else if(scale == "log2"){
            std::clog << "EAH: samples=" << samples << ", runs=" << runs << ", buckets=" << buckets << ", scale=" << scale << std::endl;
            logger::eah::Log2Scale<double> sv(0,bbob_max,buckets);
            logger::eah::Log2Scale<size_t> st(0,samples,buckets);
            logger::EAH logger(sv, st);
            run(logger, samples, runs);
            std::cout << logger::eah::stat::under_curve::volume(logger) << std::endl;

        } else if(scale == "log10"){
            std::clog << "EAH: samples=" << samples << ", runs=" << runs << ", buckets=" << buckets << ", scale=" << scale << std::endl;
            logger::eah::Log10Scale<double> sv(0,bbob_max,buckets);
            logger::eah::Log10Scale<size_t> st(0,samples,buckets);
            logger::EAH logger(sv, st);
            run(logger, samples, runs);
            std::cout << logger::eah::stat::under_curve::volume(logger) << std::endl;

        } else {
            std::ostringstream msg;
            msg << "`" << scale << "` scale not supported";
            fail(scale_not_supported, msg.str());
        }
    } else {
        std::ostringstream msg;
        msg << "`" << type << "` type not supported";
        fail(type_not_supported, msg.str());
    }
    

}
