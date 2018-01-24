# pyWannier90: A python interface of Wannier90 for PySCF 


<img src="https://github.com/hungpham2017/pyWannier90/blob/master/doc/Polyyens.png" width="500" align="middle">


### How to compile pyWannier90
 - Required: gxx, cmake, pybind11, gfortran
 - First compile the wannier90 (http://www.wannier.org/): note that the flag "-fPIC" is needed in make.inc
 - Modify the Wannier90 directory in the install.sh
 - Compile the libwannier90 library: source install.sh
 - Modify the libwannier90 directory in the pywannier90.py
 - In general, the libwannier90 library can be incorporated in any python-based electronic structure codes. However, it is highly recommended to use it with PySCF via pywannier90.py module