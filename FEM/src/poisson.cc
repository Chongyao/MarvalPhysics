#include "poisson.h"

namespace marvel{
using namespace std;
using namespace Eigen;

POI_TEMP
POI_CLASS::poisson(const Eigen::Matrix<T, dim_, -1>& nods, const Eigen::Matrix<int, num_per_cell_, -1>& cells, const Eigen::Matrix<T, -1, 1>& k):
    nods_(nods),cells_(cells),k_(k), num_nods_(nods.cols()), num_cells_(cells.cols()),quadrature_(), all_rows_(Matrix<int, 1, 1>::Ones()),
num_qdrt_(static_cast<size_t>(pow(qdrt_axis_, dim_))){
  assert(k.size() == num_cells_);
  static_assert(std::is_base_of<basis_func<T, , bas_order_, num_per_cell_>, basis>::value, "BASIS must derive from basis_func");
  static_assert(std::is_base_of<quadrature<T, dim_, qdrt_axis_, num_per_cell_>, qdrt>::value, "GAUS must derive from gaus_quad");
  PreComputation();
}

POI_TEMP
size_t POI_CLASS::Nx() const{
  return num_nods_;
}



POI_TEMP
void POI_CLASS::PreComputation(){
  Dm_inv_.resize(num_cells_);
  Jac_det_.resize(num_cells_);
  Dphi_Dxi_.resize(num_cells_);
    
  Eigen::Matrix<T, dim_, dim_> Dm_inv_tmp;
  T Jac_det_tmp;
  Eigen::Matrix<T, num_per_cell_, dim_> Dphi_Dxi_tmp;

 Dphi_Dxi_.resize(num_cells_);
 #pragma omp parallel for
 for(size_t cell_id = 0; cell_id < num_cells_; ++cell_id){
   const Matrix<T, dim_, num_per_cell_> X_cell = indexing(nods_, all_rows_, cells_.col(cell_id));
   for(size_t qdrt_id = 0; qdrt_id < num_qdrt_; ++qdrt_id){
     basis::calc_Dphi_Dxi(quadrature_.PNT_.col(qdrt_id), X_cell.data(), Dphi_Dxi_tmp);
     Dphi_Dxi_[cell_id].push_back(Dphi_Dxi_tmp);
   }
 }
 return;
}

POI_TEMP
int POI_CLASS::Val(const T *x, std::shared_ptr<dat_str_core<T,dim_>>& data)const{
  Eigen::Map<const Eigen::Matrix<T, -1 ,-1>> u(x, 1, num_nods_ );
  #pragma omp parallel for
  for(size_t cell_id = 0; cell_id < num_cells_; ++cell_id){
    const Matrix<T, dim_, num_per_cell_> u_cell = indexing(u, all_rows_, cells_.col(cell_id));
    for(size_t qdrt_id = 0; qdrt_id < num_qdrt_; ++qdrt_id){
      Matrix<T, 1, dim_> gra_u = u_cell * Dphi_Dxi_[cell_id][qdrt_id];
      T val = 0.5 * k_(cell_id) * (gra_u.array() * gra_u.array()).sum();
      data->save_val(val * quadrature_.WGT_[qdrt_id])
    }
  }
  return 0;
}



}
