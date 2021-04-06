# IOHexperimenter : W-model

W-model introduces different characteristic features such as redundancy, neutrality, epsitasis, and ruggedness into existing problems by adding layers. We provide OneMax and LeadingOnes based w-model problems, and our layer approaches are introduced. Practically, <i>redundancy</i>, <i>neutrality</i>, and <i>epistasis</i> layers are applied to transform the input bit string in order, after getting the fitness value `f` of the transformed bit string on OneMax (or LeadingOnes), a <i>ruggedness</i> layer is introduced on `f` and returns a transformed objective value. 

Before testing OneMax and LeadingOnes based w-model problems, please set up w-model layers using the function below. Also, you can find an usage case in `_run_w_model()` of the [example](https://github.com/IOHprofiler/IOHexperimenter/blob/master/build/Cpp/IOHprofiler_run_problem.cpp) 
```cpp
/// \fn void set_w_setting(const double dummy_para, const int epistasis_para, const int neutrality_para, const int ruggedness_para)
/// \brief A function to configure w-model layers.
/// \param dummy_para percentage of valid bits.
/// \param neutrality_para size of sub string for neutrality
/// \param epistasis_para size of sub string for epistasis
/// \param ruggedness_para gamma for ruggedness layper
///
/// For details of w-model, please read https://www.sciencedirect.com/science/article/pii/S1568494619308099
/// and https://dl.acm.org/doi/pdf/10.1145/3205651.3208240
void set_w_setting(const double dummy_para, const int epistasis_para, const int neutrality_para, const int ruggedness_para)
```

## Redundancy (Dummy variables)
Redundancy is introduced by a redunction function mapping the original bit string into a substring. Substring bits are pairwise different randomly chosen from the original string. `dummy_para` sets the percentage of bits to be chosen.

## Neutrality
Neutrality layer reduces the original bit string into a new string, by mapping sub strings of size ![](http://latex.codecogs.com/gif.latex?\\mu) to the value of majority. `neutrality_para` sets the block size ![](http://latex.codecogs.com/gif.latex?\\mu).

## Epistatis
Epistasis layer introduces a bijective function to map sub strings of the orignal string to new strings. `epistasis_para` sets the size if sub strings ![](http://latex.codecogs.com/gif.latex?\\nu) on which the bijective function performs.

## Ruggedness
Ruggendess layer introduces a bijective function to map the fitness value on the base problem (such as OneMax) to a new value. In the meantime, the optimal value remains the same. A translate function is applied to tune ruggedness of the problem. `ruggedness_para` sets the degree of ruggedness ![](http://latex.codecogs.com/gif.latex?\\gamma). ![](http://latex.codecogs.com/gif.latex?\\gamma=0). indicates that no ruggedness is introduced. 