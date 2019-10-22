// We can now use the BH package
// [[Rcpp::depends(BH)]]

#include <Rcpp.h>
#include <boost/math/common_factor.hpp>  
#include <boost/filesystem.hpp>

using namespace Rcpp;
namespace fs = boost::filesystem;

// [[Rcpp::export]]
int computeGCD(int a, int b) {
  return boost::math::gcd(a, b);
}

// [[Rcpp::export]]
int computeLCM(int a, int b) {
  return boost::math::lcm(a, b);
}

// [[Rcpp::export]]
int filetest(){
  if (fs::create_directory("folder_name")) {
    return 1;
  } else {
    return 0;
  }
}