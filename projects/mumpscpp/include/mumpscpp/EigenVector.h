#pragma once
#include <mumpscpp\mumpscpp.h>
#include <Eigen\core>

namespace mumpscpp {

  typedef Eigen::VectorXd EigenVector;
  namespace traits {

    template <>
    struct RhsAdaptor < Eigen::VectorXd >
    {
      double* getValueData(Eigen::VectorXd& vec) { return const_cast<double*>(vec.data()); }
      int getNumRhs(Eigen::VectorXd& vec) { return 1; }
      int getLeadingDim(Eigen::VectorXd& vec) { return vec.rows(); }
    };
  }
}