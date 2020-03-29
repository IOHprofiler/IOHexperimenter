#include "coco_transformation.hpp"

std::vector<double> Coco_Transformation_Data::raw_x = std::vector<double>(0);
std::vector<double> Coco_Transformation_Data::xopt = std::vector<double>(0);
std::vector<double> Coco_Transformation_Data::tmp1 = std::vector<double>(0);
std::vector<double> Coco_Transformation_Data::tmp2 = std::vector<double>(0);
double Coco_Transformation_Data::fopt = 0;
double Coco_Transformation_Data::penalty_factor = 0;
double Coco_Transformation_Data::factor = 0;
double Coco_Transformation_Data::lower_bound = 0;
double Coco_Transformation_Data::upper_bound = 0;
std::vector<std::vector<double> > Coco_Transformation_Data::M = std::vector<std::vector<double>>(0);
std::vector<double> Coco_Transformation_Data::b = std::vector<double>(0);;
std::vector<std::vector<double> > Coco_Transformation_Data::M1 = std::vector<std::vector<double>>(0);
std::vector<double> Coco_Transformation_Data::b1 = std::vector<double>(0);;
std::vector<std::vector<double> > Coco_Transformation_Data::rot1 = std::vector<std::vector<double>>(0);
std::vector<std::vector<double> > Coco_Transformation_Data::rot2 = std::vector<std::vector<double>>(0);
std::vector<double> Coco_Transformation_Data::datax = std::vector<double>(0);;
std::vector<double> Coco_Transformation_Data::dataxx = std::vector<double>(0);;
std::vector<double> Coco_Transformation_Data::minus_one = std::vector<double>(0);;
double Coco_Transformation_Data::condition = 0;
long Coco_Transformation_Data::rseed = 0;




