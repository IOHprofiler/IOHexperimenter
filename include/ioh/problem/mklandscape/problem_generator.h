#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

///The CliqueTree struct with properties input parameters, clique variable indices, the used codomain function, codomain values, global optimum strings and score
struct CliqueTree;

///Struct to contain the input parameters of the TD Mk Landscape:
/// Number of cliques/subfunctions M,
/// size k of each clique/subfunction,
/// number of overlapping variables between cliques/subfunctions o,
/// number of branches in the clique tree / tree decomposition b
struct InputParameters {
  uint32_t m;
  uint32_t k;
  uint32_t o;
  uint32_t b;
};

///Enum to represent various codomain classes
struct CodomainFunction {
  enum class Tag {
    Random,
    Trap,
    DeceptiveTrap,
    NKq,
    NKp,
    ///Combination of random and deceptive trap, where every clique/subfunction has probability p_deceptive to be deceptive and (1 - p_deceptive) to be random
    RandomDeceptiveTrap,
    Unknown,
  };

  struct NKq_Body {
    uint32_t q;
  };

  struct NKp_Body {
    double p;
  };

  struct RandomDeceptiveTrap_Body {
    double p_deceptive;
  };

  Tag tag;
  union {
    NKq_Body n_kq;
    NKp_Body n_kp;
    RandomDeceptiveTrap_Body random_deceptive_trap;
  };
};

extern "C" {

/// Construct CliqueTree (which represents the TD Mk Landscape) using the input parameters (M, k, o, b)
///   and the codomain function to be used to generate the codomain.
/// It returns a pointer to the (opaque) CliqueTree struct, which we can subsequently use to evaluate solutions,
///   get the global optima, and drop/destruct the CliqueTree.
CliqueTree *construct_clique_tree(InputParameters input_parameters,
                                  CodomainFunction codomain_function);

/// Construct CliqueTree (which represents the TD Mk Landscape) using the input parameters (M, k, o, b)
///   and the custom codomain values. The codomain has 2^k entries for each clique (out of M in total).
/// It returns a pointer to the (opaque) CliqueTree struct, which we can subsequently use to evaluate solutions,
///   get the global optima, and drop/destruct the CliqueTree.
/// Importantly, the codomain that was passed (using the pointer) can be freed/deleted, as we copy the codomain.
CliqueTree *construct_clique_tree_custom_codomain(InputParameters input_parameters,
                                                  const double *const *codomain);

/// Delete/drop/free the CliqueTree struct (memory)
void free_clique_tree(CliqueTree *clique_tree_ptr);

/// Evaluate a given solution
double evaluate_solution(CliqueTree *clique_tree_ptr, const int32_t *solution, uintptr_t len);

/// Get the number of global optima for this TD Mk Landscape problem
uintptr_t get_number_of_global_optima(CliqueTree *clique_tree_ptr);

/// Get the global optimum/optima score
double get_score_of_global_optima(CliqueTree *clique_tree_ptr);

/// Write the global optima of the problem to the memory pointed to by the passed pointer.
/// Then, one can check at C/C++ side whether a global optimum has been found
void write_global_optima_to_pointer(CliqueTree *clique_tree_ptr, int32_t *const *glob_opt_ptr);

} // extern "C"
