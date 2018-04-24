from conans import ConanFile

class MumpsCppConan(ConanFile):
    name = "mumpscpp"
    version = "1.0.0"
    license = "https://www.apache.org/licenses/LICENSE-2.0"
    url = "https://github.com/tuncb/mumpscpp"
    description = ("A c++ wrapper around mumps")
    requires = "msmpi/2008.2@tuncb/pangea","intel-mkl/2018@tuncb/pangea"
    default_options = "intel-mkl:threading=openmp","intel-mkl:fortran=True","intel-mkl:blacs=True","intel-mkl:scalapack=True","msmpi:fortran=True"

    def package(self):
        self.copy("*", dst="mumpscpp", src="../projects/mumpscpp/include/mumpscpp")
        self.copy("../projects/external/lib/debug/*.lib", dst="lib/debug", keep_path=False)
        self.copy("../projects/external/lib/release/*.lib", dst="lib/release", keep_path=False)

    def package_info(self):
        self.cpp_info.includedirs = ['.']
        self.cpp_info.libs = ["dmumps_c", "dmumps_fortran", "mumps_common_c", "pord_c"]
        self.cpp_info.debug.libdirs = ["lib/debug"]
        self.cpp_info.release.libdirs = ["lib/release"]