#pragma once

#include <mumpscpp\mumpscpp.h>
#include <boost/numeric/ublas/vector.hpp>

namespace mumpscpp {

  typedef boost::numeric::ublas::vector<double> UblasVector;
  namespace traits {

    template <> struct RhsAdaptor < UblasVector >
    {
      typedef boost::numeric::ublas::vector<double> RhsType;

      double* getValueData(RhsType& vec) { return const_cast<double*>(vec.data().begin()); }
      int getNumRhs(RhsType& vec) { return 1; }
      int getLeadingDim(RhsType& vec) { return (int)vec.size(); }
    };

  }
}