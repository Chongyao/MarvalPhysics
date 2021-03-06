#ifndef INIT_H
#define INIT_H
#include <iostream>

#include <Eigen/Core>


namespace marvel{
int get_inner_points(Eigen::MatrixXd &points, const Eigen::MatrixXi &surf, const Eigen::MatrixXd &nods, const bool save_surf_points);
int gen_points(const Eigen::MatrixXd &nods, const Eigen::MatrixXi &surf, const size_t &num_in_axis, Eigen::MatrixXd &points, const bool save_surf_points = false);
  int gen_points(const Eigen::MatrixXd &nods, const Eigen::MatrixXi &surf, const double interval, Eigen::MatrixXd &points, const bool save_surf_points);
}
#endif

