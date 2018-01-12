/*
pyWannier90, a python/C++ wrapper for Wannier90
Hung Q. Pham
email: pqh3.14@gmail.com
*/

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/complex.h>
#include <iostream>
#include <string>
#include <complex>
#include <vector>

extern "C" {
	
	void __w90_MOD_wannier_setup(char* seed__name, int* mp_grid_loc, int* num_kpts_loc,						//input
     double* real_lattice_loc, double* recip_lattice_loc, double* kpt_latt_loc,								//input
	 int* num_bands_tot, int* num_atoms_loc, int* atom_atomic_loc, double* atoms_cart_loc, 					//input
	 int* gamma_only_boolean, int* spinors_boolean, 														//input
	 int * nntot_loc, int* nnlist_loc, int* nncell_loc, int * num_bands_loc, int * num_wann_loc, 			//output
	 double* proj_site_loc, int* proj_l_loc, int* proj_m_loc, int* proj_radial_loc, double* proj_z_loc, 	//output
	 double* proj_x_loc, double* proj_zona_loc, int* exclude_bands_loc, int* proj_s_loc, 					//output
	 double* proj_s_qaxis_loc);

	void __w90_MOD_wannier_run(char* seed__name, int* mp_grid_loc, int* num_kpts_loc, double* real_lattice_loc, 					//input
	double* recip_lattice_loc, double* kpt_latt_loc, int* num_bands_tot, int* num_bands_loc, int* num_wann_loc, int* nntot_loc, 	//input
	int* num_atoms_loc, int* atom_atomic_loc, double* atoms_cart_loc, int* gamma_only_boolean, 										//input
	std::complex<double>* M_matrix_loc, std::complex<double>* A_matrix_loc, double* eigenvalues_loc, 								//input
	std::complex<double>* U_matrix_loc, std::complex<double>* U_matrix_opt_loc, int* lwindow_loc, 									//output
	 double* wann_centres_loc, double* wann_spreads_loc, double* spread_loc);														//output
	 
	 //gamma_only_loc, lwindow_loc are supposed to be a fortran logical. Here, it is passed as 1 or 0 and converted to 
	 // fortran logical type in wannier_lib.F90
}


namespace py = pybind11;

std::vector<py::array_t<double>> setup(char* seed__name, py::array_t<int> mp_grid_loc, int* num_kpts_loc, py::array_t<double> real_lattice_loc,
				py::array_t<double>  recip_lattice_loc, py::array_t<double>  kpt_latt_loc, int* num_bands_tot, int* num_atoms_loc,
			py::array_t<int> atom_atomic_loc, py::array_t<double>  atoms_cart_loc, int* gamma_only_boolean, int* spinors_boolean){
	
	std::cout << "You are running Wannier90_setup via pyWannier90" << "\n";
	
	// Convert python arrays of inputs to pointers
	py::buffer_info mp_grid_loc_info = mp_grid_loc.request(); 
	int* mp_grid = static_cast<int*>(mp_grid_loc_info.ptr);	
	
	py::buffer_info real_lattice_loc_info = real_lattice_loc.request(); 
	double* real_lattice = static_cast<double*>(real_lattice_loc_info.ptr);		

	py::buffer_info recip_lattice_loc_info = recip_lattice_loc.request(); 
	double* recip_lattice = static_cast<double*>(recip_lattice_loc_info.ptr);

	py::buffer_info kpt_latt_loc_info = kpt_latt_loc.request(); 
	double* kpt_latt = static_cast<double*>(kpt_latt_loc_info.ptr);
	
	py::buffer_info atom_atomic_loc_info = atom_atomic_loc.request(); 
	int* atom_atomic = static_cast<int*>(atom_atomic_loc_info.ptr);	

	py::buffer_info atoms_cart_loc_info = atoms_cart_loc.request(); 
	double* atoms_cart = static_cast<double*>(atoms_cart_loc_info.ptr);
	
	int size; 
	int nntot_loc[1];
	int num_kpts = *num_kpts_loc;	
	int num_nnmax = 12;
	size = num_kpts*num_nnmax*100;
	int nnlist_loc[size];
	int nncell_loc[3*size];
	int num_bands_loc[1];
	int num_wann_loc[1];		

	int num_bands = *num_bands_tot;
	double proj_site_loc[3*num_bands];
	int proj_l_loc[num_bands]; 
	int proj_m_loc[num_bands]; 
	int proj_radial_loc[num_bands]; 
	double proj_z_loc[3*num_bands]; 	
	double proj_x_loc[3*num_bands]; 
	double proj_zona_loc[num_bands]; 
	int exclude_bands_loc[num_bands]; 
	int proj_s_loc[num_bands];					
	double proj_s_qaxis_loc[3*num_bands];
	
	////////////////////////	
	// Call wannier_setup//
	///////////////////////	
	
	__w90_MOD_wannier_setup(seed__name, mp_grid, num_kpts_loc, real_lattice, recip_lattice, kpt_latt, 			//input						
	 num_bands_tot, num_atoms_loc, atom_atomic, atoms_cart, gamma_only_boolean, spinors_boolean, 	 			//input															
	 nntot_loc, nnlist_loc, nncell_loc, num_bands_loc, num_wann_loc, 				 							//output
	 proj_site_loc, proj_l_loc, proj_m_loc, proj_radial_loc, proj_z_loc, 										//output
	 proj_x_loc, proj_zona_loc, exclude_bands_loc, proj_s_loc, proj_s_qaxis_loc);								//output

	// Store nnlist_loc, nncell_loc in the nn_ array //

	int nntot = nntot_loc[0];	
	int nn_[nntot*num_kpts*4];
	
	for (int inn = 0; inn < nntot; inn++){
		for (int nkp = 0; nkp < num_kpts; nkp++){
			nn_[inn*num_kpts*4 + nkp*4 + 0] = nnlist_loc[inn*num_kpts + nkp];
			nn_[inn*num_kpts*4 + nkp*4 + 1] = nncell_loc[inn*num_kpts*3 + nkp*3 + 0];	
			nn_[inn*num_kpts*4 + nkp*4 + 2] = nncell_loc[inn*num_kpts*3 + nkp*3 + 1];	
			nn_[inn*num_kpts*4 + nkp*4 + 3] = nncell_loc[inn*num_kpts*3 + nkp*3 + 2];				
		}
	}
	
	//////////////////////////
	// Pass output to python//
	//////////////////////////
	
	int * bands_wann_nntot_ = new int[3] {num_bands_loc[0], num_wann_loc[0], nntot_loc[0]};
	py::buffer_info bands_wann_nntot_buf =
		{
			bands_wann_nntot_,
			sizeof(int),
			py::format_descriptor<int>::format(),
			1,
			{3},
			{sizeof(int)}
		};
	py::array_t<int> bands_wann_nntot(bands_wann_nntot_buf);	
		
	size_t nn_dim1 = nntot;
	size_t nn_dim2 = num_kpts;	
	py::buffer_info nn_buf =
		{
			nn_,
			sizeof(int),
			py::format_descriptor<int>::format(),
			3,
			{nn_dim1, nn_dim2, 4},
			{num_kpts * 4 * sizeof(int), 4 * sizeof(int), sizeof(int)}
		};	
	py::array_t<int> nn(nn_buf);	

	int num_of_wann = num_wann_loc[0];
	size_t nwann = num_of_wann;
	py::buffer_info proj_site_loc_buf =
		{
			proj_site_loc,
			sizeof(double),
			py::format_descriptor<double>::format(),
			2,
			{nwann, 3},
			{3 * sizeof(double), sizeof(double)}
		};
	py::array_t<double>	proj_site(proj_site_loc_buf);	
	
	py::buffer_info proj_l_loc_buf =
		{
			proj_l_loc,
			sizeof(int),
			py::format_descriptor<int>::format(),
			1,
			{nwann},
			{sizeof(int)}
		};
	py::array_t<int> proj_l(proj_l_loc_buf);
	
	py::buffer_info proj_m_loc_buf =
		{
			proj_m_loc,
			sizeof(int),
			py::format_descriptor<int>::format(),
			1,
			{nwann},
			{sizeof(int)}
		};
	py::array_t<int> proj_m(proj_m_loc_buf);
	
	py::buffer_info proj_radial_loc_buf =
		{
			proj_radial_loc,
			sizeof(int),
			py::format_descriptor<int>::format(),
			1,
			{nwann},
			{sizeof(int)}
		};
	py::array_t<int> proj_radial(proj_radial_loc_buf);
	
	py::buffer_info proj_z_loc_buf =
		{
			proj_z_loc,
			sizeof(double),
			py::format_descriptor<double>::format(),
			2,
			{nwann, 3},
			{3 * sizeof(double), sizeof(double)}
		};
	py::array_t<double>	proj_z(proj_z_loc_buf);
	
	py::buffer_info proj_x_loc_buf =
		{
			proj_x_loc,
			sizeof(double),
			py::format_descriptor<double>::format(),
			2,
			{nwann, 3},
			{3 * sizeof(double), sizeof(double)}
		};
	py::array_t<double>	proj_x(proj_x_loc_buf);	
	
	py::buffer_info proj_zona_loc_buf =
		{
			proj_zona_loc,
			sizeof(double),
			py::format_descriptor<double>::format(),
			1,
			{nwann},
			{sizeof(double)}
		};		
	py::array_t<double>	proj_zona(proj_zona_loc_buf);	
	
	size_t nbands = num_bands;	
	py::buffer_info exclude_bands_loc_buf =
		{
			exclude_bands_loc,
			sizeof(int),
			py::format_descriptor<int>::format(),
			1,
			{nbands},
			{sizeof(int)}
		};
	py::array_t<int> exclude_bands(exclude_bands_loc_buf);
	
	py::buffer_info proj_s_loc_buf =
		{
			proj_s_loc,
			sizeof(int),
			py::format_descriptor<int>::format(),
			1,
			{nwann},
			{sizeof(int)}
		};
	py::array_t<int> proj_s(proj_s_loc_buf);	
	
	py::buffer_info proj_s_qaxis_loc_buf =
		{
			proj_s_qaxis_loc,
			sizeof(double),
			py::format_descriptor<double>::format(),
			2,
			{nwann, 3},
			{3 * sizeof(double), sizeof(double)}
		};
	py::array_t<double>	proj_s_qaxis(proj_s_qaxis_loc_buf);
	
	return {bands_wann_nntot, nn, proj_site, proj_l, proj_m, proj_radial, proj_z, proj_x, proj_zona, exclude_bands, proj_s, proj_s_qaxis};

}


std::vector<py::array_t<std::complex<double>>> run(char* seed__name, py::array_t<int> mp_grid_loc, int* num_kpts_loc, py::array_t<double> real_lattice_loc,
				py::array_t<double>  recip_lattice_loc, py::array_t<double>  kpt_latt_loc, int* num_bands_tot, int* num_bands_loc, int* num_wann_loc, int* nntot_loc,  		 					
			int* num_atoms_loc, py::array_t<int> atom_atomic_loc, py::array_t<double>  atoms_cart_loc, int* gamma_only_boolean,		 										
			py::array_t<std::complex<double>> M_matrix_loc, py::array_t<std::complex<double>> A_matrix_loc, py::array_t<double> eigenvalues_loc){

	std::cout << "You are running Wannier90_run via pyWannier90" << "\n";
	
	// Convert python arrays of inputs to pointers
	py::buffer_info mp_grid_loc_info = mp_grid_loc.request(); 
	int* mp_grid = static_cast<int*>(mp_grid_loc_info.ptr);	
	
	py::buffer_info real_lattice_loc_info = real_lattice_loc.request(); 
	double* real_lattice = static_cast<double*>(real_lattice_loc_info.ptr);		

	py::buffer_info recip_lattice_loc_info = recip_lattice_loc.request(); 
	double* recip_lattice = static_cast<double*>(recip_lattice_loc_info.ptr);

	py::buffer_info kpt_latt_loc_info = kpt_latt_loc.request(); 
	double* kpt_latt = static_cast<double*>(kpt_latt_loc_info.ptr);
	
	py::buffer_info atom_atomic_loc_info = atom_atomic_loc.request(); 
	int* atom_atomic = static_cast<int*>(atom_atomic_loc_info.ptr);	

	py::buffer_info atoms_cart_loc_info = atoms_cart_loc.request(); 
	double* atoms_cart = static_cast<double*>(atoms_cart_loc_info.ptr);
	
	py::buffer_info M_matrix_loc_info = M_matrix_loc.request(); 
	std::complex<double>* M_matrix = static_cast<std::complex<double>*>(M_matrix_loc_info.ptr);
	
	py::buffer_info A_matrix_loc_info = A_matrix_loc.request(); 
	std::complex<double>* A_matrix = static_cast<std::complex<double>*>(A_matrix_loc_info.ptr);

	py::buffer_info eigenvalues_loc_info = eigenvalues_loc.request(); 
	double* eigenvalues = static_cast<double*>(eigenvalues_loc_info.ptr);	

// For the wannier_run - start 
	int num_kpts = *num_kpts_loc;
	int num_wann = *num_wann_loc;
	int num_bands = *num_bands_loc;
	int size;
	size = num_wann*num_wann*num_kpts;
	std::complex<double> U_matrix_loc[size];
	
	size = num_bands*num_wann*num_kpts;
	std::complex<double> U_matrix_opt_loc[size];
	
	size = num_bands*num_kpts;
	int lwindow_loc[size];
	
	double wann_centres_loc[3*num_wann];
	double wann_spreads_loc[num_wann];
	double spread_loc[3];
	
	//////////////////////
	// Call wannier_run//
	/////////////////////
	
	__w90_MOD_wannier_run(seed__name, mp_grid, num_kpts_loc, real_lattice, 							    //input
	recip_lattice, kpt_latt, num_bands_tot, num_bands_loc, num_wann_loc, nntot_loc, 					//input
	num_atoms_loc, atom_atomic, atoms_cart, gamma_only_boolean, 										//input
	M_matrix, A_matrix, eigenvalues, 																	//input
	U_matrix_loc,  U_matrix_opt_loc, lwindow_loc, 														//output
	wann_centres_loc, wann_spreads_loc, spread_loc);													//output
	
	
	// Pass output to python
		
	size_t size1 = num_kpts;
	size_t size2 = num_bands;	
	size_t size3 = num_wann;	
	size_t size4 = size1 * size2;	

	py::buffer_info U_matrix_loc_buf =
		{
			U_matrix_loc,
			sizeof(std::complex<double>),
			py::format_descriptor<std::complex<double>>::format(),
			3,
			{size1, size3, size3},
			{size3 * size3 * sizeof(std::complex<double>), size3 * sizeof(std::complex<double>), sizeof(std::complex<double>)}
		};		
	py::array_t<std::complex<double>> U_matrix(U_matrix_loc_buf);

	py::buffer_info U_matrix_opt_loc_buf =
		{
			U_matrix_opt_loc,
			sizeof(std::complex<double>),
			py::format_descriptor<std::complex<double>>::format(),
			3,
			{size1, size3, size2},
			{size3 * size2 * sizeof(std::complex<double>), size2 * sizeof(std::complex<double>), sizeof(std::complex<double>)}
		};
	py::array_t<std::complex<double>> U_matrix_opt(U_matrix_opt_loc_buf);
	
	
	py::buffer_info lwindow_loc_buf =
		{
			lwindow_loc,
			sizeof(int),
			py::format_descriptor<int>::format(),
			2,
			{size1, size2},
			{size2 * sizeof(int), sizeof(int)}
		};
	py::array_t<int> lwindow(lwindow_loc_buf);
	
	py::buffer_info wann_centres_loc_buf =
		{
			wann_centres_loc,
			sizeof(double),
			py::format_descriptor<double>::format(),
			2,
			{size3, 3},
			{3 * sizeof(double), sizeof(double)}
		};
	py::array_t<double> wann_centres(wann_centres_loc_buf);
	
	py::buffer_info wann_spreads_loc_buf =
		{
			wann_spreads_loc,
			sizeof(double),
			py::format_descriptor<double>::format(),
			1,
			{size3},
			{sizeof(double)}
		};
	py::array_t<double> wann_spreads(wann_spreads_loc_buf);

	py::buffer_info spread_loc_buf =
		{
			spread_loc,
			sizeof(double),
			py::format_descriptor<double>::format(),
			1,
			{3},
			{sizeof(double)}
		};
	py::array_t<double> spread(spread_loc_buf);
	
	return {U_matrix, U_matrix_opt, lwindow, wann_centres, wann_spreads, spread};
	
}

PYBIND11_PLUGIN(libwannier90)
{
	py::module m("libwannier90", "A python/C++ wrapper for Wannier90");	
	m.def("setup", &setup, "Run wannier90_setup to get necessary info to construct M and A matrices");
	m.def("run", &run, "Run wannier90_run to get the MLWFs");	
	return m.ptr();
}