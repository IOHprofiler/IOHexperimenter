#include "ioh.hpp"


template <typename Iterator>
void print(const Iterator x, const std::string &del = " ")
{
    for (auto e : x)
    {
        std::cout << e << del;
    }
    std::cout << std::endl;
}

struct GraphMeta
{
    std::string edge_file;
    std::string e_weights;
    std::string v_weights;
    std::string c_weights;
    bool is_edge = false;
    double change_cons = 0.0;

    static GraphMeta from_string(const std::string &definition)
    {
        GraphMeta meta;
        std::string dp;
        std::array<std::string *, 5> p{&meta.edge_file, &meta.e_weights, &meta.v_weights, &meta.c_weights, &dp};
        std::stringstream ss(definition);
        size_t i = 0;
        while (getline(ss, *p[i++], '|') && (i < p.size()))
            ;
        try
        {
            meta.change_cons = std::stod(dp);
        }
        catch (...)
        {
            meta.change_cons = 0.0;
        }
        return meta;
    }
};

struct Graph : ioh::common::HasRepr
{
    GraphMeta meta;

    Graph(const GraphMeta &meta) : meta(meta) {}

    Graph(const std::string &definition) : Graph(GraphMeta::from_string(definition)) {}

    std::string repr() const
    {
        return fmt::format("<GraphInstance {} {} {} {}>", meta.edge_file, meta.e_weights, meta.v_weights,
                           meta.c_weights);
    }
};

void get_adjacency_matrix(const fs::path &path)
{
    const auto contents = ioh::common::file::as_text_vector(path);
    const auto digraph = static_cast<bool>(std::stoi(contents.at(0)));
    int min = std::numeric_limits<int>::max(), max = 0;

    std::vector<int> source(contents.size() - 1), target(contents.size() - 1);
    for (size_t i = 1; i < contents.size(); i++)
    {
        std::stringstream{contents.at(i)} >> source[i - 1] >> target[i - 1];
        max = std::max(max, std::max(source[i - 1], target[i - 1]));
        min = std::min(min, std::min(source[i - 1], target[i - 1]));
    }

    std::vector<std::vector<int>> matrix(max - min + 1, std::vector<int>(max - min + 1));

    for (size_t i = 0; i < source.size(); i++)
    {
        matrix.at(source[i] - min).at(target[i] - min) = 1;
        matrix.at(target[i] - min).at(source[i] - min) = 1 * static_cast<int>(digraph);
    }
}


int main()
{
    using namespace ioh::common;
    std::cout << 1 << std::endl;
    auto file = file::find_static_file("example_list_maxinfluence");
    std::cout << file << std::endl;

    auto vector = file::as_text_vector<Graph>(file);
    Graph gi = vector.at(0);
    std::cout << gi << std::endl;

    std::vector<int> c_weights = file::as_numeric_vector<int>(
        file::find_static_file(gi.meta.c_weights));

    std::vector<double> e_weights = file::as_numeric_vector<double>(
        file::find_static_file(gi.meta.e_weights));
    
    std::cout << c_weights.size() << std::endl;
    std::cout << e_weights.size() << std::endl;


    



}