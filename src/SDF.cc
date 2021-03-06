#include <Eigen/Core>
#include <iostream>
#include "DEFINE_TYPE.h"
#include "SDF.h"

namespace marvel {

using namespace std;
using namespace Eigen;
//===============================================================================
template<typename T, size_t dim_>
planeSDF<T,dim_>::planeSDF(const T *center, const T *n) {
  C_.setZero();C_<< center[0], center[1],center[2];
  N_.setZero();N_<< n[0], n[1], n[2];
  cout << C_ << endl << N_<<endl;
}

template<typename T, size_t dim_>
bool planeSDF<T,dim_>::inside(const T *x) const {
  Eigen::Map<const Matrix<T, dim_, 1>> X(x);
  return (X-C_).dot(N_) < 0.0;
}
template<typename T, size_t dim_>
void planeSDF<T,dim_>::Val(const T *x, T *val) const {
  Eigen::Map<const Matrix<T, dim_, 1>> X(x);
  *val = pow((X-C_).dot(N_), 2);
}
template<typename T, size_t dim_>
void planeSDF<T,dim_>::Gra(const T *x, T *gra) const {
  Eigen::Map<const Matrix<T, dim_, 1>> X(x);
  Eigen::Map<Matrix<T, dim_, 1>> G(gra);
  G = 2*(X-C_).dot(N_)*N_;
}
template<typename T, size_t dim_>
void planeSDF<T,dim_>::Hes(const T *x, T *hes) const {
  Eigen::Map<Matrix<T, dim_, dim_>> H(hes);
  H = 2*N_*N_.transpose();
}

template<typename T, size_t dim_>
void planeSDF<T,dim_>:: update_center_position(const T* delt_x){
  Eigen::Map<const Matrix<T, dim_, 1>> delt_X(delt_x);
  C_ += delt_X;
}
//===============================================================================

//===============================================================================
template<typename T, size_t dim_>
sphereSDF<T, dim_>::sphereSDF(const T *center, const T r)
    : C_(Matrix<T, dim_, 1>(center)), R_(r) {}

template<typename T, size_t dim_>
bool sphereSDF<T, dim_>::inside(const T *x) const {
  Eigen::Map<const Matrix<T, dim_, 1>> X(x);
  return (X-C_).norm()-R_ < 0;
}

template<typename T, size_t dim_>
void sphereSDF<T, dim_>::Val(const T *x, T* val) const {
  Eigen::Map<const Matrix<T, dim_, 1>> X(x);
  *val = pow((X-C_).norm()-R_, 2);
}
template<typename T, size_t dim_>
void sphereSDF<T, dim_>::Gra(const T *x, T* gra) const {
  Eigen::Map<const Matrix<T, dim_, 1>> X(x);
  Eigen::Map<Matrix<T, dim_, 1>> G(gra);
  G = 2*(X-C_-R_*(X-C_)/(X-C_).norm());
}

template<typename T, size_t dim_>
void sphereSDF<T, dim_>::Hes(const T *x, T* hes) const {
  Eigen::Map<const Matrix<T, dim_, 1>> X(x);
  Eigen::Map<Matrix<T, dim_, dim_>>H(hes);
  Matrix<T, dim_, 1> J = (X-C_)/(X-C_).norm();
  H = 2*J*J.transpose();
}

template<typename T, size_t dim_>
void sphereSDF<T,dim_>:: update_center_position(const T* delt_x){
  Eigen::Map<const Matrix<T, dim_, 1>> delt_X(delt_x);
  C_ += delt_X;
}
//===============================================================================
template<typename T, size_t dim_> geom_contact_energy<T, dim_>::
geom_contact_energy(const vector<shared_ptr<signed_dist_func<T, dim_>>> &objs,
                    const size_t dof, const T w)
    : objs_(objs), dof_(dof), w_(w) {}

template<typename T, size_t dim_>
size_t geom_contact_energy<T, dim_>::Nx() const {
  return dof_ * dim_;
}

template<typename T, size_t dim_>
int geom_contact_energy<T, dim_>::Val(const T *x, data_ptr<T, dim_> &data) const {

  #pragma omp parallel for
  for (size_t i = 0; i < dof_; ++i) {
    for (size_t j = 0; j < objs_.size(); ++j) {
      if ( !objs_[j].get() )
        continue;
      if ( objs_[j]->inside(x+dim_*i) ) {
        T value = 0;
        objs_[j]->Val(x+dim_*i, &value);
        data->save_val(w_*value);
      }
    }
  }
  return 0;
}

template<typename T, size_t dim_>
int geom_contact_energy<T, dim_>::Gra(const T *x, data_ptr<T, dim_> &data) const {

  #pragma omp parallel for
  for (size_t i = 0; i < dof_; ++i) {
    for (size_t j = 0; j < objs_.size(); ++j) {
      if ( !objs_[j].get() )
        continue;
      if ( objs_[j]->inside(x+dim_*i) ) {
        Matrix<T, dim_, 1> g;
        objs_[j]->Gra(x+dim_*i, g.data());
        data->save_gra(i, w_ * g);
      }
    }
  }
  return 0;
}

template<typename T, size_t dim_>
int geom_contact_energy<T, dim_>::Hes(const T *x, data_ptr<T, dim_> &data) const {

#pragma omp parallel
  for (size_t i = 0; i < dof_; ++i) {
    for (size_t j = 0; j < objs_.size(); ++j) {
      if ( !objs_[j].get() )
        continue;
      if ( objs_[j]->inside(x+dim_*i) ) {
        Matrix<T, dim_, dim_> H;
        objs_[j]->Hes(x+dim_*i, H.data());
        for (size_t p = 0; p < dim_; ++p)
          for (size_t q = 0; q < dim_; ++q)
            data->save_hes(dim_*i+p, dim_*i+q, w_*H(p, q));
      }
    }
  }
  return 0;
}

template<typename T, size_t dim_>
int geom_contact_energy<T, dim_>::update_center_position(const size_t obj_id, const T* delt_x){
  objs_[obj_id]->update_center_position(delt_x);
  return 0;
}
//===============================================================================

template class signed_dist_func<double,3>;
template class planeSDF<double,3>;
template class sphereSDF<double,3>;
template class geom_contact_energy<double, 3>;
}
