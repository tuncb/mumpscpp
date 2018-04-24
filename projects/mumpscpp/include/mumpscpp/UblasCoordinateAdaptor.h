#pragma once
#include <mumpscpp\mumpscpp.h>
#include <boost/numeric/ublas/matrix_sparse.hpp>

namespace mumpscpp {

  typedef boost::numeric::ublas::coordinate_matrix<double, boost::numeric::ublas::row_major, 1, boost::numeric::ublas::unbounded_array<int>> UblasCoordinateSparseMatrix;

  namespace traits {

    template <>
    struct LhsAdaptor < UblasCoordinateSparseMatrix >
    {
      int getOrder(UblasCoordinateSparseMatrix& mat)
      {
        return (int)mat.size1();
      }
      int getNumEntry(UblasCoordinateSparseMatrix& mat)
      {
        return mat.nnz();
      }
      int* getIndex1Data(UblasCoordinateSparseMatrix& mat)
      {
        return (int*)mat.index1_data().begin();
      }
      int* getIndex2Data(UblasCoordinateSparseMatrix& mat)
      {
        return (int*)mat.index2_data().begin();
      }
      double* getValueData(UblasCoordinateSparseMatrix& mat)
      {
        return mat.value_data().begin();
      }
    };

  }
}
