//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC//    Version 1.0; svn revision $LastChangedRevision$
//LIC//
//LIC// $LastChangedDate$
//LIC// 
//LIC// Copyright (C) 2006-2016 Matthias Heil and Andrew Hazel
//LIC// 
//LIC// This library is free software; you can redistribute it and/or
//LIC// modify it under the terms of the GNU Lesser General Public
//LIC// License as published by the Free Software Foundation; either
//LIC// version 2.1 of the License, or (at your option) any later version.
//LIC// 
//LIC// This library is distributed in the hope that it will be useful,
//LIC// but WITHOUT ANY WARRANTY; without even the implied warranty of
//LIC// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//LIC// Lesser General Public License for more details.
//LIC// 
//LIC// You should have received a copy of the GNU Lesser General Public
//LIC// License along with this library; if not, write to the Free Software
//LIC// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
//LIC// 02110-1301  USA.
//LIC// 
//LIC// The authors may be contacted at oomph-lib@maths.man.ac.uk.
//LIC// 
//LIC//====================================================================
//Header file for Fourier-decomposed Helmholtz elements
#ifndef OOMPH_PML_FOURIER_DECOMPOSED_HELMHOLTZ_ELEMENTS_HEADER
#define OOMPH_PML_FOURIER_DECOMPOSED_HELMHOLTZ_ELEMENTS_HEADER


// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

#include "math.h"
#include <complex>


//OOMPH-LIB headers
#include "../generic/projection.h"
#include "../generic/nodes.h"
#include "../generic/Qelements.h"
#include "../generic/oomph_utilities.h"
#include "../generic/pml_meshes.h"
#include "../generic/projection.h"
#include "../generic/oomph_definitions.h"


namespace oomph
{


//========================================================================
/// Helper namespace for functions required for Helmholtz computations
//========================================================================
 namespace Legendre_functions_helper
 {

  /// Factorial
  extern double factorial(const unsigned& l);

  /// Legendre polynomials depending on one parameter
  extern double plgndr1(const unsigned& n, const double& x);

  /// Legendre polynomials depending on two parameters
  extern double plgndr2(const unsigned& l, const unsigned& m, const double& x);

 } // end namespace


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////



//=============================================================
/// A class for all isoparametric elements that solve the
/// Helmholtz equations with pml capabilities.
/// in Fourier decomposed form (cylindrical polars):
/// \f[
/// U(r,\varphi,z) = \Re( u^{(n)}(r,z) \exp(-i n \varphi))
/// \f]
/// We are solving for \f$ u^{(n)}(r,z)\f$ for given parameters
/// \f$ k^2 \f$ and \f$ n \f$ .
/// This contains the generic maths. Shape functions, geometric
/// mapping etc. must get implemented in derived class.
//=============================================================
 template <class PML_ELEMENT>
 class PMLFourierDecomposedHelmholtzEquations :
  public virtual PML_ELEMENT,
  public virtual FiniteElement
 {

   public:

  /// \short Function pointer to source function fct(x,f(x)) --
  /// x is a Vector!
  typedef void (*PMLFourierDecomposedHelmholtzSourceFctPt)(
   const Vector<double>& x,
   std::complex<double>& f);

  /// Constructor
   PMLFourierDecomposedHelmholtzEquations() : Source_fct_pt(0),
   K_squared_pt(0), N_pml_fourier_pt(0)
   {
     // Provide pointer to static method (Save memory)
     this->Pml_mapping_pt = 
     &PMLFourierDecomposedHelmholtzEquations::Default_pml_mapping;
     
     Alpha_pt = &Default_Physical_Constant_Value;
   }


  /// Broken copy constructor
  PMLFourierDecomposedHelmholtzEquations(
   const PMLFourierDecomposedHelmholtzEquations& dummy)
   {
    BrokenCopy::broken_copy("PMLFourierDecomposedHelmholtzEquations");
   }

  /// Broken assignment operator
//Commented out broken assignment operator because this can lead to a conflict warning
//when used in the virtual inheritence hierarchy. Essentially the compiler doesn't
//realise that two separate implementations of the broken function are the same and so,
//quite rightly, it shouts.
  /*void operator=(const PMLFourierDecomposedHelmholtzEquations&)
   {
    BrokenCopy::broken_assign
     ("PMLFourierDecomposedHelmholtzEquations");
     }*/

  /// \short Return the index at which the unknown value
  /// is stored: Real/imag part of index contains (real) index of
  /// real/imag part.
  virtual inline std::complex<unsigned>
   u_index_pml_fourier_decomposed_helmholtz()
   const
   {return std::complex<unsigned>(0,1);}


   /// Get pointer to frequency
   double*& k_squared_pt()
    {
     return K_squared_pt;
    }


   /// Get k squared
   double k_squared() const
   {
 #ifdef PARANOID
    if (K_squared_pt==0)
     {
      throw OomphLibError(
       "Please set pointer to k_squared using access fct to pointer!",
       OOMPH_CURRENT_FUNCTION,
       OOMPH_EXCEPTION_LOCATION);
     }
 #endif
    return *K_squared_pt;
   }
   
   /// Wavenumber used in PML
   double wavenumber() const { return std::sqrt(k_squared()); }
   
   /// Get pointer to complex shift
   double*& alpha_pt()
    {
     return Alpha_pt;
    }


   /// Get complex shift
   double alpha()
   {
    return *Alpha_pt;
   }

  /// Get pointer to Fourier wavenumber
  int*& pml_fourier_wavenumber_pt()
  {
   return N_pml_fourier_pt;
  }

  /// Get the Fourier wavenumber
  int pml_fourier_wavenumber()
  {
   if (N_pml_fourier_pt==0)
    {
     return 0;
    }
   else
    {
     return *N_pml_fourier_pt;
    }
  }


 /// Output with default number of plot points
 void output(std::ostream &outfile)
  {
   const unsigned n_plot=5;
   output(outfile,n_plot);
  }

 /// \short Output FE representation of soln: x,y,u_re,u_im or
 /// x,y,z,u_re,u_im at  n_plot^2 plot points
 void output(std::ostream &outfile, const unsigned &n_plot);

 /// \short Output function for real part of full time-dependent solution
 /// u = Re( (u_r +i u_i) exp(-i omega t)
 /// at phase angle omega t = phi.
 /// r,z,u at n_plot plot points in each coordinate
 /// direction
 void output_real(std::ostream &outfile, const double& phi,
                  const unsigned &n_plot);

 /// C_style output with default number of plot points
 void output(FILE* file_pt)
  {
   const unsigned n_plot=5;
   output(file_pt,n_plot);
  }

 /// \short C-style output FE representation of soln: r,z,u_re,u_im or
 /// at n_plot^2 plot points
 void output(FILE* file_pt, const unsigned &n_plot);

 /// Output exact soln: r,z,u_re_exact,u_im_exact
 /// at n_plot^2 plot points
 void output_fct(std::ostream &outfile, const unsigned &n_plot,
                 FiniteElement::SteadyExactSolutionFctPt exact_soln_pt);

 /// \short Output exact soln: (dummy time-dependent version to
 /// keep intel compiler happy)
 virtual void output_fct(std::ostream &outfile, const unsigned &n_plot,
                         const double& time,
                         FiniteElement::UnsteadyExactSolutionFctPt
                         exact_soln_pt)
  {
   throw OomphLibError(
    "There is no time-dependent output_fct() for PMLFourierDecomposedHelmholtz elements ",
    OOMPH_CURRENT_FUNCTION,
    OOMPH_EXCEPTION_LOCATION);
  }


 /// \short Output function for real part of full time-dependent fct
 /// u = Re( (u_r +i u_i) exp(-i omega t)
 /// at phase angle omega t = phi.
 /// r,z,u at n_plot plot points in each coordinate
 /// direction
 void output_real_fct(std::ostream &outfile,
                      const double& phi,
                      const unsigned &n_plot,
                      FiniteElement::SteadyExactSolutionFctPt exact_soln_pt);


 /// Get error against and norm of exact solution
 void compute_error(std::ostream &outfile,
                    FiniteElement::SteadyExactSolutionFctPt exact_soln_pt,
                    double& error, double& norm);


 /// Dummy, time dependent error checker
 void compute_error(std::ostream &outfile,
                    FiniteElement::UnsteadyExactSolutionFctPt exact_soln_pt,
                    const double& time, double& error, double& norm)
 {
  throw OomphLibError(
   "There is no time-dependent compute_error() for PMLFourierDecomposedHelmholtz elements",
   OOMPH_CURRENT_FUNCTION,
   OOMPH_EXCEPTION_LOCATION);
 }

 /// Compute norm of fe solution
 void compute_norm(double& norm);


 /// Access function: Pointer to source function
 PMLFourierDecomposedHelmholtzSourceFctPt& source_fct_pt()
  {return Source_fct_pt;}


 /// Access function: Pointer to source function. Const version
 PMLFourierDecomposedHelmholtzSourceFctPt source_fct_pt() const
 {return Source_fct_pt;}


 /// Get source term at (Eulerian) position x. This function is
 /// virtual to allow overloading in multi-physics problems where
 /// the strength of the source function might be determined by
 /// another system of equations.
 inline virtual void get_source_pml_fourier_decomposed_helmholtz(
  const unsigned& ipt,
  const Vector<double>& x,
  std::complex<double>& source) const
 {
  //If no source function has been set, return zero
  if(Source_fct_pt==0)
   {
    source = std::complex<double>(0.0,0.0);
   }
  else
   {
    // Get source strength
    (*Source_fct_pt)(x,source);
   }
 }

 /// \short Pure virtual function in which we specify the
 /// values to be pinned (and set to zero) on the outer edge of
 /// the pml layer. All of them! Vector is resized internally.
 void values_to_be_pinned_on_outer_pml_boundary(Vector<unsigned>&
                                                values_to_pin)
 {
  values_to_pin.resize(2);
  for (unsigned j=0;j<2;j++)
   {
    values_to_pin[j]=j;
   }
 }


 /// Get flux: flux[i] = du/dx_i for real and imag part
 void get_flux(const Vector<double>& s,
               Vector<std::complex <double> >& flux) const
  {
   //Find out how many nodes there are in the element
   const unsigned n_node = nnode();

   //Set up memory for the shape and test functions
   Shape psi(n_node);
   DShape dpsidx(n_node,2);

   //Call the derivatives of the shape and test functions
   dshape_eulerian(s,psi,dpsidx);

   //Initialise to zero
   const std::complex<double> zero(0.0,0.0);
   for(unsigned j=0;j<2;j++)
    {
     flux[j] = zero;
    }

   // Loop over nodes
   for(unsigned l=0;l<n_node;l++)
    {
     //Cache the complex value of the unknown
     const std::complex<double> u_value(
      this->nodal_value
      (l,u_index_pml_fourier_decomposed_helmholtz().real()),
      this->nodal_value
      (l,u_index_pml_fourier_decomposed_helmholtz().imag()));

     //Loop over derivative directions
     for(unsigned j=0;j<2;j++)
      {
       flux[j] += u_value*dpsidx(l,j);
      }
    }
  }


 /// Add the element's contribution to its residual vector (wrapper)
 void fill_in_contribution_to_residuals(Vector<double> &residuals)
 {
  //Call the generic residuals function with flag set to 0
  //using a dummy matrix argument
  fill_in_generic_residual_contribution_pml_fourier_decomposed_helmholtz(
   residuals,GeneralisedElement::Dummy_matrix,0);
 }



 /// \short Add the element's contribution to its residual vector and
 /// element Jacobian matrix (wrapper)
 void fill_in_contribution_to_jacobian(Vector<double> &residuals,
                                       DenseMatrix<double> &jacobian)
 {
  //Call the generic routine with the flag set to 1
  fill_in_generic_residual_contribution_pml_fourier_decomposed_helmholtz(
   residuals,jacobian,1);
 }


 /// \short Return FE representation of function value u(s)
 /// at local coordinate s
 inline std::complex<double>
  interpolated_u_pml_fourier_decomposed_helmholtz(
  const Vector<double> &s) const
 {
  //Find number of nodes
  const unsigned n_node = nnode();

  //Local shape function
  Shape psi(n_node);

  //Find values of shape function
  shape(s,psi);

  //Initialise value of u
  std::complex<double> interpolated_u(0.0,0.0);

  //Get the index at which the helmholtz unknown is stored
  const unsigned u_nodal_index_real =
   u_index_pml_fourier_decomposed_helmholtz().real();
  const unsigned u_nodal_index_imag =
   u_index_pml_fourier_decomposed_helmholtz().imag();

  //Loop over the local nodes and sum
  for(unsigned l=0;l<n_node;l++)
   {
    //Make a temporary complex number from the stored data
    const std::complex<double> u_value(
     this->nodal_value(l,u_nodal_index_real),
     this->nodal_value(l,u_nodal_index_imag));
    //Add to the interpolated value
    interpolated_u += u_value*psi[l];
   }
  return interpolated_u;
 }


 /// \short Self-test: Return 0 for OK
 unsigned self_test();


protected:



 /// Static so that the class doesn't need to instantiate a new default
 /// everytime it uses it
 static BermudezPMLMapping Default_pml_mapping;


 /// \short Shape/test functions and derivs w.r.t. to global coords at
 /// local coord. s; return  Jacobian of mapping
 virtual double
  dshape_and_dtest_eulerian_pml_fourier_decomposed_helmholtz(
  const Vector<double> &s,
  Shape &psi,
  DShape &dpsidx, Shape &test,
  DShape &dtestdx) const=0;


 /// \short Shape/test functions and derivs w.r.t. to global coords at
 /// integration point ipt; return  Jacobian of mapping
 virtual double
  dshape_and_dtest_eulerian_at_knot_pml_fourier_decomposed_helmholtz(
   const unsigned &ipt,
   Shape &psi,
   DShape &dpsidx,
   Shape &test,
   DShape &dtestdx) const=0;

 /// \short Compute element residual Vector only (if flag=and/or element
 /// Jacobian matrix
 virtual void
  fill_in_generic_residual_contribution_pml_fourier_decomposed_helmholtz(
   Vector<double> &residuals, DenseMatrix<double> &jacobian,
   const unsigned& flag);

 /// Pointer to source function:
 PMLFourierDecomposedHelmholtzSourceFctPt Source_fct_pt;

 /// Pointer to k^2 (wavenumber squared)
 double* K_squared_pt;
 
 /// Pointer to wavenumber complex shift
 double *Alpha_pt;

 /// Static default value for the physical constants (initialised to zero)
 static double Default_Physical_Constant_Value;

 /// Pointer to Fourier wave number
 int* N_pml_fourier_pt;
 
 };




///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////



//======================================================================
/// QPMLFourierDecomposedHelmholtzElement elements are
/// linear/quadrilateral/brick-shaped PMLFourierDecomposedHelmholtz
/// elements with isoparametric interpolation for the function.
//======================================================================
 template <unsigned NNODE_1D, class PML_ELEMENT>
  class QPMLFourierDecomposedHelmholtzElement :
  public virtual QElement<2,NNODE_1D>,
  public virtual PMLFourierDecomposedHelmholtzEquations<PML_ELEMENT>
  {

    private:

   /// \short Static int that holds the number of variables at
   /// nodes: always the same
   static const unsigned Initial_Nvalue;

    public:


   ///\short  Constructor: Call constructors for QElement and
   /// PMLFourierDecomposedHelmholtz equations
    QPMLFourierDecomposedHelmholtzElement() :
   QElement<2,NNODE_1D>(), PMLFourierDecomposedHelmholtzEquations<PML_ELEMENT>()
    {}

   /// Broken copy constructor
   QPMLFourierDecomposedHelmholtzElement(
    const QPMLFourierDecomposedHelmholtzElement<NNODE_1D,PML_ELEMENT>& dummy)
    {
     BrokenCopy::broken_copy("QPMLFourierDecomposedHelmholtzElement");
    }

   /// Broken assignment operator
   /*void operator=(const
                  QPMLFourierDecomposedHelmholtzElement<NNODE_1D>&)
    {
     BrokenCopy::broken_assign
      ("QPMLFourierDecomposedHelmholtzElement");
      }*/


   /// \short  Required  # of `values' (pinned or dofs)
   /// at node n
   inline unsigned required_nvalue(const unsigned &n) const
   {return Initial_Nvalue;}

   /// \short Output function: r,z,u
   void output(std::ostream &outfile)
   {PMLFourierDecomposedHelmholtzEquations<PML_ELEMENT>::output(outfile);}

   ///  \short Output function:
   ///   r,z,u at n_plot^2 plot points
   void output(std::ostream &outfile, const unsigned &n_plot)
   {PMLFourierDecomposedHelmholtzEquations<PML_ELEMENT>::output(outfile,n_plot);}

   /// \short Output function for real part of full time-dependent solution
   /// u = Re( (u_r +i u_i) exp(-i omega t)
   /// at phase angle omega t = phi.
   /// r,z,u  at n_plot plot points in each coordinate
   /// direction
   void output_real(std::ostream &outfile, const double& phi,
                    const unsigned &n_plot)
   {PMLFourierDecomposedHelmholtzEquations<PML_ELEMENT>::output_real
     (outfile,phi,n_plot);}

   /// \short C-style output function:  r,z,u
   void output(FILE* file_pt)
   {PMLFourierDecomposedHelmholtzEquations<PML_ELEMENT>::output(file_pt);}

   ///  \short C-style output function:
   ///   r,z,u  at n_plot^2 plot points
   void output(FILE* file_pt, const unsigned &n_plot)
   {PMLFourierDecomposedHelmholtzEquations<PML_ELEMENT>::output(file_pt,n_plot);}

   /// \short Output function for an exact solution:
   /// r,z,u_exact at n_plot^2 plot points
   void output_fct(std::ostream &outfile, const unsigned &n_plot,
                   FiniteElement::SteadyExactSolutionFctPt exact_soln_pt)
   {
    PMLFourierDecomposedHelmholtzEquations<PML_ELEMENT>::
      output_fct(outfile, n_plot, exact_soln_pt);
   }

   /// \short Output function for real part of full time-dependent fct
   /// u = Re( (u_r +i u_i) exp(-i omega t)
   /// at phase angle omega t = phi.
   /// r,z,u  at n_plot plot points in each coordinate
   /// direction
   void output_real_fct(std::ostream &outfile,
                        const double& phi,
                        const unsigned &n_plot,
                        FiniteElement::SteadyExactSolutionFctPt exact_soln_pt)
   {
    PMLFourierDecomposedHelmholtzEquations<PML_ELEMENT>::
      output_real_fct(outfile, phi, n_plot, exact_soln_pt);
   }


   /// \short Output function for a time-dependent exact solution.
   ///  r,z,u_exact at n_plot^2 plot points
   /// (Calls the steady version)
   void output_fct(std::ostream &outfile, const unsigned &n_plot,
                   const double& time,
                   FiniteElement::UnsteadyExactSolutionFctPt exact_soln_pt)
   {
    PMLFourierDecomposedHelmholtzEquations<PML_ELEMENT>::output_fct
     (outfile,n_plot,time,exact_soln_pt);
   }

    protected:

   /// Shape, test functions & derivs. w.r.t. to global coords.
   /// Return Jacobian.
   inline double
    dshape_and_dtest_eulerian_pml_fourier_decomposed_helmholtz(
     const Vector<double> &s, Shape &psi, DShape &dpsidx,
     Shape &test, DShape &dtestdx) const;


   /// \short Shape, test functions & derivs. w.r.t. to global coords. at
   /// integration point ipt. Return Jacobian.
   inline double
    dshape_and_dtest_eulerian_at_knot_pml_fourier_decomposed_helmholtz(
     const unsigned& ipt,
     Shape &psi,
     DShape &dpsidx,
     Shape &test,
     DShape &dtestdx) const;

  };




//Inline functions:


//======================================================================
/// Define the shape functions and test functions and derivatives
/// w.r.t. global coordinates and return Jacobian of mapping.
///
/// Galerkin: Test functions = shape functions
//======================================================================
 template<unsigned NNODE_1D, class PML_ELEMENT>
 double QPMLFourierDecomposedHelmholtzElement<NNODE_1D,PML_ELEMENT>::
  dshape_and_dtest_eulerian_pml_fourier_decomposed_helmholtz(
   const Vector<double> &s,
   Shape &psi,
   DShape &dpsidx,
   Shape &test,
   DShape &dtestdx) const
  {
   //Call the geometrical shape functions and derivatives
   const double J = this->dshape_eulerian(s,psi,dpsidx);

   //Set the test functions equal to the shape functions
   test = psi;
   dtestdx= dpsidx;

   //Return the jacobian
   return J;
  }




//======================================================================
/// Define the shape functions and test functions and derivatives
/// w.r.t. global coordinates and return Jacobian of mapping.
///
/// Galerkin: Test functions = shape functions
//======================================================================
 template<unsigned NNODE_1D, class PML_ELEMENT>
  double QPMLFourierDecomposedHelmholtzElement<NNODE_1D,PML_ELEMENT>::
  dshape_and_dtest_eulerian_at_knot_pml_fourier_decomposed_helmholtz(
   const unsigned &ipt,
   Shape &psi,
   DShape &dpsidx,
   Shape &test,
   DShape &dtestdx) const
  {
   //Call the geometrical shape functions and derivatives
   const double J = this->dshape_eulerian_at_knot(ipt,psi,dpsidx);

   //Set the pointers of the test functions
   test = psi;
   dtestdx = dpsidx;

   //Return the jacobian
   return J;
  }

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////



//=======================================================================
/// Face geometry for the QPMLFourierDecomposedHelmholtzElement
/// elements:
/// The spatial dimension of the face elements is one lower than that of the
/// bulk element but they have the same number of points
/// along their 1D edges.
//=======================================================================
 template<unsigned NNODE_1D, class PML_ELEMENT>
  class FaceGeometry<QPMLFourierDecomposedHelmholtzElement<NNODE_1D,PML_ELEMENT> >:
  public virtual QElement<1,NNODE_1D>
  {

    public:

    /// \short Constructor: Call the constructor for the
    /// appropriate lower-dimensional QElement
    FaceGeometry() : QElement<1,NNODE_1D>() {}
  };


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////



//==========================================================
/// Fourier decomposed Helmholtz upgraded to become projectable
//==========================================================
 template<class FOURIER_DECOMPOSED_HELMHOLTZ_ELEMENT>
 class ProjectablePMLFourierDecomposedHelmholtzElement :
  public virtual ProjectableElement<FOURIER_DECOMPOSED_HELMHOLTZ_ELEMENT>
 {

 public:


  /// \short Constructor [this was only required explicitly
  /// from gcc 4.5.2 onwards...]
  ProjectablePMLFourierDecomposedHelmholtzElement(){}

  /// \short Specify the values associated with field fld.
  /// The information is returned in a vector of pairs which comprise
  /// the Data object and the value within it, that correspond to field fld.
  Vector<std::pair<Data*,unsigned> > data_values_of_field(const unsigned& fld)
   {

#ifdef PARANOID
    if (fld>1)
     {
      std::stringstream error_stream;
      error_stream
       << "Fourier decomposed Helmholtz elements only store 2 fields so fld = "
       << fld << " is illegal \n";
      throw OomphLibError(
       error_stream.str(),
       OOMPH_CURRENT_FUNCTION,
       OOMPH_EXCEPTION_LOCATION);
     }
#endif

    // Create the vector
    unsigned nnod=this->nnode();
    Vector<std::pair<Data*,unsigned> > data_values(nnod);

    // Loop over all nodes
    for (unsigned j=0;j<nnod;j++)
     {
      // Add the data value associated field: The node itself
      data_values[j]=std::make_pair(this->node_pt(j),fld);
     }

    // Return the vector
    return data_values;
   }

  /// \short Number of fields to be projected: 2 (real and imag part)
  unsigned nfields_for_projection()
   {
    return 2;
   }

  /// \short Number of history values to be stored for fld-th field.
  /// (Note: count includes current value!)
  unsigned nhistory_values_for_projection(const unsigned &fld)
  {
#ifdef PARANOID
    if (fld>1)
     {
      std::stringstream error_stream;
      error_stream
       << "Helmholtz elements only store two fields so fld = "
       << fld << " is illegal\n";
      throw OomphLibError(
       error_stream.str(),
       OOMPH_CURRENT_FUNCTION,
       OOMPH_EXCEPTION_LOCATION);
     }
#endif
   return this->node_pt(0)->ntstorage();
  }

  ///\short Number of positional history values
  /// (Note: count includes current value!)
  unsigned nhistory_values_for_coordinate_projection()
   {
    return this->node_pt(0)->position_time_stepper_pt()->ntstorage();
   }

  /// \short Return Jacobian of mapping and shape functions of field fld
  /// at local coordinate s
  double jacobian_and_shape_of_field(const unsigned &fld,
                                     const Vector<double> &s,
                                     Shape &psi)
   {
#ifdef PARANOID
    if (fld>1)
     {
      std::stringstream error_stream;
      error_stream
       << "Helmholtz elements only store two fields so fld = "
       << fld << " is illegal.\n";
      throw OomphLibError(
       error_stream.str(),
       OOMPH_CURRENT_FUNCTION,
       OOMPH_EXCEPTION_LOCATION);
     }
#endif
    unsigned n_dim=this->dim();
    unsigned n_node=this->nnode();
    Shape test(n_node);
    DShape dpsidx(n_node,n_dim), dtestdx(n_node,n_dim);
    double J=this->
     dshape_and_dtest_eulerian_pml_fourier_decomposed_helmholtz(
      s,psi,dpsidx,
      test,dtestdx);
    return J;
   }



  /// \short Return interpolated field fld at local coordinate s, at time level
  /// t (t=0: present; t>0: history values)
  double get_field(const unsigned &t,
                   const unsigned &fld,
                   const Vector<double>& s)
   {
#ifdef PARANOID
    if (fld>1)
     {
      std::stringstream error_stream;
      error_stream
       << "Helmholtz elements only store two fields so fld = "
       << fld << " is illegal\n";
      throw OomphLibError(
       error_stream.str(),
       OOMPH_CURRENT_FUNCTION,
       OOMPH_EXCEPTION_LOCATION);
     }
#endif
    //Find the index at which the variable is stored
    std::complex<unsigned> complex_u_nodal_index =
     this->u_index_pml_fourier_decomposed_helmholtz();
    unsigned u_nodal_index = 0;
    if (fld==0)
     {
      u_nodal_index = complex_u_nodal_index.real();
     }
    else
     {
      u_nodal_index = complex_u_nodal_index.imag();
     }


      //Local shape function
    unsigned n_node=this->nnode();
    Shape psi(n_node);

    //Find values of shape function
    this->shape(s,psi);

    //Initialise value of u
    double interpolated_u = 0.0;

    //Sum over the local nodes
    for(unsigned l=0;l<n_node;l++)
     {
      interpolated_u += this->nodal_value(t,l,u_nodal_index)*psi[l];
     }
    return interpolated_u;
   }




  ///Return number of values in field fld: One per node
  unsigned nvalue_of_field(const unsigned &fld)
   {
#ifdef PARANOID
    if (fld>1)
     {
      std::stringstream error_stream;
      error_stream
       << "Helmholtz elements only store two fields so fld = "
       << fld << " is illegal\n";
      throw OomphLibError(
       error_stream.str(),
       OOMPH_CURRENT_FUNCTION,
       OOMPH_EXCEPTION_LOCATION);
     }
#endif
    return this->nnode();
   }


  ///Return local equation number of value j in field fld.
  int local_equation(const unsigned &fld,
                     const unsigned &j)
   {
#ifdef PARANOID
    if (fld>1)
     {
      std::stringstream error_stream;
      error_stream
       << "Helmholtz elements only store two fields so fld = "
       << fld << " is illegal\n";
      throw OomphLibError(
       error_stream.str(),
       OOMPH_CURRENT_FUNCTION,
       OOMPH_EXCEPTION_LOCATION);
     }
#endif
    std::complex<unsigned> complex_u_nodal_index =
     this->u_index_pml_fourier_decomposed_helmholtz();
    unsigned u_nodal_index = 0;
    if (fld==0)
     {
      u_nodal_index = complex_u_nodal_index.real();
     }
    else
     {
      u_nodal_index = complex_u_nodal_index.imag();
     }
    return this->nodal_local_eqn(j,u_nodal_index);
   }




 /// \short Output FE representation of soln: x,y,u or x,y,z,u at
 /// n_plot^DIM plot points
 void output(std::ostream &outfile, const unsigned &nplot)
 {
  FOURIER_DECOMPOSED_HELMHOLTZ_ELEMENT::output(outfile,nplot);
 }


 };


//=======================================================================
/// Face geometry for element is the same as that for the underlying
/// wrapped element
//=======================================================================
 template<class ELEMENT>
 class FaceGeometry
  <ProjectablePMLFourierDecomposedHelmholtzElement<ELEMENT> >
  : public virtual FaceGeometry<ELEMENT>
  {
    public:
   FaceGeometry() : FaceGeometry<ELEMENT>() {}
  };


//=======================================================================
/// Face geometry of the Face Geometry for element is the same as
/// that for the underlying wrapped element
//=======================================================================
 template<class ELEMENT>
 class FaceGeometry<FaceGeometry
  <ProjectablePMLFourierDecomposedHelmholtzElement<ELEMENT> > >
  : public virtual FaceGeometry<FaceGeometry<ELEMENT> >
  {
 public:
  FaceGeometry() : FaceGeometry<FaceGeometry<ELEMENT> >() {}
 };



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

//=======================================================================
/// Policy class defining the elements to be used in the actual
/// PML layers. Same!
//=======================================================================
  template<unsigned NNODE_1D, class PML_ELEMENT>
class EquivalentQElement<
 QPMLFourierDecomposedHelmholtzElement<NNODE_1D,PML_ELEMENT> > :
 public virtual QPMLFourierDecomposedHelmholtzElement<NNODE_1D, PML_ELEMENT>
{

  public:

 /// \short Constructor: Call the constructor for the
 /// appropriate QElement
 EquivalentQElement() :
  QPMLFourierDecomposedHelmholtzElement<NNODE_1D,PML_ELEMENT>()
  {}

};

}

#endif
