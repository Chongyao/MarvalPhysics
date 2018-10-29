#include "get_nn.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <unordered_map>
using namespace std;
using namespace Eigen;

namespace marvel{

spatial_hash::spatial_hash(const size_t &table_size_):table_size(table_size_){
  prime_num = vector<size_t> {7373856093, 19349663, 83492791};
  hash_table = make_shared<table_type>(table_size_);
}

bool spatial_hash::find_ele(const list<value_type> &bucket, const value_type &val){
  for(auto &v : bucket){
    if (v == val )
      return true;
  }
  return false;  
}

size_t spatial_hash::hash_func(const key_type &key){
  return  ( (key(0)*prime_num[0]) ^ (key(1)*prime_num[1]) ^ (key(2)*prime_num[2]) ) % table_size;
}

int spatial_hash::insert(const key_type &key, const value_type &value){
  size_t table_id = hash_func(key);
  if( !find_ele((*hash_table)[table_id], value) )
    (*hash_table)[table_id].push_front(value);
  return 0;
}


int spatial_hash::get_val(const key_type &key, std::list<value_type> &vals){
  size_t table_id = hash_func(key);
  vals = (*hash_table)[table_id];
}
//TODO: use hash method or exist library
// int calc_NNN(const MatrixXd &points, MatrixXi &NN, VectorXd &sup_radi, const size_t &nn_num){
//   //init
//   NN = MatrixXi(10, points.cols());
//   sup_radi = VectorXd(points.cols());

//   size_t cell_size = size_t(floor(pow(points.cols()/nn_num, 1/3)));
//   size_t table_size = size_t(floor(pow(points.cols(), 0.5)));

//   MatrixXi points_dis(points.rows(), points.cols()); 
//   points_dis = points.cast<int>();
  

//   //make hash table
//   spatial_hash points_hash(table_size);
//   for(size_t i = 0; i < points.cols(); ++i){
//     points_hash.insert(points_dis.col(i), i);
//   }

//   //
//   for(size_t i = 0; i < points.cols(); ++i){
//   }
// }

struct pair_dis{
  size_t m;
  size_t n;
  double dis;
};

int calc_NNN(const MatrixXd &points, MatrixXi &NN, VectorXd &sup_radi, const size_t &nn_num){
  assert(points.cols() > nn_num);
  
  NN.setZero(nn_num, points.cols());
  sup_radi.setZero(points.cols());
  
  size_t points_num = points.cols();
  vector<pair_dis> pairs(points_num*(points_num - 1) / 2);
  auto id_calc = [=](size_t i, size_t j){return ((2*points_num - i - 1)*i/2) + j - i - 1; };
#pragma omp parallel for
  for(size_t i = 0; i < points_num - 1; ++i){
    for(size_t j = i + 1; j < points_num; ++j){      
      pairs[id_calc(i ,j)] = {i, j, (points.col(i) - points.col(j)).norm()};
    }
  }

  sort(pairs.begin(), pairs.end(), [](const pair_dis &a, const pair_dis &b){return a.dis < b.dis;});

  vector<int> count(points_num);
  size_t total = 0;
  for(size_t i = 0; i < pairs.size(); ++i){
    if(total == nn_num*points_num) break;
    if(count[pairs[i].m] != -1){
      NN(count[pairs[i].m], pairs[i].m) = pairs[i].n;
      sup_radi(pairs[i].m) += pairs[i].dis;
      count[pairs[i].m]++;
      total++;
      if(count[pairs[i].m] == nn_num)
        count[pairs[i].m] = -1;
    }
    if(count[pairs[i].n] != -1){
      NN(count[pairs[i].n], pairs[i].n) = pairs[i].m;
      sup_radi(pairs[i].n) += pairs[i].dis;
      count[pairs[i].n]++;
      total++;
      if(count[pairs[i].n] == nn_num)
        count[pairs[i].n] = -1;
    }
  }

  sup_radi*= 3.0/nn_num;
  cout << "done " << endl;
  cout << NN.block(0,0,nn_num, 10) << endl;
  cout << sup_radi.head(10) << endl;


  return 0;
}


int find_NN(const Vector3i &query, const unordered_multimap<Vector3i,size_t> &points_hash, vector<pair_dis> &NN_cand, const size_t &nn_num){
  size_t cand_num = 0;
  int grid_delt = 0;
  //count for grid_delt;
  do{
    ++grid_delt;
    for(int p = -grid_delt; p < grid_delt + 1; ++p){
      for(int q = -grid_delt; q < grid_delt + 1; ++q){
        for(int r = -grid_delt; r < grid_delt + 1; ++r){
          Vector3i delt = {p, q, r};
          cand_num += points_hash.count(points_dis.col(i) + delt);
        }
      }
    }
  }while(cand_num  < nn_num + 2);

  // get NN_cand
  for(int p = -grid_delt; p < grid_delt + 1; ++p){
    for(int q = -grid_delt; q < grid_delt + 1; ++q){
      for(int r = -grid_delt; r < grid_delt + 1; ++r){
        Vector3i delt = {p, q, r};
        auto range = points_hash.equal_range(points_dis.col(i) + delt);
        if(range.first - range.second != 0){
          for_each(range.first, range.second, [=](Vector3i &one_point){
              NN_cand.push_back({i, x.second, (points.col(i) - points.col(x.second)).norm()});
            });
        }
        
      }
    }
  }
}


int hash_NNN(const Eigen::MatrixXd &points, Eigen::MatrixXi &NN, Eigen::VectorXd &sup_radi, const size_t &nn_num){
  assert(points.cols() > nn_num);
  //init data
  const size_t points_num = points.cols();
  NN.setZero(nn_num, points_num);
  sup_radi.setZero(points_num);

  //set hash parameter
  double cell_size = pow(points.cols()/nn_num, 1/3);
  size_t table_size = size_t(floor(pow(points.cols(), 0.5)));
  
  //generate discretized 3D position
  MatrixXi points_dis = floor(points.array()/cell_size).cast<int>();
  Vector3i max_id = {points_dis.row(0).maxCoeff(), points_dis.row(1).maxCoeff(), points_dis.row(1).maxCoeff()}
  

  //build hash_map
  unordered_multimap<Vector3i, size_t> points_hash(table_size);

  //insert elements
  for(size_t i = 0; i < points_num; ++i){
    points_hash.insert({points_dis.col(i), i});
  }

  //calc NN
  for(size_t i = 0; i < points_num; ++i){
    vector<pair_dis> NN_cand;
    find_NN(points_dis.col(i), points_hash, NN_cand, nn_num);
    sort(NN_cand.begin(), NN_cand.end(), [](const pair_dis &a, const pair_dis &b){return a.dis < b.dis;});    
    for(size_t j = 0; j < nn_num; ++j){
      NN(j, i) = NN_cand[j + 1].n;
      sup_radi[i] += NN_cand[j + 1].dis;
    }
  }

  sup_radi*= 3.0/nn_num;
  cout << "done " << endl;
  cout << NN.block(0,0,nn_num, 10) << endl;
  cout << sup_radi.head(10) << endl;

}
}//namespace:marvel
