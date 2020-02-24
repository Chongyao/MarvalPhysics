#ifndef FEM_BASIS
#define FEM_BASIS
#include <Eigen/Dense>
#include <unsupported/Eigen/KroneckerProduct>
#include <iostream>
namespace marvel{
using namespace Eigen;
using namespace std;

template<typename T, size_t dim_, size_t order_, size_t num_per_cell_>
struct impl_Dphi_Dxi{
  static void calc_Dhpi_Dxi(const Eigen::Matrix<T, dim_, 1>& PNT, const T* X,  Eigen::Matrix<T, num_per_cell_, dim_>& Dphi_Dxi){
    cout << "unsupported type of basis func.\n";
    assert(0);
    return;
  }
};

template<typename T>
struct impl_Dphi_Dxi<T, 3, 1, 4>{
  static void calc_Dphi_Dxi(const Eigen::Matrix<T, 3, 1>& PNT, const T* X,  Eigen::Matrix<T, 4, 3>& Dphi_Dxi){
    Dphi_Dxi.setZero();
    Dphi_Dxi.template topRows<3>().setIdentity();
    Dphi_Dxi.row(3) = Eigen::Matrix<T, 1, 3>::Ones() * (-1);
    return;
  }
};

template<typename T>
struct impl_Dphi_Dxi<T, 3, 1, 8>{
  static void calc_Dphi_Dxi(const Eigen::Matrix<T, 3, 1>& PNT, const T* X,  Eigen::Matrix<T, 8, 3>& Dphi_Dxi){
    Dphi_Dxi.setZero();
    const T xi0 = PNT(0), xi1 = PNT(1), xi2 = PNT(2);

    vector<double> l(3, 0);
    vector<double> sign(3, 0);
    for(size_t z = 0; z < 2; ++z){
      sign[2] = z == 0 ? -1 : 1;
      l[2] = 1 + sign[2] * xi2;
      for(size_t y = 0; y < 2; ++y){
        sign[1] = y == 0 ? -1 : 1;
        l[1] = 1 + sign[1] * xi1;
        for(size_t x = 0; x < 2; ++x){
          sign[0] = x == 0 ? - 1 : 1;
          l[0] = 1 + sign[0] * xi0;
          const size_t p_id = z * 4 + y * 2 + (y == 0 ? x : 1 - x);
          for(size_t d = 0; d < 3; ++d)
            Dphi_Dxi(p_id, d) = sign[d] * l[(d + 1) % 3] * l[(d + 2) % 3];
        }
      }
    }
    Dphi_Dxi /= 8.0;

    
    return;

  }
};

template<typename T, size_t dim_, size_t order_, size_t num_per_cell_>
class basis_func{
 public:
  static void calc_Dphi_Dxi(const Eigen::Matrix<T, dim_, 1>& PNT, const T* X,  Eigen::Matrix<T, num_per_cell_, dim_>& Dphi_Dxi){
    return impl_Dphi_Dxi<T, dim_, order_, num_per_cell_>::calc_Dphi_Dxi(PNT, X, Dphi_Dxi);
  }
 
  static void calc_InvDm_Det(const Eigen::Matrix<T, num_per_cell_, dim_>& Dphi_Dxi, const T* X, T& Jac_det, Matrix<T, dim_, dim_>& Dm_inv){
    Dm_inv.setZero();
    const Eigen::Map<const Matrix<T, dim_, num_per_cell_>> rest(X);
    Matrix<T, dim_, dim_> Dm = rest * Dphi_Dxi;
    Dm_inv = Dm.inverse();
    Jac_det = fabs(Dm.determinant());
    return;
  }
  static void get_def_gra(const Eigen::Matrix<T, num_per_cell_, dim_>& Dphi_Dxi, const T* const x, const Matrix<T, dim_, dim_>& Dm_inv,  Eigen::Matrix<T, dim_, dim_> & def_gra) {
    const Eigen::Map<const Matrix<T, dim_, num_per_cell_>> deformed(x);
    def_gra = deformed * Dphi_Dxi * Dm_inv;
    return;
  }
  
  static void get_Ddef_Dx(const Eigen::Matrix<T, num_per_cell_, dim_>& Dphi_Dxi, const Eigen::Matrix<T, dim_, dim_>& Dm_inv, Eigen::Matrix<T, dim_ * dim_, dim_ * num_per_cell_>& Ddef_Dx){
    Ddef_Dx.setZero();
    const Eigen::Matrix<T, num_per_cell_, dim_> Ddef_Dx_compressed = Dphi_Dxi * Dm_inv;
    #pragma omp parallel for
    for(size_t i = 0; i < num_per_cell_; ++i)
      for(size_t j = 0; j < dim_; ++j)
        Ddef_Dx.block(j * dim_, i * dim_, dim_, dim_) = Eigen::Matrix<T, dim_, dim_>::Identity() * Ddef_Dx_compressed(i, j);
    return;
  }
};


}
#endif
