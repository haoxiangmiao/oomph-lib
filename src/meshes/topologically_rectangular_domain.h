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
//Include guards
#ifndef OOMPH_TOPOLOGICALLY_RECTANGULAR_DOMAIN_HEADER
#define OOMPH_TOPOLOGICALLY_RECTANGULAR_DOMAIN_HEADER


// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif


//oomph-lib headers
#include "../generic/domain.h"


namespace oomph
{

//=============================================================================
/// \short Topologically Rectangular Domain - a domain dexcribing a 
/// topologically rectangular problem - primarily contains functions to access 
/// the position of the global boundary relative to the macro element boundary,
/// as well as first and second derivates of the global boundary wrt the macro
/// element boundary 
/// NOTE : suitable for HermiteElementQuadMesh
//=============================================================================
class TopologicallyRectangularDomain : public Domain
{

public:


 /// \short boundary function pointer - for a given boundary takes the 
 /// macro element coordinate position on that boundary and for that position 
 /// returns the global coordinates (x) coordinates, or derivatives - dx_i/dm_t
 /// or second derivatives d2x_i/dm_t^2
 typedef void (*BoundaryFctPt)(const double& s, Vector<double>& r);
       

 /// \short Constructor - domain boundaries are described with four boundary
 /// function pointers describing the topology of the north, east, south, and
 /// west boundaries
 TopologicallyRectangularDomain(BoundaryFctPt north_pt, BoundaryFctPt east_pt,
                                BoundaryFctPt south_pt, BoundaryFctPt west_pt);


 /// \short Constructor - takes length of domain in x and y direction as 
 /// arguements. Assumes domain is rectangular, and the south west (lower 
 /// left) corner is at 0,0.
 TopologicallyRectangularDomain(const double& l_x, const double& l_y);


 /// \short Constructor - takes the minimum and maximum coordinates of the 
 /// of an assumed rectanguler domain in the x and y direction
 TopologicallyRectangularDomain(const double& x_min, const double& x_max,
                                const double& y_min, const double& y_max);

 
 /// Broken copy constructor
 TopologicallyRectangularDomain(const TopologicallyRectangularDomain&) 
  { 
   BrokenCopy::broken_copy("TopologicallyRectangularDomain");
  } 
 

 /// Broken assignment operator
 void operator=(const TopologicallyRectangularDomain&) 
  {
   BrokenCopy::broken_assign("TopologicallyRectangularDomain");
  }


 /// Destructor - deletes the underlying macro element
 ~TopologicallyRectangularDomain()
  {
   delete Macro_element_pt[0];
   Macro_element_pt[0] = 0;
  }


 /// \short allows the boundary derivate function pointers to be set. To 
 /// compute the derivatives of the problem domain global coordinates (x_i) wrt
 /// the macro element coordinates (m_i), dx_i/dm_t is required along the 
 /// domain boundaries (where dm_t is the macro element coordinate tangential 
 /// to the domain boundary). The derivatives dx_i/dm_t can either be 
 /// prescribed with function pointers, or if the function pointers are not 
 /// provided then dx_i/dm_t is computed with finite differencing.
 /// Note - these functions are only required for domains contructed with 
 /// boundary function pointers
 void set_boundary_derivative_functions(BoundaryFctPt d_north_pt, 
                                        BoundaryFctPt d_east_pt,
                                        BoundaryFctPt d_south_pt, 
                                        BoundaryFctPt d_west_pt);


 /// \short allows the boundary second derivate function pointers to be set. 
 /// To compute the second derivatives of the problem domain global 
 /// coordinates (x_i) wrt the macro element coordinates (m_i), d2x_i/dm_t^2 
 /// is required along the domain boundaries (where dm_t is the macro element 
 /// coordinate tangential to the domain boundary). The derivatives 
 /// d2x_i/dm_t^2 can either be prescribed with function pointers, or if the 
 /// function pointers are not provided then dx_i/dm_t is computed with finite 
 /// differencing.
 /// Note - these functions are only required for domains contructed with 
 /// boundary function pointers
 void set_boundary_second_derivative_functions(BoundaryFctPt d2_north_pt, 
                                               BoundaryFctPt d2_east_pt,
                                               BoundaryFctPt d2_south_pt, 
                                               BoundaryFctPt d2_west_pt);


 /// returns the global coordinate position (f) of macro element position s 
 /// on boundary i_direct (e.g. N/S/W/E in 2D) at time t (no time dependence)
 void macro_element_boundary(const unsigned& t,
                             const unsigned& i_macro, 
                             const unsigned& i_direct,
                             const Vector<double>& s,
                             Vector<double>& f);

   
 /// returns the derivates of the global coordinate position (f) wrt to the 
 /// macro element coordinate at macro macro element position s on boundary 
 /// i_direct (e.g. N/S/W/E in 2D) at time t (no time dependence)
 void dmacro_element_boundary(const unsigned& t,
                              const unsigned& i_macro, 
                              const unsigned& i_direct,
                              const Vector<double>& s,
                              Vector<double>& f);


 /// returns the second derivates of the global coordinate position (f) wrt to 
 /// the macro element coordinate at macro macro element position s on boundary
 /// i_direct (e.g. N/S/W/E in 2D) at time t (no time dependence)
 void d2macro_element_boundary(const unsigned& t,
                               const unsigned& i_macro, 
                               const unsigned& i_direct,
                               const Vector<double>& s,
                               Vector<double>& f);


private:
 
 /// \short Function pointer to prescribe the north boundary of this 
 /// topologically rectangular domain
 BoundaryFctPt North_boundary_fn_pt;

 /// \short Function pointer to prescribe the east boundary of this 
 /// topologically rectangular domain  
 BoundaryFctPt East_boundary_fn_pt;

 /// \short Function pointer to prescribe the north boundary of this 
 /// topologically rectangular domain
 BoundaryFctPt South_boundary_fn_pt;

 /// \short Function pointer to prescribe the west boundary of this 
 /// topologically rectangular domain
 BoundaryFctPt West_boundary_fn_pt;


 /// \short Function pointer to prescribe the derivates of global coordinates 
 /// wrt to the macro element coordinate tangential to the north boundary
 BoundaryFctPt dNorth_boundary_fn_pt;
  
 /// \short Function pointer to prescribe the derivates of global coordinates 
 /// wrt to the macro element coordinate tangential to the east boundary
 BoundaryFctPt dEast_boundary_fn_pt;

 /// \short Function pointer to prescribe the derivates of global coordinates 
 /// wrt to the macro element coordinate tangential to the south boundary
 BoundaryFctPt dSouth_boundary_fn_pt;

 /// \short Function pointer to prescribe the derivates of global coordinates 
 /// wrt to the macro element coordinate tangential to the west boundary
 BoundaryFctPt dWest_boundary_fn_pt;


 /// \short Function pointer to prescribe the second derivates of global 
 /// coordinates wrt to the macro element coordinate tangential to the north
 /// boundary
 BoundaryFctPt d2North_boundary_fn_pt;
  
 /// \short Function pointer to prescribe the second derivates of global 
 /// coordinates wrt to the macro element coordinate tangential to the east
 /// boundary
 BoundaryFctPt d2East_boundary_fn_pt;

 /// \short Function pointer to prescribe the second derivates of global 
 /// coordinates wrt to the macro element coordinate tangential to the south 
 /// boundary
 BoundaryFctPt d2South_boundary_fn_pt;

 /// \short Function pointer to prescribe the second derivates of global 
 /// coordinates wrt to the macro element coordinate tangential to the west 
 /// boundary
 BoundaryFctPt d2West_boundary_fn_pt;


 /// \short coordinate position of south west corner of domain (only used if
 /// boundary functions are not used)
 Vector<double> x_south_west;

 /// \short coordinate position of north east corner of domain (only used if
 /// boundary functions are not used)
 Vector<double> x_north_east;


 /// \short takes the macro element coordinate position along the north 
 /// boundary and returns the global coordinate position along that boundary
 void r_N(const Vector<double>& s, Vector<double>& f);

 /// \short takes the macro element coordinate position along the east
 /// boundary and returns the global coordinate position along that boundary
 void r_E(const Vector<double>& s, Vector<double>& f);

 /// \short takes the macro element coordinate position along the south
 /// boundary and returns the global coordinate position along that boundary
 void r_S(const Vector<double>& s, Vector<double>& f);

 /// \short takes the macro element coordinate position along the west
 /// boundary and returns the global coordinate position along that boundary
 /// access down boundary function pointer
 void r_W(const Vector<double>& s, Vector<double>& f);


 /// \short takes the macro element coordinate position along the north 
 /// boundary and returns the derivates of the global coordinates with respect
 /// to the boundary
 void dr_N(const Vector<double>& s, Vector<double>& dr);

 /// \short takes the macro element coordinate position along the E
 /// boundary and returns the derivates of the global coordinates with respect
 /// to the boundary
 void dr_E(const Vector<double>& s, Vector<double>& dr);

 /// \short takes the macro element coordinate position along the south 
 /// boundary and returns the derivates of the global coordinates with respect
 /// to the boundary
 void dr_S(const Vector<double>& s, Vector<double>& dr);

 /// \short takes the macro element coordinate position along the W
 /// boundary and returns the derivates of the global coordinates with respect
 /// to the boundary
 void dr_W(const Vector<double>& s, Vector<double>& dr);


 /// \short takes the macro element coordinate position along the north 
 /// boundary and returns the second derivates of the global coordinates with 
 /// respect to the boundary
 void d2r_N(const Vector<double>& s, Vector<double>& d2r);

 /// \short takes the macro element coordinate position along the east 
 /// boundary and returns the second derivates of the global coordinates with 
 /// respect to the boundary
 void d2r_E(const Vector<double>& s, Vector<double>& d2r);

 /// \short takes the macro element coordinate position along the south
 /// boundary and returns the second derivates of the global coordinates with 
 /// respect to the boundary
 void d2r_S(const Vector<double>& s, Vector<double>& d2r);
 
 /// \short takes the macro element coordinate position along the west
 /// boundary and returns the second derivates of the global coordinates with 
 /// respect to the boundary
 void d2r_W(const Vector<double>& s, Vector<double>& d2r);

};
}
#endif
