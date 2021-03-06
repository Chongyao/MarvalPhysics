#ifndef COLL_WRAPPER_H
#define COLL_WRAPPER_H
#include "Point_Sys/src/data_stream.h"
#include "coll_response.h"
#include <Collision/CollisionDetect-rigid/src/Collision_eigen.h>
#include <memory>
#include <Eigen/Core>
namespace marvel{

class coll_info{
 public:
  coll_info(const size_t& point_id, const size_t& mesh_id, const size_t& face_id, const double& time):mesh_id_(mesh_id),face_id_(face_id),time_(time),point_id_(point_id){}

  const size_t point_id_;
  const size_t mesh_id_;
  const size_t face_id_;
  const double time_;
};



class coll_wrapper{

 public:
  coll_wrapper(const std::vector<std::shared_ptr<Eigen::MatrixXi>>& obta_surfs,
               const std::vector<std::shared_ptr<Eigen::MatrixXd>>& obta_nods,
               const std::shared_ptr<Eigen::MatrixXi>& core_surf_ptr,
               const Eigen::MatrixXd& core_nods);
  coll_wrapper(const std::vector<std::shared_ptr<Eigen::MatrixXi>>&obta_surfs,
               const std::vector<std::shared_ptr<Eigen::MatrixXd>>& obta_nods,
               const std::shared_ptr<Eigen::MatrixXi>& core_surf_ptr,
               const double* core_nods, const size_t& num_nods);


  int Collide(const std::vector<std::shared_ptr<Eigen::MatrixXi>>& obta_surfs,
              const std::vector<std::shared_ptr<Eigen::MatrixXd>>& obta_nods,
               Eigen::MatrixXd& new_core_velo,  Eigen::MatrixXd& new_core_nods);

  // int Collide(const std::vector<std::shared_ptr<Eigen::MatrixXi>>& obta_surfs,
  //             const std::vector<std::shared_ptr<Eigen::MatrixXd>>& obta_nods,
  //              double* new_core_velo,  double* new_core_nods);


  // int Collide_imp(const std::vector<std::shared_ptr<Eigen::MatrixXi>>& obta_surfs,
  //                 const std::vector<std::shared_ptr<Eigen::MatrixXd>>& obta_nods,
  //                 Eigen::MatrixXd& new_core_nods, energy_dat& dat_str,
  //                 const Eigen::VectorXd& mass);

  int Collide_imp(const std::vector<std::shared_ptr<Eigen::MatrixXi>>& obta_surfs,
                  const std::vector<std::shared_ptr<Eigen::MatrixXd>>& obta_nods,
                  Eigen::MatrixXd& new_core_nods, Eigen::MatrixXd& new_core_velo,
                  energy_dat& dat_str,
                  const VectorXd& mass);
  
  
 private:
  Collision_zcy* const COLL_ptr;
  Eigen::MatrixXd pre_core_nods;
  Eigen::MatrixXd pre_core_velo;
  const std::shared_ptr<Eigen::MatrixXi> core_tris_ptr;
  const size_t core_num_nods;
  const size_t core_num_tris;
};

}//namespace marvel
#endif
