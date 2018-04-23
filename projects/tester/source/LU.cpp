#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <mumpscpp/dmumps_c.h>
#include <mumpscpp/mumpscpp.h>

BOOST_AUTO_TEST_SUITE(LU)

BOOST_AUTO_TEST_CASE(SimpleDouble)
{
  using namespace boost::numeric::ublas;
  coordinate_matrix<double> m(3, 3, 3 * 3);
  for (unsigned i = 0; i < m.size1(); ++i)
    for (unsigned j = 0; j < m.size2(); ++j)
      m(i, j) = 3 * i + j;
  std::cout << m << std::endl;

  mumpscpp::Mumps<double> mumps;



}

BOOST_AUTO_TEST_SUITE_END()