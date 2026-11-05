#include "../utils.hpp"

#include "ioh/logger/flatfile.hpp"
#include "ioh/logger/analyzer.hpp"
#include "ioh/problem/bbob/sphere.hpp"
#include "ioh/problem/bbob/attractive_sector.hpp"

using namespace ioh;

TEST_F(BaseTest, logger_flatfile)
{
    auto p0 = problem::bbob::Sphere(1, 2);
    auto p1 = problem::bbob::Sphere(1, 4);
    auto p2 = problem::bbob::AttractiveSector(1, 4);

    trigger::Always always;
    watch::TransformedY transformed_y;
    {
        auto logger = logger::FlatFile( {always}, {transformed_y}, "IOH.dat", "." );

        const int runs = 3;
        const int samples = 3;
        
        for(auto pb : std::array<problem::BBOB*,3>({&p0,&p1,&p2})) {
            pb->attach_logger(logger);
            for(auto r = 0; r < runs; ++r) {
                for(auto s = 0; s < samples; ++s) {
                    (*pb)(common::random::pbo::uniform(pb->meta_data().n_variables, s));
                }
                pb->reset();
            }
        }
    }
    EXPECT_TRUE(fs::exists("./IOH.dat"));
    EXPECT_GT(ioh::common::file::as_string("./IOH.dat").size(),0);
    fs::remove("./IOH.dat");
    EXPECT_TRUE(!fs::exists("./IOH.dat"));
}

namespace
{
    //! A property with a value below the resolution of "{:.10f}" and a fractional part that needs 17 digits
    struct TinyValue : logger::Property
    {
        explicit TinyValue(const std::string &name = "tiny") : Property(name) {}
        std::optional<double> operator()(const logger::Info &) const override { return 3e-11; }
    };

    struct ThirdValue : logger::Property
    {
        explicit ThirdValue(const std::string &name = "third") : Property(name) {}
        std::optional<double> operator()(const logger::Info &) const override { return 1.0 / 3.0; }
    };

    std::vector<std::string> split_fields(const std::string &line)
    {
        std::istringstream ss(line);
        std::vector<std::string> fields;
        for (std::string f; ss >> f;)
            fields.push_back(f);
        return fields;
    }

    std::vector<std::string> read_lines(const fs::path &file)
    {
        std::ifstream in(file);
        std::vector<std::string> lines;
        for (std::string l; std::getline(in, l);)
            lines.push_back(l);
        return lines;
    }
} // namespace

TEST_F(BaseTest, flatfile_full_precision)
{
    TinyValue tiny;
    ThirdValue third;
    watch::Evaluations evaluations;
    const std::array<std::string, 2> files = {"IOH_default_precision.dat", "IOH_full_precision.dat"};
    for (size_t i = 0; i < 2; ++i)
    {
        auto p = problem::bbob::Sphere(1, 2);
        logger::FlatFile logger({trigger::always}, {evaluations, tiny, third}, files[i], ".", " ", "", "None", "\n",
                                false, true, {});
        EXPECT_FALSE(logger.full_precision());
        logger.set_full_precision(i == 1);
        p.attach_logger(logger);
        p(std::vector<double>{0.1, 0.2});
        p.detach_logger();
    }

    const auto def = read_lines(files[0]);
    const auto full = read_lines(files[1]);
    ASSERT_EQ(def.size(), 2u);
    ASSERT_EQ(full.size(), 2u);
    // The header, and the number of columns, do not change
    EXPECT_EQ(def[0], full[0]);
    EXPECT_EQ(split_fields(def[1]).size(), split_fields(def[0]).size());
    EXPECT_EQ(split_fields(full[1]).size(), split_fields(full[0]).size());

    // Today: below 5e-11 is written as 0, and 1/3 is cut after 10 digits
    const auto d = split_fields(def[1]), f = split_fields(full[1]);
    const size_t n = d.size();
    EXPECT_EQ(d[n - 5], "1");
    EXPECT_EQ(std::stod(d[n - 4]), 0.0);
    EXPECT_NE(std::stod(d[n - 3]), 1.0 / 3.0);


    // With the option: the exact doubles come back
    EXPECT_EQ(std::stod(f[n - 4]), 3e-11);
    EXPECT_EQ(std::stod(f[n - 3]), 1.0 / 3.0);
    // x positions: round-trip as well
    EXPECT_EQ(std::stod(f[n - 2]), 0.1);
    EXPECT_EQ(std::stod(f[n - 1]), 0.2);
    // The evaluation counter keeps its own format
    EXPECT_EQ(f[n - 5], "1");

    for (const auto &file : files)
        fs::remove(file);
}

TEST_F(BaseTest, analyzer_full_precision)
{
    TinyValue tiny;
    std::vector<std::string> data_lines[2];
    std::string json[2];
    for (size_t i = 0; i < 2; ++i)
    {
        fs::path out;
        {
            auto p = problem::bbob::Sphere(1, 2);
            logger::Analyzer logger({trigger::always}, {tiny}, fs::current_path(),
                                    i == 0 ? "ioh_prec_default" : "ioh_prec_full");
            logger.set_full_precision(i == 1);
            out = logger.output_directory();
            p.attach_logger(logger);
            p(std::vector<double>{0.1, 0.2});
            p.detach_logger();
        }
        data_lines[i] = read_lines(out / "data_f1_Sphere" / "IOHprofiler_f1_DIM2.dat");
        json[i] = ioh::common::file::as_string(out / "IOHprofiler_f1_Sphere.json");
        fs::remove_all(out);
    }

    // The header and the column count do not change, and every line still parses
    ASSERT_EQ(data_lines[0].size(), 2u);
    ASSERT_EQ(data_lines[1].size(), 2u);
    EXPECT_EQ(data_lines[0][0], data_lines[1][0]);
    const size_t ncol = split_fields(data_lines[0][0]).size();
    EXPECT_EQ(ncol, 3u);
    for (size_t i = 0; i < 2; ++i)
    {
        const auto fields = split_fields(data_lines[i][1]);
        ASSERT_EQ(fields.size(), ncol);
        for (const auto &f : fields)
            EXPECT_NO_THROW(std::stod(f));
    }
    EXPECT_EQ(std::stod(split_fields(data_lines[0][1])[2]), 0.0);
    EXPECT_EQ(std::stod(split_fields(data_lines[1][1])[2]), 3e-11);

    // The precision is recorded in the JSON metadata, as an extra key, only when the option is on
    EXPECT_EQ(json[0].find("precision"), std::string::npos);
    EXPECT_NE(json[1].find("\"precision\": \"%.17g\""), std::string::npos);
}
