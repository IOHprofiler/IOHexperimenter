#pragma once



namespace ioh::common::pareto
{
            
    
    inline bool dominates( const std::vector<double> &a, const std::vector<double> &b, FOptimizationType optimization_type)
    {
        
        bool dominates = true;
        for (size_t i = 0; i < a.size(); ++i)
        {
            if (!optimization_type(a[i], b[i])) 
            {
                dominates = false;
                break;
            }
        }

        if (dominates)
        {
            for (size_t i = 0; i < a.size(); ++i)
            {
                if (a[i] != b[i])
                {
                    return true; // a dominates b if at least one objective is strictly better
                }
            }
        }



        return dominates;
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