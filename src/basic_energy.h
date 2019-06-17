#ifndef BASIC_ENERGY_H
#define BASIC_ENERGY_H
#include "def.h"
#include "data_str_core.h"
#include <vector>
#include <memory>

namespace marvel{

template<typename T, size_t dim_>
using data_ptr = std::shared_ptr<dat_str_core<T, dim_>>;

template<typename T, size_t dim_>
class position_constraint : public Functional<T, dim_>{
 public:
  //used for displace based
  position_constraint(const size_t dof, const T &w, const std::vector<size_t> &cons);

  //used for position based
  position_constraint(const T *rest, const size_t dof, const T &w, const std::vector<size_t> &cons);
  int Val(const T *x, data_ptr<T, dim_> &data) const;  
  int Gra(const T *x, data_ptr<T, dim_> &data) const ;
  int Hes(const T *x, data_ptr<T, dim_> &data) const ;
  size_t Nx() const;
 private:
  Eigen::Matrix<T, -1, -1> rest_;
  const size_t dof_;
  const T w_;
  const std::vector<size_t> cons_;
};

template<typename T, size_t dim_>
class gravity_energy : public Functional<T, dim_>{
 public:
  gravity_energy(const size_t dof, const T &w_g, const T &gravity, const Eigen::Matrix<T, -1, 1> &mass, const char &axis);
  int Val(const T *disp, data_ptr<T, dim_> &data) const ;
  int Gra(const T *disp, data_ptr<T, dim_> &data) const;
  int Hes(const T *disp, data_ptr<T, dim_> &data) const;
  size_t Nx() const;
 private:
  const char axis_;
  const size_t dof_;
  const T w_g_;
  const T gravity_;
  const Eigen::Matrix<T, -1, 1> mass_;
};

//simple collision with ground
template<typename T, size_t dim_>
class collision : public Functional<T, dim_>{
 public:
  collision(const size_t dim, const T &w_coll, const char &ground_axis, const T &ground_pos, const size_t &num_surf_point , const std::shared_ptr<Eigen::Matrix<T, -1, -1>>& init_points_ptr);
  int Val(const T *disp, data_ptr<T, dim_> &data) const;
  int Gra(const T *disp, data_ptr<T, dim_> &data) const;
  int Hes(const T *disp, data_ptr<T, dim_> &data) const;
  size_t Nx() const;
 private:
  const char ground_axis_;
  const T w_coll_;
  const T ground_pos_;
  const size_t num_surf_point_;
  const size_t dof_;
  const std::shared_ptr<Eigen::Matrix<T, -1, -1>> init_points_ptr_;

};

template<typename T, size_t dim_>
class momentum : public  Functional<T, dim_>{
 public:
  //used for displacement based 
  momentum(const size_t dof,const Eigen::Matrix<T, -1, 1>& mass_vec, const T& dt);

  //used for position based
  momentum(const T* x, const size_t dof,const Eigen::Matrix<T, -1, 1>& mass_vec, const T& dt);
  int Val(const T *disp, data_ptr<T, dim_> &data) const ;
  int Gra(const T *disp, data_ptr<T, dim_> &data) const ;
  int Hes(const T *disp, data_ptr<T, dim_> &data) const ;
  int update_location_and_velocity(const T *new_dispk_ptr);
  int set_initial_velocity(const Matrix<T, dim_, 1>& velo);
  size_t Nx() const;

  Eigen::Matrix<T, -1, 1> vk_, dispk_;
 private:
  const size_t dof_;
  Eigen::Matrix<T, -1, 1> mass_vec_;
  const T dt_;
  const T d1dt_;
  const T d1dtdt_;

};

// template class position_constraint<3>;
// template class momentum<3>;
// template class gravity_energy<3>;
// template class collision<3>;

}//namespace marvel

#include "basic_energy.imp"

#endif
