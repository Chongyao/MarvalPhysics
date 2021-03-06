#include "config.h"
#include "multigrid.h"
#include "gauss_seidel.h"
#include "inverse_isoparametric_map.h"
#include "eigen_ext.h"
#include <unsupported/Eigen/KroneckerProduct>
#include <Eigen/Eigenvalues>
#include <fstream>
#include <map>
namespace marvel{
using namespace std;
using namespace Eigen;
//============================transfer===========================//
transfer::transfer(const SPM& I, const SPM& R): I_(I), R_(R){}


transfer get_transfer(const MatrixXd& nods_H, const MatrixXi& cells_H, const MatrixXd& nods_h, const MatrixXi& cells_h){
  const size_t num_per_cell_ = 8;
  assert(cells_h.cols() % cells_H.cols() == 0);
  assert(cells_H.rows() == cells_h.rows() && cells_H.rows() == num_per_cell_);
  
  const size_t one_to_many = cells_h.cols() / cells_H.cols();
  
  const VectorXi all_rows = Matrix<int, 3, 1>::LinSpaced(3, 0, 2);
  
  vector<Triplet<double>> trips_I; //n_h X n_H
  
  vector<bool> if_calc (nods_h.cols(), false);
  for(size_t i = 0; i < cells_H.cols(); ++i){
    const Matrix<double, 3, num_per_cell_> x_H_cell = indexing(nods_H, all_rows, cells_H.col(i));
    vector<Triplet<double>> trips_patch;
    // #pragma omp parallel for
    for(size_t j = 0; j < one_to_many; ++j){
      for(size_t k = 0; k < num_per_cell_; ++k){
        const size_t nod_h_id = cells_h(k, i * one_to_many + j);
        if(if_calc[nod_h_id])
          continue;
        else{
          if_calc[nod_h_id] = true;
          Matrix<double, num_per_cell_, 1> w;
          VectorXd xi = inverse_isoparametric_hex(nods_h.col(nod_h_id), x_H_cell.data(), w.data());
          for(size_t m = 0; m < num_per_cell_; ++m){
            trips_patch.push_back(Triplet<double>(nod_h_id, cells_H(m, i), w(m)));
          }
        }
      }
    }
    #pragma omp critical
    {
    trips_I.insert(trips_I.end(), trips_patch.begin(), trips_patch.end());      
    }
  }

  SparseMatrix<double> I(nods_h.cols(), nods_H.cols());{
    I.reserve(trips_I.size());
    I.setFromTriplets(trips_I.begin(), trips_I.end());
    Matrix3d Iden = Matrix3d::Identity();
    I = kroneckerProduct(I, Iden).eval();
  }

  return transfer(I, I.transpose());
}
//============================transfer===========================//
//============================layer===========================//
layer::layer(const SPM& A, const solver_type& type, const size_t itrs):A_(A), u_(VectorXd::Zero(A.rows())), type_(type), itrs_(itrs){
  if(type == solver_type::PCG)
    GS_solver_ = nullptr;
  else if(type == solver_type::GS)
    GS_solver_ = make_shared<Gauss_seidel>(A, itrs);
  else if(type_ == solver_type::WJ)
    GS_solver_ = make_shared<Weighted_Jacobi>(A, itrs, 1e-10);
}

int layer::solve(){
  if(type_ == solver_type::PCG){
    ConjugateGradient<SPM, Lower|Upper> cg;
    cg.compute(A_);
    u_ = cg.solve(rhs_);
  }else{
    // Gauss_seidel GS(A_, rhs_.data(), itrs_, 1e-4, 1.0);
    // VectorXd solution(rhs_.size());
    // GS.solve(u_.data(), solution.data());
    // gauss_seidel_solver(A_, rhs_, u_, itrs_);
    GS_solver_->solve(rhs_, u_);
  }
  return 0;
}

VectorXd layer::get_residual()const{
  return rhs_ - A_ * u_;
}

//============================layer===========================//

//============================multigrid process===========================//

multigrid_process::multigrid_process(const vector<int>& process, VS<layer>& layers, const VS<transfer>& transfers):layers_(layers), transfers_(transfers), process_(process), dof_(layers[0]->A_.rows()){
  assert(layers_.size() == transfers_.size() + 1);
  int sum = 0;
  for(const auto& op : process){
    sum += op;
  }
  assert(sum == 0);
}

int multigrid_process::execute(double* solution){
  Map<VectorXd> u(solution, layers_[0]->u_.size());
  layers_[0]->u_ = u;
  
  const size_t num_layers = layers_.size();
  size_t layer_id = 0;

  double time_rel = 0, time_res = 0, time_corr = 0;
  for(const auto& op : process_){
    layer_id += op;
    if(op == 1){
      __TIME_BEGIN__;
      relax(layer_id - 1);
      time_rel += __TIME_END__("relax", false);
      __TIME_BEGIN__;
      restrict(layer_id);
      layers_[layer_id]->u_.setZero();
      time_res += __TIME_END__("res", false);
    }else{
      __TIME_BEGIN__; 
      correct(layer_id);
      time_corr += __TIME_END__("corr", false);
      __TIME_BEGIN__;
      relax(layer_id);
      time_rel += __TIME_END__("relax",false);
    }
    if(layer_id == num_layers - 1){
      __TIME_BEGIN__;
      relax(layer_id);
      time_rel += __TIME_END__("relax", false);
    }

  }
  u = layers_[0]->u_;
  // cout << "time for relax is " << time_rel << " seconds.\n"
  //      << "time for restrict is " << time_res << " seconds.\n"
  //      << "time for correction is "<< time_corr << " seconds.\n";
  return 0;
}

int multigrid_process::relax(const size_t layer_id){
  // cout << "relax layer " << layer_id << endl;
  // cout << "relative error  befor GS " << layers_[layer_id]->get_residual().norm() / layers_[layer_id]->rhs_.norm() << endl;
    layers_[layer_id]->solve();
    // cout << "relative error after GS" << layers_[layer_id]->get_residual().norm() / layers_[layer_id]->rhs_.norm()<< endl;
  return 0;
}

int multigrid_process::restrict(const size_t layer_id){
  assert(layer_id< layers_.size());
  assert(transfers_[layer_id - 1]->R_.cols() == layers_[layer_id - 1]->get_residual().rows());
  layers_[layer_id]->rhs_
      = transfers_[layer_id - 1]->R_ * layers_[layer_id - 1]->get_residual();
  return 0;
}

int multigrid_process::correct(const size_t layer_id){
  assert(layer_id - 1 >= 0);
  assert(transfers_[layer_id]->I_.cols() == layers_[layer_id + 1]->u_.rows());
  layers_[layer_id]->u_
      += transfers_[layer_id]->I_ * layers_[layer_id + 1]->u_;
  return 0;
}


}
