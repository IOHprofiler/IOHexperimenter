#pragma once



namespace ioh::common::pareto
{
            
    /** Functions to detemrine if a solution is dominated, dominate or is part of pareto front*/
    inline bool dominates( const std::vector<double> &a, const std::vector<double> &b, FOptimizationType optimization_type)
    {
        
        bool better = false;
        for (size_t i = 0; i < a.size(); ++i)
        {
            if (optimization_type(b[i], a[i])) return false;
            if (optimization_type(a[i], b[i])) better = true;
        }
        return better;
    }


    inline bool vector_difference( const std::vector<double> &a, const std::vector<double> &b)
    {
        double max_difference = 0.0;
        for (size_t i = 0; i < a.size(); ++i)
        {
            double difference = std::abs(a[i] - b[i]);
            if (difference > max_difference)
            {
                max_difference = difference;
            }
        }
        return max_difference;
    }
}