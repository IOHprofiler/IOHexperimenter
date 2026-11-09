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
    //! Split a string on a separator (an empty field is kept)
    std::vector<std::string> split_on(const std::string &s, const std::string &sep)
    {
        std::vector<std::string> fields;
        size_t start = 0, pos;
        while ((pos = s.find(sep, start)) != std::string::npos)
        {
            fields.push_back(s.substr(start, pos - start));
            start = pos + sep.size();
        }
        fields.push_back(s.substr(start));
        return fields;
    }

    //! Log 3 evaluations of a 2D sphere with the given logger arguments and return the lines of the file
    std::vector<std::string> flatfile_lines(const std::string &sep, const bool meta, const bool positions)
    {
        const std::string name = "ioh_flatfile_separator_test.dat";
        fs::remove(name);
        {
            auto problem = problem::bbob::Sphere(1, 2);
            trigger::Always always;
            watch::Evaluations evaluations;
            watch::TransformedY transformed_y;
            std::vector<std::string> titles = {"suite_name",      "problem_name", "problem_id", "problem_instance",
                                               "optimization_type", "dimension",    "run"};
            if (!meta)
                titles.clear();
            logger::FlatFile logger({always}, {evaluations, transformed_y}, name, ".", sep, "# ", "None", "\n", false,
                                    positions, titles);
            problem.attach_logger(logger);
            for (int s = 0; s < 3; ++s)
                problem({0.1 * s, 0.2});
        }
        std::ifstream in(name);
        std::vector<std::string> lines;
        for (std::string line; std::getline(in, line);)
            lines.push_back(line);
        in.close();
        fs::remove(name);
        return lines;
    }

    //! Check that every data row has as many fields as the header, and that the fields hold what they should
    void check_flatfile(const std::string &sep, const bool meta, const bool positions)
    {
        const auto lines = flatfile_lines(sep, meta, positions);
        ASSERT_EQ(lines.size(), 4u) << "sep '" << sep << "'";
        ASSERT_EQ(lines[0].substr(0, 2), "# ");
        const auto header = split_on(lines[0].substr(2), sep);
        const size_t n_fields = (meta ? 7 : 0) + 2 + (positions ? 2 : 0);
        EXPECT_EQ(header.size(), n_fields) << lines[0];
        for (size_t i = 1; i < lines.size(); ++i)
        {
            const auto row = split_on(lines[i], sep);
            ASSERT_EQ(row.size(), header.size()) << "row: " << lines[i];
            const size_t first = meta ? 7 : 0;
            if (meta)
            {
                EXPECT_EQ(row[5], "2") << lines[i];     // dimension
                EXPECT_EQ(row[6], "0") << lines[i];     // run
            }
            EXPECT_EQ(row[first], std::to_string(i)) << lines[i]; // evaluations
        }
    }
} // namespace

TEST_F(BaseTest, logger_flatfile_separator_with_meta_data)
{
    for (const std::string sep : {"\t", ",", " ; "})
        check_flatfile(sep, true, false);
}

TEST_F(BaseTest, logger_flatfile_separator_with_positions)
{
    check_flatfile("\t", true, true);
}

TEST_F(BaseTest, logger_flatfile_separator_without_meta_data)
{
    check_flatfile("\t", false, false);
    check_flatfile(",", false, true);
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
