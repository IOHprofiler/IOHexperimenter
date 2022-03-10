#pragma once
#include <stdexcept>
#include <fstream>
#include "pbo_problem.hpp"

namespace ioh::problem::pbo
{
    std::vector<std::string> meta_list_pwt;
    int read_meta_list_instance(const bool reread = false, const std::string &path_to_meta_list_instance = "example_list_pwt")
    {
        if (meta_list_pwt.empty() || reread) // Only read if unread, or forced reread
        {
            std::vector<std::string> l{};
            std::ifstream list_data(path_to_meta_list_instance);
            std::string str{};
            while (std::getline(list_data, str))
            {
                if (str.empty()) // Skip over empty lines
                    continue;
                l.push_back(str);
            }
            meta_list_pwt = l;
        }
        return meta_list_pwt.size();
    }
    class PackWhileTravel final : public PBOProblem<PackWhileTravel>
    {
        double velocity_gap, velocity_max, capacity, penalty;
        std::vector<double> distances;
        std::vector<std::vector<double>> weights, profits;
        std::vector<std::vector<double>> index_map;

        // Read TTP instance from file, convert to PWT instance, return problem dimension
        int read_instance_by_id(const int instance)
        {
            int list_size = read_meta_list_instance();
            if (list_size <= instance)
                return -1;
            std::ifstream ttp_data(meta_list[instance]);
            std::string str, tstr;
            int index_line = 0;
            while (std::getline(ttp_data, str) && index_line++ < 2)// Skip 2 lines, to line 3
                ;
            int n_cities = std::stoi(str.substr(str.find_last_of(':') + 1));
            std::getline(ttp_data, str);
            int n_items = std::stoi(str.substr(str.find_last_of(':') + 1));
            std::getline(ttp_data, str);
            capacity = std::stod(str.substr(str.find_last_of(':') + 1));
            std::getline(ttp_data, str);
            velocity_gap = std::stod(str.substr(str.find_last_of(':') + 1));
            std::getline(ttp_data, str);
            velocity_max = std::stod(str.substr(str.find_last_of(':') + 1));
            velocity_gap = velocity_max - velocity_gap;
            std::getline(ttp_data, str);
            double rent_ratio = std::stod(str.substr(str.find_last_of(':') + 1));
            while (std::getline(ttp_data, str) && index_line++ < 4) // Skip 2 lines, to line 11
                ;
            weights = std::vector<std::vector<double>>(n_cities, {});
            profits = std::vector<std::vector<double>>(n_cities, {});
            index_map = std::vector<std::vector<int>>(n_cities, {});
            distances = std::vector<double>(n_cities, 0);
            double cur_x, cur_y, next_x, next_y, init_x, init_y, distance;
            int first_space = str.find_first_of('	'), second_space = str.find_last_of('	');
            init_x = std::stod(str.substr(first_space + 1, second_space - first_space - 1));
            init_y = std::stod(str.substr(second_space + 1));
            cur_x = init_x;
            cur_y = init_y;
            index_line = 0;
            penalty = 0;
            while (std::getline(ttp_data, str) && index_line < n_cities - 1)// Read until next header
            {
                first_space = str.find_first_of('	'), second_space = str.find_last_of('	');
                next_x = std::stod(str.substr(first_space + 1, second_space - first_space - 1));
                next_y = std::stod(str.substr(second_space + 1));
                distance = std::ceil(std::sqrt(std::pow(next_x - cur_x, 2) + std::pow(next_y - cur_y, 2)));// CEIL_2D
                distances[index_line++] = distance;
                penalty -= distance;
                cur_x = next_x;
                cur_y = next_y;
            }
            distance = std::ceil(std::sqrt(std::pow(init_x - cur_x, 2) + std::pow(init_y - cur_y, 2)));// CEIL_2D
            distances[index_line] = distance;
            penalty = (penalty - distance) * rent_ratio / (velocity_max - velocity_gap);// Complete penalty term
            index_line = 0;
            int city_index;
            while (std::getline(ttp_data, str) && index_line < n_items)
            {
                tstr = str.substr(str.find_first_of('	'));
                first_space = tstr.find_first_of('	');
                second_space = tstr.find_last_of('	');
                city_index = std::stoi(tstr.substr(second_space + 1)) - 1;
                index_map[city_index].push_back(index_line++);
                profits[city_index].push_back(std::stod(tstr.substr(0, first_space)));
                weights[city_index].push_back(std::stod(tstr.substr(first_space + 1, second_space - first_space - 1)));
            }
            return n_items;// Dimension is number of items
        }
    protected:
        // [mandatory] The evaluate method is mandatory to implement
        double evaluate(const std::vector<int> &x) override
        { 
            double profit_sum = 0, cons = 0, time = 0;
            for (auto i = 0; i < weights.size(); i++)
            {
                for (auto j = 0; j < weights[i].size(); j++)
                {
                    if (x[index_map[i][j]] >= 1)
                    {
                        cons += weights[i][j];
                        profit_sum += profits[i][j];
                    }
                }
                if (cons > capacity) // Assuming weights are non-negative
                    continue;// Keep adding weights, but ignore time to avoid division by 0
                time += distance[i] / (velocity_max - (velocity_gap * cons / capacity));
            }
            if (cons > capacity)
                return capacity - cons + penalty;
            return profit_sum - time;
        }

    public:
        PackWhileTravel(const int instance, const int n_variables) :
            PBOProblem(
                104, // problem id, which will be overwritten when registering this class in all pseudo-Boolean problems
                instance, // the instance id
                read_instance_by_id(instance - 1), // dimensions
                "PackWhileTravel" // problem name
            )
        {
            if (velocity_gap >= velocity_max || velocity_gap <= 0 || velocity_max <= 0)
                throw std::invalid_argument("Minimum velocity must be positive and smaller than maximum velocity");
            if (capacity <= 0)
                throw std::invalid_argument("Capacity must be positive");
            if (weights.size() != profits.size() || weights.size() != distances.size())
                throw std::invalid_argument("Weights, profits, and number of cities don't match");
            for (int i = 0; i < weights.size(); i++)
            {
                if (weights[i].size() != profits[i].size())
                    throw std::invalid_argument("Weights and profits don't match");
            }
            for (int i = 0; i < weights.size(); i++)
            {
                for (int j = 0; j < weights[i].size(); j++)
                {
                    if (weights[i][j] < 0 || profits[i][j] < 0)
                        throw std::invalid_argument("Weights and profits must be non-negative");
                }
            }
            objective_.x = std::vector<int>(graph.get_n_vertices(), 1);
            objective_.y = evaluate(objective_.x);
        }
    };
} // namespace ioh::problem::pbo