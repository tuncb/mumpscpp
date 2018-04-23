#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#define MUMPSCPP_LIB_MSMPI
#define MUMPSCPP_LIB_MKL_THREAD


#include "catch.h"
#include <mumpscpp/mumpscpp.h>
#include <mumpscpp/mumpsLibraries.h>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <Eigen\core>
#include <shiva\Environment.h>
#include <shiva\Communicator.h>
#include <boost/numeric/ublas/io.hpp>
#include <mumpscpp/UblasCoordinateAdaptor.h>
#include <mumpscpp/UblasVectorAdaptor.h>
#include <mumpscpp/EigenVector.h>

shiva::environment mpi_env;

void assemble_unsymmetric(mumpscpp::UblasCoordinateSparseMatrix& k)
{
  const size_t matrix_size = 5;
  k.resize(matrix_size, matrix_size, false);
  k.clear();
  k.append_element(0, 0, 1.0);
  k.append_element(0, 1, 2.0);
  k.append_element(0, 2, 3.0);
  k.append_element(0, 3, -4.0);
  k.append_element(0, 4, -5.0);
  k.append_element(1, 0, 2.0);
  k.append_element(1, 1, 1.0);
  k.append_element(1, 2, 4.0);
  k.append_element(1, 3, 3.0);
  k.append_element(1, 4, -2.0);
  k.append_element(2, 0, 3.0);
  k.append_element(2, 1, 4.0);
  k.append_element(2, 2, 1.0);
  k.append_element(2, 3, 2.0);
  k.append_element(2, 4, 3.0);
  k.append_element(3, 0, 4.0);
  k.append_element(3, 1, -2.0);
  k.append_element(3, 2, 4.0);
  k.append_element(3, 3, 1.0);
  k.append_element(3, 4, 3.0);
  k.append_element(4, 0, 5.0);
  k.append_element(4, 1, 4.0);
  k.append_element(4, 2, -2.0);
  k.append_element(4, 3, 4.0);
  k.append_element(4, 4, 1.0);
  k.sort();
}

void assemble_symmetric(mumpscpp::UblasCoordinateSparseMatrix& k)
{
  // lower triangular part
  const size_t matrix_size = 3;
  k.resize(matrix_size, matrix_size, false);
  k.clear();
  k.append_element(0, 0, 2.0);
  k.append_element(1, 0, -1.0);
  k.append_element(1, 1, 2.0);
  k.append_element(2, 0, 0.0);
  k.append_element(2, 1, -1.0);
  k.append_element(2, 2, 2.0);

  k.sort();
}


bool is_close(double d1, double d2, double eps)
{
  return abs(d1 - d2) < eps;
};

TEST_CASE("Mumps unsymmetric", "[mumps]") {
  shiva::communicator world;
  const size_t matrix_size = 5;
  
  mumpscpp::UblasCoordinateSparseMatrix k;
  assemble_unsymmetric(k);
  mumpscpp::EigenVector f = Eigen::VectorXd::Ones(matrix_size);

  mumpscpp::Mumps<double> mumps(mumpscpp::MatrixType::unsymmetric, mumpscpp::HostParallelism::involved, world.fortran_mpi_communicator());
  mumps.set_output_level(mumpscpp::OutputLevel::error);
  mumps.setDistributedInput(k);



  mumps.analyzeFactorize();
  mumps.solve(f);
  
  REQUIRE(is_close(f[0], 0.1933, 0.0001));
  REQUIRE(is_close(f[1], 0.1015, 0.0001));
  REQUIRE(is_close(f[2], 0.1314, 0.0001));
  REQUIRE(is_close(f[3], -0.0212, 0.0001));
  REQUIRE(is_close(f[4], -0.0249, 0.0001));

  mumps.destroy();
}

TEST_CASE("Mumps unsymmetric schur", "[mumps]") {
  shiva::communicator world;
  const size_t matrix_size = 5;

  mumpscpp::UblasCoordinateSparseMatrix k;
  assemble_unsymmetric(k);
  mumpscpp::EigenVector f = Eigen::VectorXd::Ones(matrix_size);

  mumpscpp::Mumps<double> mumps(mumpscpp::MatrixType::unsymmetric, mumpscpp::HostParallelism::involved, world.fortran_mpi_communicator());
  mumps.set_output_level(mumpscpp::OutputLevel::none);
  mumps.setDistributedInput(k);

  auto get_schur = [&](std::vector<int>& dofs)
  {
    if (world.rank() == 0) {
      mumps.set_host_shur_complement(dofs, mumpscpp::SchurReturnType::host);
    }

    mumps.analyzeFactorize();
    auto&& schur = mumps.get_host_schur();
    Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> mf(schur.data(), 2, 2);
    return mf;

  };
  
  std::vector<int> dofs = { 1, 2 };
  auto mf1 = get_schur(dofs);
  REQUIRE(is_close(mf1(0,0), 2.7826, 0.0001));
  REQUIRE(is_close(mf1(0,1), 18.6957, 0.0001));
  REQUIRE(is_close(mf1(1,0), -8.5652, 0.0001));
  REQUIRE(is_close(mf1(1,1), 17.6087, 0.0001));

  mumps.destroy();

}

TEST_CASE("Mumps symmetric", "[mumps]") {
  shiva::communicator world;
  const size_t matrix_size = 3;

  mumpscpp::UblasCoordinateSparseMatrix k;
  assemble_symmetric(k);
  mumpscpp::EigenVector f = Eigen::VectorXd::Zero(matrix_size);
  f[0] = 1.25;
  f[1] = -2.0;
  f[2] = 1.75;

  mumpscpp::Mumps<double> mumps(mumpscpp::MatrixType::symmetric, mumpscpp::HostParallelism::involved, world.fortran_mpi_communicator());
  mumps.set_output_level(mumpscpp::OutputLevel::error);
  mumps.setDistributedInput(k);

  mumps.analyzeFactorize();
  mumps.solve(f);
  mumps.write_problem("d:\\test.txt");

  //std::cout << f << std::endl;

  REQUIRE(is_close(f[0], 0.375, 0.0001));
  REQUIRE(is_close(f[1], -0.5, 0.0001));
  REQUIRE(is_close(f[2], 0.625, 0.0001));

  mumps.destroy();
}

TEST_CASE("Mumps symmetric schur", "[mumps]") {
  shiva::communicator world;
  const size_t matrix_size = 3;

  mumpscpp::UblasCoordinateSparseMatrix k;
  assemble_symmetric(k);
  mumpscpp::EigenVector f = Eigen::VectorXd::Ones(matrix_size);

  mumpscpp::Mumps<double> mumps(mumpscpp::MatrixType::symmetric, mumpscpp::HostParallelism::involved, world.fortran_mpi_communicator());
  mumps.set_output_level(mumpscpp::OutputLevel::none);
  mumps.setDistributedInput(k);

  auto get_schur = [&](std::vector<int>& dofs)
  {
    mumps.set_host_shur_complement(dofs, mumpscpp::SchurReturnType::slaves_lower);

    mumps.analyzeFactorize();
    auto&& schur = mumps.get_host_schur();
    return schur;

  };

  std::vector<int> dofs = { 1, 2 };
  auto mf1 = get_schur(dofs);

  //for (auto d : mf1) { std::cout << d << std::endl; }

  // only the lower triangular will be sent by mumps
  REQUIRE(is_close(mf1[0], 2.0, 0.0001));
  REQUIRE(is_close(mf1[1], -1, 0.0001));
  REQUIRE(is_close(mf1[2], 0.0, 0.0001));
  REQUIRE(is_close(mf1[3], 1.5, 0.0001));

  mumps.destroy();

}


