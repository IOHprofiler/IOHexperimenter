#include "../utils.hpp"

#include "ioh/logger/flatfile.hpp"
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
