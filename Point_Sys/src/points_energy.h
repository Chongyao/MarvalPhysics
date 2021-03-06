#ifndef POINTS_ENERGY_H
#define POINTS_ENERGY_H
#include <utility>
#include <src/def.h>
#include <Eigen/Core>
#include <Eigen/SparseCore>
#include "get_nn.h"
#include "data_stream.h"
namespace marvel{


// using dat_ptr = std::shared_ptr<energy_dat>;

using dat_ptr = std::shared_ptr<dat_str_core<double, 3>>;

class point_sys : public Functional<double, 3>{
public:
  point_sys(const Eigen::MatrixXd &points, const double &rho, const double &Young, const double &Poission, const double &vol_all, const double &kv, const std::vector<std::vector<size_t>> &friends, const Eigen::VectorXd &sup_radi );
  size_t Nx() const ;

  int pre_compute(dat_ptr &data) const ;//calc once
  int calc_defo_gra(const double *disp, dat_ptr &data) const;
  int Val(const double *x, dat_ptr &data)const;
  int Gra(const double *x, dat_ptr &data) const;
  int Hes(const double*x, dat_ptr &data) const;
  
  int gravity(const double *disp, dat_ptr &data, const double &gravity) const;
  double get_mass(const size_t &i) const;

  const Eigen::SparseMatrix<double>& get_Mass_Matrix();
  const Eigen::VectorXd get_Mass_VectorXd();
  // int Gra(const double *x, double *gra) const;
  // int Val(const double *x, double *val) const;
  // int Gra(const double *x, double *gra) const;
  // int Hes(const double *x,  std::vector<Eigen::Triplet<double>> *hes) const;
 private:
  const size_t dim_;
  const Eigen::MatrixXd points_;
  Eigen::SparseMatrix<double> M_;

  const double rho_;
  const double Poission_;
  const double Young_;
  const double kv_;
  
  double vol_all_;
  double scal_fac_;
  // mutable spatial_hash SH_;


  int calc_rhoi_vi() const;
  int calc_weig() const;
  int calc_Mass_matrix();
  double kernel(const double &r, const double &h) const;
  double kernel(const size_t &i, const size_t &j) const;

  //point info
  Eigen::VectorXd mass_i_;
  Eigen::VectorXd sup_radi_;
  mutable Eigen::VectorXd rho_i_;
  mutable Eigen::VectorXd vol_i_;
  mutable std::vector<std::vector<size_t>> friends_;
  mutable std::vector<std::vector<double>> weig_;

};
}
#endif
