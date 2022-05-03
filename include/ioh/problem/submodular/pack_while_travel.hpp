// Author: Viet Anh Do

#pragma once
#include <fstream>
#include <stdexcept>
#include <ioh/common/log.hpp>
#include <ioh/problem/problem.hpp>
#include <ioh/problem/transformation.hpp>

namespace ioh
{
    namespace problem
    {
        namespace submodular

        {
            // Packing While Travelling
            // Description: refer to Evolutionary Submodular Optimization website at https://cs.adelaide.edu.au/~optlog/CompetitionESO2022.php
            class PackWhileTravel final : public Integer
            {
                int n_items;
                double velocity_gap, velocity_max, capacity, penalty;
                std::vector<double> *distances;
                std::vector<std::vector<double>> *weights, *profits;
                std::vector<std::vector<int>> *index_map;
                bool is_initialized;

                // Read TTP instance from file, convert to PWT instance, return problem dimension
                int read_instance_by_id(const int instance, const std::string &instance_list_file)
                {
                    if (!is_null()) // If already initialized, skip reading file
                        return n_items;
                    is_initialized = false;
                    std::vector<std::string> instance_list = Helper::read_list_instance(instance_list_file);
                    if (static_cast<int>(instance_list.size()) <= instance || instance < 0) // If instance id is invalid,
                                               // return a valid dummy size
                        return 1;
                    std::ifstream ttp_data(instance_list[instance]);
                    if (!ttp_data)
                        throw std::invalid_argument("Fail to open instance file: " + (instance_list[instance]));
                    char eol = Helper::get_eol_in_file(instance_list[instance]);
                    std::string str, tstr;
                    int index_line = 0;
                    while (std::getline(ttp_data, str, eol) && index_line++ < 2); // Skip 2 lines, to line 3
                    int n_cities; // Number of locations
                    if (!Helper::is_int(str.substr(str.find_last_of(':') + 1), &n_cities)){
                        IOH_DBG(warning, "Cannot read number of cities for PWT"); // FIXME raise an exception?
                        return 1; // return a valid dummy size
                    }
                    std::getline(ttp_data, str, eol); // Number of items
                    if (!Helper::is_int(str.substr(str.find_last_of(':') + 1), &n_items))
                    {
                        IOH_DBG(warning, "Cannot read number of items for PWT"); // FIXME raise an exception?
                        return 1; // return a valid dummy size
                    }
                    std::getline(ttp_data, str, eol); // Carry capacity
                    if (!Helper::is_double(str.substr(str.find_last_of(':') + 1), &capacity))
                    {
                        IOH_DBG(warning, "Cannot read carry capacity for PWT"); // FIXME raise an exception?
                        return 1; // return a valid dummy size
                    }
                    std::getline(ttp_data, str, eol); // Minimum velocity
                    if (!Helper::is_double(str.substr(str.find_last_of(':') + 1), &velocity_gap))
                    {
                        IOH_DBG(warning, "Cannot read minimum velocity for PWT"); // FIXME raise an exception?
                        return 1; // return a valid dummy size
                    }
                    std::getline(ttp_data, str, eol); // Maximum velocity
                    if (!Helper::is_double(str.substr(str.find_last_of(':') + 1), &velocity_max))
                    {
                        IOH_DBG(warning, "Cannot read maximum velocity for PWT"); // FIXME raise an exception?
                        return 1; // return a valid dummy size
                    }
                    velocity_gap = velocity_max - velocity_gap;
                    std::getline(ttp_data, str, eol);
                    double rent_ratio; // Rent ratio
                    if (!Helper::is_double(str.substr(str.find_last_of(':') + 1), &rent_ratio))
                    {
                        IOH_DBG(warning, "Cannot read rent ratio for PWT"); // FIXME raise an exception?
                        return 1; // return a valid dummy size
                    }
                    while (std::getline(ttp_data, str, eol) && index_line++ < 5) // Skip 2 lines, to line 11
                        ;
                    double cur_x, cur_y, init_x, init_y, distance; // Start reading location coordinates
                    int first_space = str.find_first_of('	'), second_space = str.find_last_of('	');
                    if (!Helper::is_double(str.substr(first_space + 1, second_space - first_space - 1), &init_x) ||
                        !Helper::is_double(str.substr(second_space + 1), &init_y))
                    {
                        IOH_DBG(warning, "Cannot read coordinates for PWT"); // FIXME raise an exception?
                        return 1; // return a valid dummy size
                    }
                    cur_x = init_x;
                    cur_y = init_y;
                    index_line = 0;
                    penalty = 0;
                    distances = new std::vector<double>();
                    weights = new std::vector<std::vector<double>>();
                    profits = new std::vector<std::vector<double>>();
                    index_map = new std::vector<std::vector<int>>();
                    while (std::getline(ttp_data, str, eol) && index_line++ < n_cities - 1) // Read until next header
                    {// Populate route distances and compute penalty term
                        first_space = str.find_first_of('	'), second_space = str.find_last_of('	');
                        double next_x, next_y;
                        if (!Helper::is_double(str.substr(first_space + 1, second_space - first_space - 1), &next_x) ||
                            !Helper::is_double(str.substr(second_space + 1), &next_y))
                        {
                            IOH_DBG(warning,  "Cannot read coordinates for PWT" )
                            return 1; // return a valid dummy size
                        }
                        distance =
                            std::ceil(std::sqrt(std::pow(next_x - cur_x, 2) + std::pow(next_y - cur_y, 2))); // CEIL_2D
                        distances->push_back(distance);
                        penalty -= distance;
                        cur_x = next_x;
                        cur_y = next_y;
                    } // End reading location coordinates
                    distance =
                        std::ceil(std::sqrt(std::pow(init_x - cur_x, 2) + std::pow(init_y - cur_y, 2))); // CEIL_2D
                    distances->push_back(distance);
                    penalty =
                        (penalty - distance) * rent_ratio / (velocity_max - velocity_gap); // Complete penalty term
                    index_line = 0;
                    while (std::getline(ttp_data, str, eol) && index_line < n_items)// Read item data
                    {
                        tstr = str.substr(str.find_first_of('	') + 1);
                        first_space = tstr.find_first_of('	');
                        second_space = tstr.find_last_of('	');
                        int city_index = std::stoi(tstr.substr(second_space + 1)) - 1;
                        while (static_cast<int>(weights->size()) <= city_index)
                        {
                            weights->push_back({});
                            profits->push_back({});
                            index_map->push_back({});
                        }
                        (*index_map)[city_index].push_back(index_line++);
                        double temp;
                        if (Helper::is_double(tstr.substr(0, first_space), &temp))
                        {
                            (*profits)[city_index].push_back(temp);
                        }
                        else
                        {
                            IOH_DBG(warning,  "Cannot read item profits for PWT" )
                            return 1; // return a valid dummy size
                        }
                        if (Helper::is_double(tstr.substr(first_space + 1, second_space - first_space - 1), &temp))
                        {
                            (*weights)[city_index].push_back(temp);
                        }
                        else
                        {
                            IOH_DBG(warning,  "Cannot read item weights for PWT" )
                            return 1; // return a valid dummy size
                        }
                    }
                    is_initialized = true;
                    return n_items; // Dimension is number of items
                }

            protected:
                //! Variables transformation method
                std::vector<int> transform_variables(std::vector<int> x) override { return x; }
                //! Objectives transformation method
                double transform_objectives(const double y) override { return y; }
                // [mandatory] The evaluate method is mandatory to implement
                double evaluate(const std::vector<int> &x) override
                {
                    double profit_sum = 0, cons = 0, time = 0;
                    for (size_t i = 0; i < weights->size(); i++)
                    {
                        for (size_t j = 0; j < (*weights)[i].size(); j++)
                        {
                            if (x[(*index_map)[i][j]] >= 1)
                            {
                                cons += (*weights)[i][j];
                                profit_sum += (*profits)[i][j];
                            }
                        }
                        if (cons > capacity) // Assuming weights are non-negative
                            continue; // Keep adding weights, but ignore time to avoid division by 0
                        time += (*distances)[i] / (velocity_max - (velocity_gap * cons / capacity));
                    }
                    if (cons > capacity)
                        return capacity - cons + penalty;
                    return profit_sum - time;
                }

            public:
                // Check if instance is null
                bool is_null() { return !distances || (!is_initialized) || distances->empty(); }

                // Get problem dimension from initialized instance
                int get_dim() { return is_null() ? 0 : n_items; }

                // Constructor
                PackWhileTravel(const int instance = 1, [[maybe_unused]] const int n_variables = 1,
                                const std::string &instance_list_file = Helper::instance_list_path.empty()
                                    ? "example_list_pwt"
                                    : Helper::instance_list_path) :
                    Integer(MetaData(instance + 3000000,// problem id, starting at 3000000
                        instance, "PackWhileTravel" + std::to_string(instance),
                        read_instance_by_id(instance - 1, instance_list_file), // n_variables will be updated based on the given instance.
                        // n_variables,
                        common::OptimizationType::Maximization),
                        Constraint<int>(read_instance_by_id(instance - 1, instance_list_file), 0, 1)
                    )
                {
                    if (is_null())
                    {
                        IOH_DBG(warning, "Instance not created properly (e.g. invalid id)."); // FIXME raise an exception?
                        return;
                    }
                    if (velocity_gap >= velocity_max || velocity_gap <= 0 || velocity_max <= 0)
                        throw std::invalid_argument(
                            "Minimum velocity must be positive and smaller than maximum velocity");
                    if (capacity <= 0)
                        throw std::invalid_argument("Capacity must be positive");
                    if (weights->size() != profits->size() || weights->size() != distances->size())
                        throw std::invalid_argument("Weights, profits, and number of cities don't match");
                    for (size_t i = 0; i < weights->size(); i++)
                    {
                        if ((*weights)[i].size() != (*profits)[i].size())
                            throw std::invalid_argument("Weights and profits don't match");
                    }
                    for (size_t i = 0; i < weights->size(); i++)
                    {
                        for (size_t j = 0; j < (*weights)[i].size(); j++)
                        {
                            if ((*weights)[i][j] < 0 || (*profits)[i][j] < 0)
                                throw std::invalid_argument("Weights and profits must be non-negative");
                        }
                    }
                    objective_.x = std::vector<int>(meta_data_.n_variables, 1);
                    objective_.y = evaluate(objective_.x);
                }
                // Constructor without n_variable, swap argument positions to avoid ambiguity
                PackWhileTravel(const std::string &instance_file = Helper::instance_list_path, const int instance = 1) :
                    PackWhileTravel(instance, 1, instance_file)
                {
                }

            };
        } // namespace submodular
    } // namespace problem
} // namespace ioh
