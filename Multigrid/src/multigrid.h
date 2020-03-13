#ifndef MARVEL_MULTIGRID
#define MARVEL_MULTIGRID

#include<vector>
#include<memory>
#include<Eigen/Dense>
#include<Eigen/Sparse>
#include<data_str_core.h>

namespace marvel{

template<typename T>
using VS = std::vector<std::shared_ptr<T>>;
using SPM = Eigen::SparseMatrix<double, Eigen::RowMajor>;

struct transfer {
  transfer(const SPM& I, const SPM&R);
  SPM I_;
  SPM R_;
};

struct layer{
  layer(const SPM& A, const bool if_direct, const size_t itrs);
  int solve();
  Eigen::VectorXd get_residual()const;
  
  const SPM A_;
  Eigen::VectorXd rhs_;
  Eigen::VectorXd u_;
  const bool if_direct_;
  const size_t itrs_;

};

class multigrid_process{
 public:
  multigrid_process(const std::vector<int>& process, VS<layer>& layers, const VS<transfer>& transfers);
  int execute(double* solution);
  
  
 private:
  int relax();
  int restrict();
  int correct();
  
  std::vector<int> process_;
  VS<layer> layers_;
  VS<transfer> transfers_;
  
  const size_t dof_;
  const SPM A_h_;
  const Eigen::VectorXd b_h_;
  
  size_t layer_id_{0};

  
};

}
#endif
