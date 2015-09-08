///LIC// ====================================================================
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
//Header file for specific (two-dimensional) fluid free surface elements

//Include guards, to prevent multiple includes
#ifndef OOMPH_SURFACTANT_TRANSPORT_ELEMENTS_HEADER
#define OOMPH_SURFACTANT_TRANSPORT_ELEMENTS_HEADER


// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

//OOMPH-LIB headers
#include "specific_node_update_interface_elements.h"

namespace oomph
{

//==================================================================
/// Generic surfactant transport equations implemented
/// independently of the dimension and then specialised using
/// the generic mechanisms introduce in the FluidInterfaceElements
//=================================================================
class SurfactantTransportInterfaceElement : 
  public FluidInterfaceElement
{
private:
 /// Pointer to an Elasticity number
  double *Beta_pt;

 /// Pointer to Surface Peclet number
  double *Peclet_S_pt;

 /// Pointer to the surface Peclect Strouhal number
  double *Peclet_Strouhal_S_pt;

 /// Index at which the surfactant concentration is stored at the
 /// nodes
 Vector<unsigned> C_index;
 
 /// Default value of the physical constants
 static double Default_Physical_Constant_Value;

protected:
 
 ///Get the surfactant concentration
 double interpolated_C(const Vector<double> &s);
 
 /// The time derivative of the surface concentration
 double dcdt_surface(const unsigned &l) const;

 /// The surface tension function is linear in the
 /// concentration with constant of proportionality equal
 /// to the elasticity  number.
 double sigma(const Vector<double> &s);

 ///Return the derivative of sigma with respect to C
 ///For use in computing the Jacobian
 double dsigma_dC(const Vector<double> &s)
 {return -this->beta();}
  
  /// \short Overload the Helper function to calculate the residuals and 
  /// jacobian entries. This particular function ensures that the
  /// additional entries are calculated inside the integration loop
  void add_additional_residual_contributions_interface(
   Vector<double> &residuals, DenseMatrix<double> &jacobian,
   const unsigned &flag,const Shape &psif, const DShape &dpsifds,
   const DShape &dpsifdS, const DShape &dpsifdS_div,
   const Vector<double> &s,
   const Vector<double> &interpolated_x, const Vector<double> &interpolated_n, 
   const double &W,const double &J);

 
  /// Add the element's contribution to its residuals vector,
  /// jacobian matrix and mass matrix
  void fill_in_contribution_to_jacobian_and_mass_matrix(
   Vector<double> &residuals, DenseMatrix<double> &jacobian, 
   DenseMatrix<double> &mass_matrix)
  {
   //Add the contribution to the jacobian
   this->fill_in_contribution_to_jacobian(residuals,jacobian);
   //No mass matrix terms, but should probably do kinematic bit here
  }
  
  public:

  ///Constructor that passes the bulk element and face index down
  ///to the underlying
  SurfactantTransportInterfaceElement() : FluidInterfaceElement()
   {
    //Initialise the values
    Beta_pt = &Default_Physical_Constant_Value;
    Peclet_S_pt = &Default_Physical_Constant_Value;
    Peclet_Strouhal_S_pt = &Default_Physical_Constant_Value;
   }
  
  //Set the c_index
  inline void set_c_index(const Vector<unsigned> &c_index) {this->C_index = c_index;}
  
  ///Return the Elasticity number
  double beta() {return *Beta_pt;}
  
  ///Return the surface peclect number
  double peclet_s() {return *Peclet_S_pt;}
  
  ///Return the surface peclect strouhal number
  double peclet_strouhal_s() {return *Peclet_Strouhal_S_pt;}
  
  ///Access function for pointer to the Elasticity number
  double* &beta_pt() {return Beta_pt;}
  
  ///Access function for pointer to the surface Peclet number
  double* &peclet_s_pt() {return Peclet_S_pt;}
  
  ///Access function for pointer to the surface Peclet x Strouhal number
  double* &peclet_strouhal_s_pt() {return Peclet_Strouhal_S_pt;}
  
  
  ///Overload the output function
  void output(std::ostream &outfile) {FiniteElement::output(outfile);}
   
  void output(std::ostream &outfile, const unsigned &n_plot);
  
  ///Overload the C-style output function
  void output(FILE* file_pt) {FiniteElement::output(file_pt);}
  
  ///C-style Output function
  void output(FILE* file_pt, const unsigned &n_plot)
  {FiniteElement::output(file_pt,n_plot);}
 
  /// \short Compute the concentration intergated over the surface area
  double integrate_c();

};


///=============================================================================
///This is the policy class for the surfactanttransport equations which require
///one additional value for the surface concentration
//=============================================================================
template<>
 class FluidInterfaceAdditionalValues<SurfactantTransportInterfaceElement>
 {
   public:
  FluidInterfaceAdditionalValues<SurfactantTransportInterfaceElement>() {}

  inline unsigned nadditional_values(const unsigned &n) {return 1;}

  inline void setup_equation_indices(
   SurfactantTransportInterfaceElement* const &element_pt,
   const unsigned &id)
  {
   const unsigned n_node = element_pt->nnode();
   Vector<unsigned> c_index(n_node);
   for(unsigned n=0;n<n_node;n++)
    {
     c_index[n] =
      dynamic_cast<BoundaryNodeBase*>(element_pt->node_pt(n))
      ->index_of_first_value_assigned_by_face_element(id);
    }
   
   element_pt->set_c_index(c_index);
  }
  
 };


//-------------------------------GEOMETRIC SPECIALISATIONS----------------

///Specialise to the Line geometry
template<class ELEMENT>
 class SpineLineSurfactantTransportInterfaceElement :
 public SpineUpdateFluidInterfaceElement<SurfactantTransportInterfaceElement,
 LineDerivatives,ELEMENT>
 {
   public:
  
   SpineLineSurfactantTransportInterfaceElement(
  FiniteElement* const &element_pt, 
  const int &face_index) : 
  SpineUpdateFluidInterfaceElement<SurfactantTransportInterfaceElement,
   LineDerivatives,ELEMENT>(element_pt,face_index) {}
 };


//Define the bounding element type for the line elements 
template<class ELEMENT>
 class BoundingElementType<SpineUpdateFluidInterfaceElement<
 SurfactantTransportInterfaceElement,LineDerivatives,ELEMENT> >:
 public SpinePointFluidInterfaceBoundingElement<ELEMENT>
 {
   public:
  
  BoundingElementType<SpineUpdateFluidInterfaceElement<
   SurfactantTransportInterfaceElement,LineDerivatives,ELEMENT> >() :
  SpinePointFluidInterfaceBoundingElement<ELEMENT>() { }
 };


///Specialise to the Axisymmetric geometry
template<class ELEMENT>
 class SpineAxisymmetricSurfactantTransportInterfaceElement :
 public SpineUpdateFluidInterfaceElement<SurfactantTransportInterfaceElement,
 AxisymmetricDerivatives,ELEMENT>
 {
   public:
  
   SpineAxisymmetricSurfactantTransportInterfaceElement(
  FiniteElement* const &element_pt, 
  const int &face_index) : 
  SpineUpdateFluidInterfaceElement<SurfactantTransportInterfaceElement,
   AxisymmetricDerivatives,ELEMENT>(element_pt,face_index) {}
 };


//Define the bounding element type for the axisymmetric surface 
template<class ELEMENT>
 class BoundingElementType<SpineUpdateFluidInterfaceElement<
 SurfactantTransportInterfaceElement,AxisymmetricDerivatives,ELEMENT> >:
 public SpinePointFluidInterfaceBoundingElement<ELEMENT>
 {
   public:
  
  BoundingElementType<SpineUpdateFluidInterfaceElement<
   SurfactantTransportInterfaceElement,AxisymmetricDerivatives,ELEMENT> >() :
  SpinePointFluidInterfaceBoundingElement<ELEMENT>() { }
 };

///Specialise to the Axisymmetric geometry
template<class ELEMENT>
 class ElasticAxisymmetricSurfactantTransportInterfaceElement :
 public ElasticUpdateFluidInterfaceElement<SurfactantTransportInterfaceElement,
 AxisymmetricDerivatives,ELEMENT>
 {
   public:
  
   ElasticAxisymmetricSurfactantTransportInterfaceElement(
  FiniteElement* const &element_pt, 
  const int &face_index) : 
  ElasticUpdateFluidInterfaceElement<SurfactantTransportInterfaceElement,
   AxisymmetricDerivatives,ELEMENT>(element_pt,face_index) {}
 };


//Define the bounding element type for the axisymmetric surface 
template<class ELEMENT>
 class BoundingElementType<ElasticUpdateFluidInterfaceElement<
 SurfactantTransportInterfaceElement,AxisymmetricDerivatives,ELEMENT> >:
 public ElasticPointFluidInterfaceBoundingElement<ELEMENT>
 {
   public:
  
  BoundingElementType<ElasticUpdateFluidInterfaceElement<
   SurfactantTransportInterfaceElement,AxisymmetricDerivatives,ELEMENT> >() :
  ElasticPointFluidInterfaceBoundingElement<ELEMENT>() { }
 };


 ///Specialise to surface geometry
 template<class ELEMENT>
  class SpineSurfaceSurfactantTransportInterfaceElement :
  public SpineUpdateFluidInterfaceElement<SurfactantTransportInterfaceElement,
  SurfaceDerivatives,ELEMENT>
  {
    public:
   
    SpineSurfaceSurfactantTransportInterfaceElement(
     FiniteElement* const &element_pt, 
     const int &face_index) : 
   SpineUpdateFluidInterfaceElement<SurfactantTransportInterfaceElement,
    SurfaceDerivatives,ELEMENT>(element_pt,face_index) {}
  };
 
 
//Define the bounding element type for the 2D surface 
 template<class ELEMENT>
  class BoundingElementType<SpineUpdateFluidInterfaceElement<
  SurfactantTransportInterfaceElement,SurfaceDerivatives,ELEMENT> >:
 public SpineLineFluidInterfaceBoundingElement<ELEMENT>
 {
   public:
  
  BoundingElementType<SpineUpdateFluidInterfaceElement<
   SurfactantTransportInterfaceElement,SurfaceDerivatives,ELEMENT> >() :
  SpineLineFluidInterfaceBoundingElement<ELEMENT>() { }
 };


}

#endif






