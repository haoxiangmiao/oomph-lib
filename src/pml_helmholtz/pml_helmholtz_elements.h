//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented,
//LIC// multi-physics finite-element library, available
//LIC// at http://www.oomph-lib.org.
//LIC//
//LIC//           Version 0.90. August 3, 2009.
//LIC//
//LIC// Copyright (C) 2006-2009 Matthias Heil and Andrew Hazel
//LIC//
//LIC// This library is free software; you can redistribute it and/or
//LIC// modify it under the terms of the GNU Lesser General Public
//LIC// License as published by the Free Software Foundation; either
//LIC// version 2.1 of the License, or (at your option) any later version.
//LIC//
//LIC// This library is distributed in the hope that it will be useful,
//LIC// but WITHOUT ANY WARRANTY; without even the implied warranty of
//LIC// MERCHANTABILITY or FITNESS_pt FOR A PARTICULAR PURPOSE.  See the GNU
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
//Header file for Helmholtz elements
#ifndef OOMPH_PML_HELMHOLTZ_ELEMENTS_HEADER
#define OOMPH_PML_HELMHOLTZ_ELEMENTS_HEADER


// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

#include "math.h"
#include <complex>

//OOMPH-LIB headers
#include "../generic/nodes.h"
#include "../generic/Qelements.h"
#include "../generic/oomph_utilities.h"
#include "../generic/pml_meshes.h"
#include "../generic/projection.h"

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

namespace oomph
{
  //=======================================================================
  /// Class to hold the mapping function for the PML
  ///
  //=======================================================================
  class PMLMapping
  {

  public:

    /// Default constructor (empty)
    PMLMapping(){};

   /// \short Pure virtual to return PML mapping gamma, where gamma is the
   /// \f$d\tilde x / d x\f$ as  function of \f$\nu\f$ where \f$\nu = x - h\f$ where h is
   /// the vector from the origin to the start of the PML
   virtual std::complex<double> gamma(const double& nu_i,
     const double& pml_width_i,
     const double& k_squared_local) = 0;

  };

  //=======================================================================
  /// The mapping function propsed by Bermudez et al, appears to be the best
  /// and so this will be the default mapping (see definition of
  /// PmlHelmholtzEquations)
  //=======================================================================
  class BermudezPMLMapping : public PMLMapping
  {

    public:

    /// Default constructor (empty)
    BermudezPMLMapping(){};

    /// \short Overwrite the pure PML mapping coefficient function to return the
    /// coeffcients proposed by Bermudez et al
    std::complex<double> gamma(const double& nu_i,
                               const double& pml_width_i,
                               const double& k_squared_local)
    {
       /// \short return \f$\gamma=1 + (1/k)(i/|outer_boundary - x|)\f$ or more
       /// abstractly \f$\gamma = 1 + \frac i {k\delta_{pml}}(1/|1-\bar\nu|)\f$
       return 1.0 + (1.0 / std::complex<double> (sqrt(k_squared_local), 0))
        * std::complex<double>
         (0.0, 1.0/(std::fabs(pml_width_i - nu_i)));
    }

  };


  ////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////

//=============================================================
/// A class for all isoparametric elements that solve the
/// Helmholtz equations with pml capabilities.
/// This contains the generic maths. Shape functions, geometric
/// mapping etc. must get implemented in derived class.
//=============================================================
template <unsigned DIM>
 class PmlHelmholtzEquations :
 public virtual PMLElementBase<DIM>,
 public virtual FiniteElement
{

public:

 /// \short Function pointer to source function fct(x,f(x)) --
 /// x is a Vector!
 typedef void (*PmlHelmholtzSourceFctPt)(const Vector<double>& x,
                                      std::complex<double>& f);

 /// Constructor
 PmlHelmholtzEquations() : Source_fct_pt(0),
  K_squared_pt(0)
  {
    // Provide pointer to static method (Save memory)
    Pml_mapping_pt = &PmlHelmholtzEquations::Default_pml_mapping;
    Alpha_pt = &Default_Physical_Constant_Value;
  }


 /// Broken copy constructor
 PmlHelmholtzEquations(const PmlHelmholtzEquations& dummy)
  {
   BrokenCopy::broken_copy("PmlHelmholtzEquations");
  }

 /// Broken assignment operator
 void operator=(const PmlHelmholtzEquations&)
  {
   BrokenCopy::broken_assign("PmlHelmholtzEquations");
  }

 /// \short Return the index at which the unknown value
 /// is stored.
 virtual inline std::complex<unsigned> u_index_helmholtz() const
 {return std::complex<unsigned>(0,1);}

  /// Get pointer to k_squared
  double*& k_squared_pt(){ return K_squared_pt; }


  /// Get the square of wavenumber
 double k_squared()
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
 
 /// Alpha, wavenumber complex shift
const double &alpha() const {return *Alpha_pt;}

/// Pointer to Alpha, wavenumber complex shift
double* &alpha_pt() {return Alpha_pt;}


 /// \short Number of scalars/fields output by this element. Reimplements
 /// broken virtual function in base class.
 unsigned nscalar_paraview() const
  {
   return 2;
  }

 /// \short Write values of the i-th scalar field at the plot points. Needs
 /// to be implemented for each new specific element type.
 void scalar_value_paraview(std::ofstream& file_out,
                            const unsigned& i,
                            const unsigned& nplot) const
  {

   //Vector of local coordinates
   Vector<double> s(DIM);

   // Loop over plot points
   unsigned num_plot_points=nplot_points_paraview(nplot);
   for (unsigned iplot=0;iplot<num_plot_points;iplot++)
    {

     // Get local coordinates of plot point
     get_s_plot(iplot,nplot,s);
     std::complex<double> u(interpolated_u_pml_helmholtz(s));

     // Paraview need to ouput the fileds seperatly so it loops through all
     // the elements twice
     switch(i)
      {
       // Real part first
      case 0:
       file_out << u.real() << std::endl;
       break;

       // Imaginary part second
      case 1:
       file_out << u.imag() << std::endl;
       break;

       // Never get here
      default:
#ifdef PARANOID
       std::stringstream error_stream;
       error_stream
        << "PML Helmholtz elements only store 2 fields (real and imaginary)"
        << "so i must be 0 or 1 rather"
        << " than " << i << std::endl;
       throw OomphLibError(
        error_stream.str(),
        OOMPH_CURRENT_FUNCTION,
        OOMPH_EXCEPTION_LOCATION);
#endif
       break;

      }
    }
  }

 /// \short Name of the i-th scalar field. Default implementation
 /// returns V1 for the first one, V2 for the second etc. Can (should!) be
 /// overloaded with more meaningful names in specific elements.
 std::string scalar_name_paraview(const unsigned& i) const
  {
   switch(i)
    {
    case 0:
     return "Real part";
     break;

    case 1:
     return "Imaginary part";
     break;

     // Never get here
    default:
#ifdef PARANOID
     std::stringstream error_stream;
     error_stream
      << "PML Helmholtz elements only store 2 fields (real and imaginary)"
      << "so i must be 0 or 1 rather"
      << " than " << i << std::endl;
     throw OomphLibError(
      error_stream.str(),
      OOMPH_CURRENT_FUNCTION,
     OOMPH_EXCEPTION_LOCATION);
#endif

     // Dummy return for the default statement
     return " ";
     break;
    }
  }

 /// Output with default number of plot points
 void output(std::ostream &outfile)
  {
   const unsigned n_plot=5;
   output(outfile,n_plot);
  }

 /// \short Output FE representation of soln: x,y,u_re,u_im or
 /// x,y,z,u_re,u_im at  n_plot^DIM plot points
 void output(std::ostream &outfile, const unsigned &n_plot);


/// Output function for real part of full time-dependent solution
/// constructed by adding the scattered field
///  u = Re( (u_r +i u_i) exp(-i omega t)
/// at phase angle omega t = phi computed here, to the corresponding
/// incoming wave specified via the function pointer.
void  output_total_real(
 std::ostream &outfile,
 FiniteElement::SteadyExactSolutionFctPt incoming_wave_fct_pt,
 const double& phi,
 const unsigned &nplot);


 /// \short Output function for real part of full time-dependent solution
 /// u = Re( (u_r +i u_i) exp(-i omega t))
 /// at phase angle omega t = phi.
 /// x,y,u   or    x,y,z,u at n_plot plot points in each coordinate
 /// direction
 void output_real(std::ostream &outfile, const double& phi,
                  const unsigned &n_plot);

 /// \short Output function for imaginary part of full time-dependent solution
 /// u = Im( (u_r +i u_i) exp(-i omega t) )
 /// at phase angle omega t = phi.
 /// x,y,u   or    x,y,z,u at n_plot plot points in each coordinate
 /// direction
 void output_imag(std::ostream &outfile, const double& phi,
                  const unsigned &n_plot);

 /// C_style output with default number of plot points
 void output(FILE* file_pt)
  {
   const unsigned n_plot=5;
   output(file_pt,n_plot);
  }

 /// \short C-style output FE representation of soln: x,y,u_re,u_im or
 /// x,y,z,u_re,u_im at  n_plot^DIM plot points
 void output(FILE* file_pt, const unsigned &n_plot);

 /// Output exact soln: x,y,u_re_exact,u_im_exact
 /// or x,y,z,u_re_exact,u_im_exact at n_plot^DIM plot points
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
  "There is no time-dependent output_fct() for PmlHelmholtz elements ",
  OOMPH_CURRENT_FUNCTION,
    OOMPH_EXCEPTION_LOCATION);
  }



 /// \short Output function for real part of full time-dependent fct
 /// u = Re( (u_r +i u_i) exp(-i omega t)
 /// at phase angle omega t = phi.
 /// x,y,u   or    x,y,z,u at n_plot plot points in each coordinate
 /// direction
 void output_real_fct(std::ostream &outfile,
                      const double& phi,
                      const unsigned &n_plot,
                      FiniteElement::SteadyExactSolutionFctPt exact_soln_pt);

 /// \short Output function for imaginary part of full time-dependent fct
 /// u = Im( (u_r +i u_i) exp(-i omega t))
 /// at phase angle omega t = phi.
 /// x,y,u   or    x,y,z,u at n_plot plot points in each coordinate
 /// direction
 void output_imag_fct(std::ostream &outfile,
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
"There is no time-dependent compute_error() for PmlHelmholtz elements",
OOMPH_CURRENT_FUNCTION,
    OOMPH_EXCEPTION_LOCATION);
  }


 /// Compute norm of fe solution
 void compute_norm(double& norm);

 /// Access function: Pointer to source function
 PmlHelmholtzSourceFctPt& source_fct_pt() {return Source_fct_pt;}

 /// Access function: Pointer to source function. Const version
 PmlHelmholtzSourceFctPt source_fct_pt() const {return Source_fct_pt;}


 /// Get source term at (Eulerian) position x. This function is
 /// virtual to allow overloading in multi-physics problems where
 /// the strength of the source function might be determined by
 /// another system of equations.
 inline virtual void get_source_helmholtz(const unsigned& ipt,
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

     values_to_pin[0]=0;
     values_to_pin[1]=1;
   }


 /// Get flux: flux[i] = du/dx_i for real and imag part
 void get_flux(const Vector<double>& s,
               Vector<std::complex <double> >& flux) const
  {
   //Find out how many nodes there are in the element
   const unsigned n_node = nnode();


   //Set up memory for the shape and test functions
   Shape psi(n_node);
   DShape dpsidx(n_node,DIM);

   //Call the derivatives of the shape and test functions
   dshape_eulerian(s,psi,dpsidx);

   //Initialise to zero
   const std::complex<double> zero(0.0,0.0);
   for(unsigned j=0;j<DIM;j++)
    {
     flux[j] = zero;
    }

   // Loop over nodes
   for(unsigned l=0;l<n_node;l++)
    {
     //Cache the complex value of the unknown
     const std::complex<double> u_value(
      this->nodal_value(l,u_index_helmholtz().real()),
      this->nodal_value(l,u_index_helmholtz().imag()));
     //Loop over derivative directions
     for(unsigned j=0;j<DIM;j++)
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
   fill_in_generic_residual_contribution_helmholtz(
    residuals,GeneralisedElement::Dummy_matrix,0);
  }


 /// \short Add the element's contribution to its residual vector and
 /// element Jacobian matrix (wrapper)
 void fill_in_contribution_to_jacobian(Vector<double> &residuals,
                                   DenseMatrix<double> &jacobian)
  {
   //Call the generic routine with the flag set to 1
    fill_in_generic_residual_contribution_helmholtz(residuals,jacobian,1);
  }



 /// \short Return FE representation of function value u_helmholtz(s)
 /// at local coordinate s
 inline std::complex<double>
  interpolated_u_pml_helmholtz(const Vector<double> &s) const
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
   const unsigned u_nodal_index_real = u_index_helmholtz().real();
   const unsigned u_nodal_index_imag = u_index_helmholtz().imag();

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


/// \short Compute pml coefficients at position x and integration point ipt.
/// pml_laplace_factor is used in the residual contribution from the laplace
/// operator, similarly pml_k_squared_factor is used in the contribution from
/// the k^2 of the Helmholtz operator.
 void compute_pml_coefficients(
  const unsigned& ipt,
  const Vector<double>& x,
  Vector< std::complex<double> >& pml_laplace_factor,
  std::complex<double>& pml_k_squared_factor)
 {

    /// Vector which points from the inner boundary to x
    Vector<double> nu(DIM);
    for(unsigned k=0; k<DIM; k++)
    {
      nu[k] = x[k] - this->Pml_inner_boundary[k];
    }

    /// Vector which points from the inner boundary to the edge of the boundary
    Vector<double> pml_width(DIM);
    for(unsigned k=0; k<DIM; k++)
    {
      pml_width[k] = this->Pml_outer_boundary[k] - this->Pml_inner_boundary[k];
    }

    // Declare gamma_i vectors of complex numbers for PML weights
    Vector<std::complex<double> > pml_gamma(DIM);

    if (this->Pml_is_enabled)
    {
      // Cache k_squared to pass into mapping function
      double k_squared_local = k_squared();

      for(unsigned k=0; k<DIM; k++) {
        // If PML is enabled in the respective direction
        if (this->Pml_direction_active[k])
        {
          pml_gamma[k] = Pml_mapping_pt->gamma(nu[k], pml_width[k],
            k_squared_local);
        }
        else
        {
          pml_gamma[k] = 1.0;
        }
      }

     /// \short  for 2D, in order:
     /// g_y/g_x, g_x/g_y for Laplace bit and g_x*g_y for Helmholtz bit
     /// for 3D, in order: g_y*g_x/g_x, g*x*g_z/g_y, g_x*g_y/g_z for Laplace bit
     /// and g_x*g_y*g_z for Helmholtz bit
     if (DIM == 2)
      {
       pml_laplace_factor[0] = pml_gamma[1] / pml_gamma[0];
       pml_laplace_factor[1] = pml_gamma[0] / pml_gamma[1];
       pml_k_squared_factor = pml_gamma[0] * pml_gamma[1];
      }
     else // if (DIM == 3)
      {
       pml_laplace_factor[0] = pml_gamma[1] * pml_gamma[2] / pml_gamma[0];
       pml_laplace_factor[1] = pml_gamma[0] * pml_gamma[2] / pml_gamma[1];
       pml_laplace_factor[2] = pml_gamma[0] * pml_gamma[1] / pml_gamma[2];
       pml_k_squared_factor = pml_gamma[0] * pml_gamma[1] * pml_gamma[2];
      }

    }
    else
     {
      /// \short The weights all default to 1.0 as if the propagation
      /// medium is the physical domain
      for(unsigned k=0; k<DIM; k++)
      {
        pml_laplace_factor[k] = std::complex<double> (1.0,0.0);
      }

      pml_k_squared_factor = std::complex<double> (1.0,0.0);
     }

 }

 /// Return a pointer to the PML Mapping object
 PMLMapping* &pml_mapping_pt() {return Pml_mapping_pt;}

 /// Return a pointer to the PML Mapping object (const version)
 PMLMapping* const &pml_mapping_pt() const {return Pml_mapping_pt;}

 /// Static so that the class doesn't need to instantiate a new default
 /// everytime it uses it
 static BermudezPMLMapping Default_pml_mapping;

 /// \short The number of "DOF types" that degrees of freedom in this element
 /// are sub-divided into: real and imaginary part
 unsigned ndof_types() const
  {
   return 2;
  }
 
 /// \short Create a list of pairs for all unknowns in this element,
 /// so that the first entry in each pair contains the global equation
 /// number of the unknown, while the second one contains the number
 /// of the "DOF type" that this unknown is associated with.
 /// (Function can obviously only be called if the equation numbering
 /// scheme has been set up.) Real=0; Imag=1
 void get_dof_numbers_for_unknowns(
  std::list<std::pair<unsigned long,unsigned> >& dof_lookup_list) const
 {
  // temporary pair (used to store dof lookup prior to being added to list)
  std::pair<unsigned,unsigned> dof_lookup;
 
  // number of nodes
  unsigned n_node = this->nnode();
  
  // loop over the nodes
  for (unsigned n = 0; n < n_node; n++)
   {
    // determine local eqn number for real part
    int local_eqn_number = 
     this->nodal_local_eqn(n, u_index_helmholtz().real());
    
    // ignore pinned values
    if (local_eqn_number >= 0)
     {
      // store dof lookup in temporary pair: First entry in pair
      // is global equation number; second entry is dof type
      dof_lookup.first = this->eqn_number(local_eqn_number);
      dof_lookup.second = 0;
      
      // add to list
      dof_lookup_list.push_front(dof_lookup);
     }

    // determine local eqn number for imag part
    local_eqn_number = 
     this->nodal_local_eqn(n, u_index_helmholtz().imag());
    
    // ignore pinned values
    if (local_eqn_number >= 0)
     {
      // store dof lookup in temporary pair: First entry in pair
      // is global equation number; second entry is dof type
      dof_lookup.first = this->eqn_number(local_eqn_number);
      dof_lookup.second = 1;
      
      // add to list
      dof_lookup_list.push_front(dof_lookup);
     }
   }
 }


protected:

 /// \short Shape/test functions and derivs w.r.t. to global coords at
 /// local coord. s; return  Jacobian of mapping
 virtual double dshape_and_dtest_eulerian_helmholtz(const Vector<double> &s,
                                                  Shape &psi,
                                                  DShape &dpsidx, Shape &test,
                                                  DShape &dtestdx) const=0;


 /// \short Shape/test functions and derivs w.r.t. to global coords at
 /// integration point ipt; return  Jacobian of mapping
 virtual double dshape_and_dtest_eulerian_at_knot_helmholtz(
  const unsigned &ipt,
  Shape &psi,
  DShape &dpsidx,
  Shape &test,
  DShape &dtestdx) const=0;

 /// \short Compute element residual Vector only (if flag=and/or element
 /// Jacobian matrix
 virtual void fill_in_generic_residual_contribution_helmholtz(
  Vector<double> &residuals, DenseMatrix<double> &jacobian,
  const unsigned& flag);

  /// Pointer to wavenumber complex shift
double *Alpha_pt;

 /// Pointer to source function:
 PmlHelmholtzSourceFctPt Source_fct_pt;

 /// Pointer to wave number (must be set!)
 double* K_squared_pt;

 /// Pointer to class which holds the pml mapping function, also known as gamma
 PMLMapping* Pml_mapping_pt;
 
 /// Static default value for the physical constants (initialised to zero)
 static double Default_Physical_Constant_Value;

};


///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////



//======================================================================
/// \short QPmlHelmholtzElement elements are linear/quadrilateral/
/// brick-shaped PmlHelmholtz elements with isoparametric
/// interpolation for the function.
//======================================================================
template <unsigned DIM, unsigned NNODE_1D>
 class QPmlHelmholtzElement : public virtual QElement<DIM,NNODE_1D>,
 public virtual PmlHelmholtzEquations<DIM>
{

private:

 /// \short Static int that holds the number of variables at
 /// nodes: always the same
 static const unsigned Initial_Nvalue;

  public:


 ///\short  Constructor: Call constructors for QElement and
 /// PmlHelmholtz equations
 QPmlHelmholtzElement() :
  QElement<DIM,NNODE_1D>(), PmlHelmholtzEquations<DIM>()
  {}

 /// Broken copy constructor
 QPmlHelmholtzElement
  (const QPmlHelmholtzElement<DIM,NNODE_1D>& dummy)
  {
   BrokenCopy::broken_copy("QPmlHelmholtzElement");
  }

 /// Broken assignment operator
 void operator=(const QPmlHelmholtzElement<DIM,NNODE_1D>&)
  {
   BrokenCopy::broken_assign("QPmlHelmholtzElement");
  }


 /// \short  Required  # of `values' (pinned or dofs)
 /// at node n
 inline unsigned required_nvalue(const unsigned &n) const
  {return Initial_Nvalue;}

 /// \short Output function:
 ///  x,y,u   or    x,y,z,u
 void output(std::ostream &outfile)
  {PmlHelmholtzEquations<DIM>::output(outfile);}


 ///  \short Output function:
 ///   x,y,u   or    x,y,z,u at n_plot^DIM plot points
 void output(std::ostream &outfile, const unsigned &n_plot)
  {PmlHelmholtzEquations<DIM>::output(outfile,n_plot);}

 /// \short Output function for real part of full time-dependent solution
 /// u = Re( (u_r +i u_i) exp(-i omega t)
 /// at phase angle omega t = phi.
 /// x,y,u   or    x,y,z,u at n_plot plot points in each coordinate
 /// direction
 void output_real(std::ostream &outfile, const double& phi,
                  const unsigned &n_plot)
 {PmlHelmholtzEquations<DIM>::output_real(outfile,phi,n_plot);}

 /// \short Output function for imaginary part of full time-dependent solution
 /// u = Im( (u_r +i u_i) exp(-i omega t))
 /// at phase angle omega t = phi.
 /// x,y,u   or    x,y,z,u at n_plot plot points in each coordinate
 /// direction
 void output_imag(std::ostream &outfile, const double& phi,
                  const unsigned &n_plot)
 {PmlHelmholtzEquations<DIM>::output_imag(outfile,phi,n_plot);}


 /// \short C-style output function:
 ///  x,y,u   or    x,y,z,u
 void output(FILE* file_pt)
  {PmlHelmholtzEquations<DIM>::output(file_pt);}


 ///  \short C-style output function:
 ///   x,y,u   or    x,y,z,u at n_plot^DIM plot points
 void output(FILE* file_pt, const unsigned &n_plot)
  {PmlHelmholtzEquations<DIM>::output(file_pt,n_plot);}


 /// \short Output function for an exact solution:
 ///  x,y,u_exact   or    x,y,z,u_exact at n_plot^DIM plot points
 void output_fct(std::ostream &outfile, const unsigned &n_plot,
                 FiniteElement::SteadyExactSolutionFctPt exact_soln_pt)
  {PmlHelmholtzEquations<DIM>::output_fct(outfile,
                                                  n_plot,
                                                  exact_soln_pt);}


 /// \short Output function for real part of full time-dependent fct
 /// u = Re( (u_r +i u_i) exp(-i omega t)
 /// at phase angle omega t = phi.
 /// x,y,u   or    x,y,z,u at n_plot plot points in each coordinate
 /// direction
 void output_real_fct(std::ostream &outfile,
                      const double& phi,
                      const unsigned &n_plot,
                      FiniteElement::SteadyExactSolutionFctPt exact_soln_pt)
 {
  PmlHelmholtzEquations<DIM>::output_real_fct(outfile,
                                                      phi,
                                                      n_plot,
                                                      exact_soln_pt);
 }

 /// \short Output function for imaginary part of full time-dependent fct
 /// u = Im( (u_r +i u_i) exp(-i omega t))
 /// at phase angle omega t = phi.
 /// x,y,u   or    x,y,z,u at n_plot plot points in each coordinate
 /// direction
 void output_imag_fct(std::ostream &outfile,
                      const double& phi,
                      const unsigned &n_plot,
                      FiniteElement::SteadyExactSolutionFctPt exact_soln_pt)
 {
  PmlHelmholtzEquations<DIM>::output_imag_fct(outfile,
                                                      phi,
                                                      n_plot,
                                                      exact_soln_pt);
 }


 /// \short Output function for a time-dependent exact solution.
 ///  x,y,u_exact   or    x,y,z,u_exact at n_plot^DIM plot points
 /// (Calls the steady version)
 void output_fct(std::ostream &outfile, const unsigned &n_plot,
                 const double& time,
                 FiniteElement::UnsteadyExactSolutionFctPt exact_soln_pt)
  {PmlHelmholtzEquations<DIM>::output_fct(outfile,
                                                  n_plot,
                                                  time,
                                                  exact_soln_pt);}

protected:

/// Shape, test functions & derivs. w.r.t. to global coords. Return Jacobian.
 inline double dshape_and_dtest_eulerian_helmholtz(
  const Vector<double> &s, Shape &psi, DShape &dpsidx,
  Shape &test, DShape &dtestdx) const;


 /// \short Shape, test functions & derivs. w.r.t. to global coords. at
 /// integration point ipt. Return Jacobian.
 inline double dshape_and_dtest_eulerian_at_knot_helmholtz(const unsigned& ipt,
                                                         Shape &psi,
                                                         DShape &dpsidx,
                                                         Shape &test,
                                                         DShape &dtestdx)
  const;

};




//Inline functions:


//======================================================================
/// Define the shape functions and test functions and derivatives
/// w.r.t. global coordinates and return Jacobian of mapping.
///
/// Galerkin: Test functions = shape functions
//======================================================================
template<unsigned DIM, unsigned NNODE_1D>
 double QPmlHelmholtzElement<DIM,NNODE_1D>::dshape_and_dtest_eulerian_helmholtz(
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
template<unsigned DIM, unsigned NNODE_1D>
double QPmlHelmholtzElement<DIM,NNODE_1D>::
 dshape_and_dtest_eulerian_at_knot_helmholtz(
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
/// Face geometry for the QPmlHelmholtzElement elements: The spatial
/// dimension of the face elements is one lower than that of the
/// bulk element but they have the same number of points
/// along their 1D edges.
//=======================================================================
template<unsigned DIM, unsigned NNODE_1D>
class FaceGeometry<QPmlHelmholtzElement<DIM,NNODE_1D> >:
 public virtual QElement<DIM-1,NNODE_1D>
{

  public:

 /// \short Constructor: Call the constructor for the
 /// appropriate lower-dimensional QElement
 FaceGeometry() : QElement<DIM-1,NNODE_1D>() {}

};

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


//=======================================================================
/// Face geometry for the 1D QPmlHelmholtzElement elements:
/// Point elements
//=======================================================================
template<unsigned NNODE_1D>
class FaceGeometry<QPmlHelmholtzElement<1,NNODE_1D> >:
 public virtual PointElement
{

  public:

 /// \short Constructor: Call the constructor for the
 /// appropriate lower-dimensional QElement
 FaceGeometry() : PointElement() {}

};


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


//==========================================================
/// PmlHelmholtz upgraded to become projectable
//==========================================================
 template<class HELMHOLTZ_ELEMENT>
 class ProjectablePmlHelmholtzElement :
  public virtual ProjectableElement<HELMHOLTZ_ELEMENT>
 {

 public:


  /// \short Constructor [this was only required explicitly
  /// from gcc 4.5.2 onwards...]
  ProjectablePmlHelmholtzElement(){}

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
       << "PmlHelmholtz elements only store 2 fields so fld = "
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
       << "PmlHelmholtz elements only store two fields so fld = "
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
       << "PmlHelmholtz elements only store two fields so fld = "
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
    double J=this->dshape_and_dtest_eulerian_helmholtz(s,psi,dpsidx,
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
       << "PmlHelmholtz elements only store two fields so fld = "
       << fld << " is illegal\n";
      throw OomphLibError(
       error_stream.str(),
       OOMPH_CURRENT_FUNCTION,
       OOMPH_EXCEPTION_LOCATION);
     }
#endif
    //Find the index at which the variable is stored
    std::complex<unsigned> complex_u_nodal_index = this->u_index_helmholtz();
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
       << "PmlHelmholtz elements only store two fields so fld = "
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
       << "PmlHelmholtz elements only store two fields so fld = "
       << fld << " is illegal\n";
      throw OomphLibError(
       error_stream.str(),
       OOMPH_CURRENT_FUNCTION,
       OOMPH_EXCEPTION_LOCATION);
     }
#endif
    std::complex<unsigned> complex_u_nodal_index = this->u_index_helmholtz();
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
   HELMHOLTZ_ELEMENT::output(outfile,nplot);
  }


 };


//=======================================================================
/// Face geometry for element is the same as that for the underlying
/// wrapped element
//=======================================================================
 template<class ELEMENT>
 class FaceGeometry<ProjectablePmlHelmholtzElement<ELEMENT> >
  : public virtual FaceGeometry<ELEMENT>
 {
 public:
  FaceGeometry() : FaceGeometry<ELEMENT>() {}
 };

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

//=======================================================================
/// Policy class defining the elements to be used in the actual
/// PML layers. Same!
//=======================================================================
  template<unsigned NNODE_1D>
class PMLLayerElement<
 QPmlHelmholtzElement<2,NNODE_1D> > :
 public virtual QPmlHelmholtzElement<2,NNODE_1D>
{

  public:

 /// \short Constructor: Call the constructor for the
 /// appropriate QElement
 PMLLayerElement() :
  QPmlHelmholtzElement<2,NNODE_1D>()
  {}

};

} // End of Namespace oomph

#endif
