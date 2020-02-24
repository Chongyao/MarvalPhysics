#ifndef ELAS_ENERGY
#define ELAS_ENERGY
#include "def.h"
#include "data_str_core.h"
#include "constitutive.h"
#include "gaussian_quadrature.h"
#include "basis_func.h"

#include <Eigen/Dense>
#include "eigen_ext.h"
#include <iostream>
#include <set>
#include <fstream>
namespace marvel{

using namespace Eigen;
using namespace std;

template<typename T>
inline void compute_lame_coeffs(const T Ym, const T Pr,
                                T &mu, T &lambda) {
  mu = Ym/(2*(1+Pr));
  lambda = Ym*Pr/((1+Pr)*(1-2*Pr));
}


template<typename T, size_t dim_, size_t num_per_cell_, size_t bas_order_, size_t qdrt_axis_,
         template<typename, size_t> class CSTTT,  // constituitive function
         template<typename, size_t, size_t, size_t > class BASIS, //  basis
         template<typename, size_t, size_t, size_t> class QDRT> //  
class BaseElas : public Functional<T, dim_>{
  using basis = BASIS<T, dim_, bas_order_, num_per_cell_>;
  using csttt = CSTTT<T, dim_>;
  using qdrt = QDRT<T, dim_, qdrt_axis_, num_per_cell_>;
  
 public:
  BaseElas(const Eigen::Matrix<T, dim_, -1>& nods, const Eigen::Matrix<int, num_per_cell_, -1>& cells, const T& ym, const T&poi):
      all_dim_(nods.size()), num_nods_(nods.cols()), num_cells_(cells.cols()) ,
      nods_(nods), cells_(cells), all_rows_(Matrix<int, dim_, 1>::LinSpaced(dim_, 0, dim_ -1)), quadrature_(), num_qdrt_(static_cast<size_t>(pow(qdrt_axis_, dim_))){
    static_assert(std::is_base_of<elas_csttt<T, dim_>, csttt>::value, "CSTTT must derive from elas_csttt");
    static_assert(std::is_base_of<basis_func<T, dim_, bas_order_, num_per_cell_>, basis>::value, "BASIS must derive from basis_func");
    static_assert(std::is_base_of<quadrature<T, dim_, qdrt_axis_, num_per_cell_>, qdrt>::value, "GAUS must derive from gaus_quad");
    //set mtr
    T mu, lambda;
    compute_lame_coeffs(ym, poi, mu, lambda);
    mtr_.resize(2, cells_.cols());
    mtr_.row(0) = Eigen::Matrix<T, 1, -1>::Ones(cells_.cols()) * lambda;
    mtr_.row(1) = Eigen::Matrix<T, 1, -1>::Ones(cells_.cols()) * mu;

    PreComputation();
  }
  
  size_t Nx() const {return all_dim_;}
    
  int Val(const T *x, std::shared_ptr<dat_str_core<T,dim_>>& data) const {
    Eigen::Map<const Eigen::Matrix<T, -1 ,-1>> deformed(x, dim_, num_nods_ );
    T myval = 0;
    #pragma omp parallel for
    for(size_t cell_id = 0; cell_id < num_cells_ ; ++cell_id){
      Matrix<T, dim_, dim_> def_gra;
      
      const Matrix<T, dim_, num_per_cell_> x_cell = indexing(deformed, all_rows_, cells_.col(cell_id));
      const Matrix<T, dim_, num_per_cell_> X_cell = indexing(nods_, all_rows_, cells_.col(cell_id));
      //TODO:considering the order of basis
      
      for(size_t qdrt_id = 0; qdrt_id < num_qdrt_; ++qdrt_id){
        basis::get_def_gra(Dphi_Dxi_[cell_id][qdrt_id], x_cell.data(), Dm_inv_[cell_id][qdrt_id], def_gra);
        data->save_val(csttt::val(def_gra, mtr_(0, cell_id), mtr_(1, cell_id))  * quadrature_.WGT_[qdrt_id] * Jac_det_[cell_id][qdrt_id]);
      }
    }

    return 0;
  }
  
  int Gra(const T *x, std::shared_ptr<dat_str_core<T,dim_>>& data) const {
    Eigen::Map<const Eigen::Matrix<T, -1, -1>> deformed(x, dim_, num_nods_);

    #pragma omp parallel for
    for(size_t cell_id = 0; cell_id < num_cells_ ; ++cell_id){
      Matrix<T, dim_, dim_> def_gra;
      
      const Matrix<T, dim_, num_per_cell_> x_cell = indexing(deformed, all_rows_, cells_.col(cell_id));
      const Matrix<T, dim_, num_per_cell_> X_cell = indexing(nods_, all_rows_, cells_.col(cell_id));

      Matrix<T, dim_ * dim_, 1> gra_F_based;
      Matrix<T, dim_ * num_per_cell_, 1> gra_x_based = Matrix<T, dim_ *  num_per_cell_, 1>::Zero();


      //TODO:considering the order of basis
      for(size_t qdrt_id = 0; qdrt_id < num_qdrt_; ++qdrt_id){
        
        basis::get_def_gra(Dphi_Dxi_[cell_id][qdrt_id], x_cell.data(), Dm_inv_[cell_id][qdrt_id], def_gra);
        gra_F_based = csttt::gra(def_gra, mtr_(0, cell_id), mtr_(1, cell_id));
        gra_x_based += Ddef_Dx_[cell_id][qdrt_id].transpose() * gra_F_based * quadrature_.WGT_[qdrt_id] * Jac_det_[cell_id][qdrt_id];
      }

      //save gra
      const Eigen::Map<Matrix<T, dim_, num_per_cell_> > gra_x_based_reshape(gra_x_based.data());
      for(size_t p = 0; p < num_per_cell_; ++p){
        data->save_gra(cells_(p, cell_id), gra_x_based_reshape.col(p));
      }
    }

    return 0;
  }
  int Hes(const T *x, std::shared_ptr<dat_str_core<T,dim_>>& data) const {
    Eigen::Map<const Eigen::Matrix<T, -1, -1>> deformed(x, dim_, num_nods_);

    #pragma omp parallel for
    for(size_t cell_id = 0; cell_id < num_cells_ ; ++cell_id){

      Matrix<T, dim_, dim_> def_gra;
      const Matrix<T, dim_, num_per_cell_> x_cell = indexing(deformed, all_rows_, cells_.col(cell_id));
      const Matrix<T, dim_, num_per_cell_> X_cell = indexing(nods_, all_rows_, cells_.col(cell_id));

      Matrix<T, dim_ * num_per_cell_, 1> gra_x_based = Matrix<T, dim_ *  num_per_cell_, 1>::Zero();
      Matrix<T, dim_ * dim_, dim_ * dim_> hes_F_based; 
      Matrix<T, dim_ * num_per_cell_, dim_ * num_per_cell_> hes_x_based; hes_x_based.setZero();


      //TODO:considering the order of basis
      for(size_t qdrt_id = 0; qdrt_id < num_qdrt_; ++qdrt_id){
        basis::get_def_gra(Dphi_Dxi_[cell_id][qdrt_id], x_cell.data(), Dm_inv_[cell_id][qdrt_id], def_gra);
        hes_F_based = csttt::hes(def_gra, mtr_(0, cell_id), mtr_(1, cell_id));
        hes_x_based += Ddef_Dx_[cell_id][qdrt_id].transpose() * hes_F_based * Ddef_Dx_[cell_id][qdrt_id] * quadrature_.WGT_[qdrt_id] * Jac_det_[cell_id][qdrt_id];
      }
      //save hes
      for(size_t p = 0; p < dim_ * num_per_cell_; ++p){
        for(size_t q = 0; q < dim_ * num_per_cell_; ++q){
          const size_t I = cells_(p / dim_, cell_id) * dim_ + p%dim_;
          const size_t J = cells_(q / dim_, cell_id) * dim_ + q%dim_;
          // if(hes_x_based(p, q))
          data->save_hes(I, J, hes_x_based(p, q));
        }
      }
    }
     return 0;
     

  }


protected:
  void PreComputation() {
    Dm_inv_.resize(num_cells_);
    Jac_det_.resize(num_cells_);
    Ddef_Dx_.resize(num_cells_);
    Dphi_Dxi_.resize(num_cells_);
    
    Eigen::Matrix<T, dim_, dim_> Dm_inv_tmp;
    T Jac_det_tmp;
    Eigen::Matrix<T, dim_ * dim_, dim_ * num_per_cell_> Ddef_Dx_tmp;
    Eigen::Matrix<T, num_per_cell_, dim_> Dphi_Dxi_tmp;
    
    for(size_t cell_id = 0; cell_id < num_cells_ ; ++cell_id){
      const Matrix<T, dim_, num_per_cell_> X_cell = indexing(nods_, all_rows_, cells_.col(cell_id));
      for(size_t qdrt_id = 0; qdrt_id < num_qdrt_; ++qdrt_id) {
        basis::calc_Dphi_Dxi(quadrature_.PNT_.col(qdrt_id), X_cell.data(), Dphi_Dxi_tmp);
        Dphi_Dxi_[cell_id].push_back(Dphi_Dxi_tmp);
        
        basis::calc_InvDm_Det(Dphi_Dxi_tmp, X_cell.data(), Jac_det_tmp, Dm_inv_tmp);
        Jac_det_[cell_id].push_back(Jac_det_tmp);
        Dm_inv_[cell_id].push_back(Dm_inv_tmp);
        basis::get_Ddef_Dx(Dphi_Dxi_tmp, Dm_inv_tmp, Ddef_Dx_tmp);
        Ddef_Dx_[cell_id].push_back(Ddef_Dx_tmp);
      }
    }
    return;
  }
  
 private:
  const size_t all_dim_, num_nods_, num_cells_, num_qdrt_;
  Eigen::Matrix<T, 2, -1> mtr_;
  const Eigen::Matrix<T, dim_, -1> nods_; // vertices
  const Eigen::Matrix<int, num_per_cell_, -1> cells_; // elements
  Matrix<int, dim_, 1> all_rows_;
  const qdrt quadrature_;

  
private:  // precomputed values
  std::vector<std::vector<Eigen::Matrix<T, dim_, dim_>>> Dm_inv_;
  std::vector<std::vector<T>> Jac_det_;
  std::vector<std::vector<Eigen::Matrix<T, dim_ * dim_, dim_ * num_per_cell_>, Eigen::aligned_allocator<Eigen::Matrix<T, dim_ *dim_, dim_ * num_per_cell_>>>> Ddef_Dx_;
  std::vector<std::vector<Eigen::Matrix<T, num_per_cell_, dim_>>> Dphi_Dxi_;
  
};
}
#endif
