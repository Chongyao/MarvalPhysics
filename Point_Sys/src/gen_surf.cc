#include "gen_surf.h"
#include <Eigen/Core>
#include <cmath>


using namespace Eigen;
namespace marvel{

/********************************CLASS deform_surf******************************/
template<typename T>
deform_surf<T>::deform_surf(const MatrixXi &surf, const Matrix<T,-1,-1> &nods, const Matrix<T, -1, -1> &sam_points):surf_(surf), nods_(nods), sam_points_(sam_points){}
/********************************CLASS deform_surf******************************/

/********************************CLASS deform_surf_MLS******************************/
template<typename T>
deform_surf_MLS<T>::deform_surf_MLS(const MatrixXi &surf, const Matrix<T,-1,-1> &nods, const Matrix<T, -1, -1> &sam_points, const std::vector<std::vector<size_t>> &friends, const T &kernel_cof):deform_surf<T>(surf, nods, sam_points), friends_(friends), kernel_cof_(kernel_cof){
  
}

template<typename T>
T deform_surf_MLS<T>::kernel(const size_t &vertex_id, const size_t &point_id, const T &h) const {
  T r = (this->sam_points_.col(point_id) - this->nods_.col(vertex_id)).norm();
  return exp(-r*r/h/h);
}

template<typename T>
T deform_surf_MLS<T>::kernel(const T &r, const T &h) const {
  return exp(-r*r/h/h);
}

template<typename T>
Matrix<T, -1, -1> deform_surf_MLS<T>::update_surf(const Matrix<T, -1 ,-1>& moved_points_dis, const Matrix<T, -1, -1> &dis_gra_all) const {
  Matrix<T, 3, -1> nods_dis(this->nods_.cols());
  for(size_t i = 0; i < this->nods_.size(); ++i){
    Matrix<T, 3, 1> ver_dis;
    T scal_fac = 0;
    ver_dis.setZero(3, 1);
    
    for(size_t j = 0; j < friends_[i].size(); ++j){
      T rij = this->sam_points_.col(j) - this->nods_.col(i);
      T gas_weig = kernel(rij.norm(), kernel_cof_);
      
      scal_fac += gas_weig;
      Map<Matrix<T, -1, -1>> dis_gra(dis_gra_all.col(j), 3, 3);
      ver_dis += gas_weig(moved_points_dis.col(i) + dis_gra.transpose()*rij);
    }//every friend

    nods_dis.col(i) = ver_dis/scal_fac;
  }//every nod

  return nods_dis;

}


/********************************CLASS deform_surf_MLS******************************/


/********************************CLASS deform_surf_LI******************************/
template<typename T>  
deform_surf_LI<T>::deform_surf_LI(const MatrixXi &surf, const Matrix<T, -1, -1> &nods, const Matrix<T, -1, -1> &sam_points, const MatrixXi &four_NN):surf_(surf), nods_(nods), sam_points_(sam_points), four_NN_(four_NN){
  for(size_t i = 0; i < nods.cols(); ++i){
    Matrix3d basis;
    for(size_t j = 0; j < 3; ++j){
      basis.col(i) = sam_points_.col(four_NN(j, i)) - sam_points.col(four_NN_(3, i));
    }
  }
}//function:deform_surf_LI()

// deform
template<typename T>
Matrix<T, -1, -1>& deform_surf_LI<T>::update_surf(const Matrix<T, -1, -1> &moved_points) const{
  
}


/********************************CLASS deform_surf_LI******************************/

}//namespace: marvel
