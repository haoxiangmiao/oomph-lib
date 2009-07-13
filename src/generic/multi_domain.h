//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC//           Version 0.85. June 9, 2008.
//LIC// 
//LIC// Copyright (C) 2006-2008 Matthias Heil and Andrew Hazel
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
//LIC// License along with thi library; if not, write to the Free Software
//LIC// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
//LIC// 02110-1301  USA.
//LIC// 
//LIC// The authors may be contacted at oomph-lib@maths.man.ac.uk.
//LIC// 
//LIC//====================================================================
//Header file for multi-domain functions, including the class
//ElementWithExternalElement which stores pointers to external elements

//Include guards to prevent multiple inclusion of the header
#ifndef OOMPH_MULTI_DOMAIN_HEADER
#define OOMPH_MULTI_DOMAIN_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif

//Oomph-lib headers
#include "geom_objects.h"
#include "problem.h"
#include "shape.h"

#include "mesh.h"
#include "mesh_as_geometric_object.h"
#include "algebraic_elements.h"
#include "macro_element_node_update_element.h"
#include "Qelements.h"
#include "element_with_external_element.h"

namespace oomph
{

//======================================================================
// Namespace for global multi-domain functions
//======================================================================
namespace Multi_domain_functions
 {
  // Lookup scheme for whether an element's integration point
  // has had an external element assigned to it
  extern Vector<Vector<unsigned> > External_element_located;

  // List of all Vectors used by the external storage routines 
  
  /// \short Vector of (local) coordinates at integration points of
  /// elements on current processor
  extern Vector<double> Local_zetas;

  /// \short Vector of (local) coordinates at integration points of
  /// elements received from elsewhere
  extern Vector<double> Zetas;

  /// \short Vector of the dimension of the element on current processor
  extern Vector<unsigned> Local_zeta_dim;

  /// \short Vector of the dimension of the element received from elsewhere
  extern Vector<unsigned> Zeta_dim;

  /// \short Vector to indicate locally which processor a coordinate 
  /// has been located on
  extern Vector<int> Found_zeta;

  /// \short Vector of local coordinates within any elements found
  /// locally by current processor
  extern Vector<Vector<double> > Found_ss;

  /// \short Vector to indicate (on another process) whether a
  /// located element should be newly created (New), already exists (Exists), 
  /// or is not on the current process at all (Not_found)
  extern Vector<unsigned> Located_element;

  /// \short Vector of the local coordinates for each entry in Located_element
  extern Vector<double> Located_coord;

  /// \short Vector for current processor which indicates when an external
  /// halo element (and subsequent nodes) should be created
  extern Vector<unsigned> Located_zetas;

  /// \short Vector of doubles to be sent from another processor
  extern Vector<double> Double_values;

  /// \short Vector of unsigneds to be sent from another processor
  extern Vector<unsigned> Unsigned_values;

  // Counters for each of the above arrays

  /// \short Double_values
  extern unsigned Count_double_values;

  /// \short Unsigned_values
  extern unsigned Count_unsigned_values;

  /// \short Located_coord
  extern unsigned Count_located_coord;

  /// \short Local_zeta_dim
  extern unsigned Count_local_zeta_dim;

  /// \short Zeta_dim
  extern unsigned Count_zeta_dim;

  /// \short Local_zetas
  extern unsigned Count_local_zetas;
 
  /// \short Zetas
  extern unsigned Count_zetas;

  /// \short Enumerators for location procedure
  enum { New, Exists, Not_found };

  /// Default parameters for binning method to be passed to MeshAsGeomObject

  /// \short Bool to tell the MeshAsGeomObject whether to calculate
  /// the extreme coordinates of the bin structure
  extern bool Compute_extreme_bin_coordinates;

  /// \short Number of bins in the first dimension in binning method in
  /// setup_multi_domain_interaction(). Default value of 10.
  extern unsigned Nx_bin;

  /// \short Number of bins in the second dimension in binning method in
  /// setup_multi_domain_interaction(). Default value of 10.
  extern unsigned Ny_bin;

  /// \short Number of bins in the third dimension in binning method in
  /// setup_multi_domain_interaction(). Default value of 10.
  extern unsigned Nz_bin;

  /// \short Minimum and maximum coordinates for
  /// each dimension of the bin structure used in
  /// MeshAsGeomObject::locate_zeta(...). 
  /// No defaults; can be set by user if they know their mesh's coordinates
  /// (or the MeshAsGeomObject calculates these values by default based
  ///  upon the mesh itself; see MeshAsGeomObject::get_max_and_min_coords(...))

  /// \short Minimum coordinate in first dimension
  extern double X_min;

  /// \short Maximum coordinate in first dimension
  extern double X_max;

  /// \short Minimum coordinate in second dimension
  extern double Y_min;

  /// \short Maximum coordinate in second dimension
  extern double Y_max;

  /// \short Minimum coordinate in third dimension
  extern double Z_min;

  /// \short Maximum coordinate in third dimension
  extern double Z_max;

  /// \short Percentage offset to add to each extreme of the bin structure.
  /// Default value of 0.05.
  extern double Percentage_offset;

  /// \short Boolean to indicate when to use the bulk element as the
  /// external element.  Defaults to false, you must have set up FaceElements
  /// properly first in order for it to work
  extern bool Use_bulk_element_as_external;

  /// \short Boolean to indicate whether to doc timings or not.
  extern bool Doc_timings;

  /// \short Boolean to indicate whether to document info (to screen)
  ///        during setup_multi_domain_interaction() routines
  extern bool Doc_stats;

#ifdef OOMPH_HAS_MPI
  /// \short Boolean to indicate when to check for duplicate data
  ///        between the external halo storage schemes  
  extern bool Check_for_duplicates;
#endif

  // Functions for multi-domain method

  /// \short Set up the multi-domain interactions for the specified problem
  /// This is a two-mesh example where first_mesh_pt and second_mesh_pt
  /// occupy the same physical space and are populated by
  /// ELEMENT_0 and ELEMENT_1 respectively, and are combined to solve
  /// a single problem.  The interaction indices default to zero
  template<class ELEMENT_0,class ELEMENT_1>
   void setup_multi_domain_interactions(Problem* problem_pt, 
                                        Mesh* const &first_mesh_pt,
                                        Mesh* const &second_mesh_pt,
                                        const unsigned& first_interaction=0,
                                        const unsigned& second_interaction=0);

 /// \short Function to set up the multi-domain interactions for 
 /// non-FSI problems involving \c ElementWithExternalElements. 
 /// - \c mesh_pt points to the mesh of ElemenWithExternalElements for which
 ///   the interaction is set up. 
 ///   \n\n
 /// - \c external_mesh_pt points to the mesh that contains the elements
 ///   of type EXT_ELEMENT that provide the "source" for the
 ///   \c ElementWithExternalElements.
 /// - The interaction_index parameter defaults to zero and must be otherwise
 ///   set by the user if there is more than one mesh that provides sources
 ///   for the Mesh pointed to by mesh_pt
 template<class EXT_ELEMENT, unsigned EL_DIM>
   void setup_multi_domain_interaction(Problem* problem_pt,
                                       Mesh* const &mesh_pt,
                                       Mesh* const &external_mesh_pt,
                                       const unsigned& interaction_index=0);

 /// \short Function to set up the multi-domain interactions for 
 /// FSI problems (involving \c ElementWithExternalElements). 
 /// - \c mesh_pt points to the mesh of ElemenWithExternalElements for which
 ///   the interaction is set up. 
 /// - \c external_mesh_pt points to the mesh that contains the elements
 ///   of type EXT_ELEMENT that provide the "source" for the
 ///   \c ElementWithExternalElements.
 /// - \c external_face_mesh_pt points to the face mesh created from
 ///   the \c external_mesh_pt which is of the same dimension as \c mesh_pt.
 ///   The elements contained in \c external_face_mesh_pt are of type 
 ///   FACE_ELEMENT_GEOM_OBJECT 
 /// - The interaction_index parameter defaults to zero and must be otherwise
 ///   set by the user if there is more than one mesh that provides sources
 ///   for the Mesh pointed to by mesh_pt
 template<class EXT_ELEMENT, class FACE_ELEMENT_GEOM_OBJECT, unsigned EL_DIM>
   void setup_multi_domain_interaction(Problem* problem_pt,
                                       Mesh* const &mesh_pt,
                                       Mesh* const &external_mesh_pt,
                                       Mesh* const &external_face_mesh_pt,
                                       const unsigned& interaction_index=0);

 /// \short Auxiliary function which is called from the two preceding
 /// functions 
 template<class EXT_ELEMENT, class GEOM_OBJECT,
  unsigned EL_DIM_LAG, unsigned EL_DIM_EUL>
  void aux_setup_multi_domain_interaction(Problem* problem_pt,
                                          Mesh* const &mesh_pt,
                                          Mesh* const &external_mesh_pt,
                                          const unsigned& interaction_index,
                                          Mesh* const &external_face_mesh_pt=0);

#ifdef OOMPH_HAS_MPI
  /// \short A helper function to remove duplicate data that 
  /// are created due to coincident nodes between external halo elements 
  /// on different processors
  void remove_duplicate_data(Problem* problem_pt, Mesh* const &mesh_pt);
#endif

  /// \short Helper function to add external data from source elements
  /// at each integration point of the specified mesh's elements
  void add_external_data_from_source_elements
   (Mesh* const &mesh_pt,const unsigned& interaction_index);

  /// \short Helper function to locate "local" zeta coordinates
  template<class GEOM_OBJECT,unsigned EL_DIM_LAG,unsigned EL_DIM_EUL>
   void locate_zeta_for_local_coordinates
   (Mesh* const &mesh_pt, Mesh* const &external_mesh_pt,
    MeshAsGeomObject<EL_DIM_LAG,EL_DIM_EUL,GEOM_OBJECT >* &mesh_geom_obj_pt,
    const unsigned& interaction_index);

#ifdef OOMPH_HAS_MPI
  /// \short Helper function to send any "missing" zeta coordinates to
  /// the next process and receive any coordinates from previous process
  void send_and_receive_missing_zetas(Problem* problem_pt);

  /// \short Helper function to locate these "missing" zeta coordinates
  template<class GEOM_OBJECT,unsigned EL_DIM_LAG,unsigned EL_DIM_EUL>
   void locate_zeta_for_missing_coordinates
   (int& iproc,Mesh* const &external_mesh_pt,Problem* problem_pt,
    MeshAsGeomObject<EL_DIM_LAG,EL_DIM_EUL,GEOM_OBJECT >* &mesh_geom_obj_pt);

  /// \short Helper function to send back any located information
  void send_and_receive_located_info(int& iproc, Mesh* const &external_mesh_pt,
                                     Problem* problem_pt);

  /// \short Helper function to create external (halo) elements on the loop 
  /// process based on the info received in send_and_received_located_info
  template<class EXT_ELEMENT,unsigned EL_DIM_LAG,unsigned EL_DIM_EUL>
   void create_external_halo_elements(int& iproc, Mesh* const &mesh_pt,
                                      Mesh* const &external_mesh_pt,
                                      Problem* problem_pt,
                                      const unsigned& interaction_index);

  // Helper functions for external haloed node identification

  /// \short Helper function to add external haloed nodes, inc. masters
  void add_external_haloed_node_to_storage(int& iproc, Node* nod_pt,
                                           Problem* problem_pt,
                                           Mesh* const &external_mesh_pt,
                                           int& n_cont_inter_values,
                                           FiniteElement* f_el_pt);

  /// \short Helper function to add external haloed node that is not a master
  void add_external_haloed_node_helper(int& iproc, Node* nod_pt,
                                       Problem* problem_pt,
                                       Mesh* const &external_mesh_pt,
                                       int& n_cont_inter_values,
                                       FiniteElement* f_el_pt);

  /// \short Helper function to add external haloed node that is a master
  void add_external_haloed_master_node_helper(int& iproc,Node* master_nod_pt,
                                              Problem* problem_pt,
                                              Mesh* const &external_mesh_pt,
                                              int& n_cont_inter_values);

  /// \short Helper function to get the required nodal information from an
  /// external haloed node so that a fully-functional external halo
  /// node (and therefore element) can be created on the receiving process
  void get_required_nodal_information_helper(int& iproc, Node* nod_pt,
                                             Problem* problem_pt,
                                             Mesh* const &external_mesh_pt,
                                             int& n_cont_inter_values,
                                             FiniteElement* f_el_pt);

  /// \short Helper function to get the required master nodal information from
  /// an external haloed master node so that a fully-functional external halo
  /// master node (and possible element) can be created on the receiving proc
  void get_required_master_nodal_information_helper
   (int& iproc, Node* master_nod_pt, Problem* problem_pt,
    Mesh* const &external_mesh_pt, int& n_cont_inter_values);

  // Helper functions for external halo node identification

  /// \short Helper function to add external halo nodes, including any masters,
  /// based on information received from the haloed process
  template<class EXT_ELEMENT>
   void add_external_halo_node_to_storage(Node* &new_nod_pt,
                                          Mesh* const &external_mesh_pt,
                                          unsigned& loc_p,
                                          unsigned& node_index,
                                          EXT_ELEMENT* new_el_pt,
                                          int& n_cont_inter_values,
                                          Problem* problem_pt);

  /// \short Helper function to add external halo node that is not a master
  template<class EXT_ELEMENT>
   void add_external_halo_node_helper(Node* &new_nod_pt,
                                      Mesh* const &external_mesh_pt,
                                      unsigned& loc_p,
                                      unsigned& node_index,
                                      EXT_ELEMENT* new_el_pt,
                                      int& n_cont_inter_values,
                                      Problem* problem_pt);

  /// \short Helper function to add external halo node that is a master
  template<class EXT_ELEMENT>
   void add_external_halo_master_node_helper(Node* &new_master_nod_pt,
                                             Node* &new_nod_pt,
                                             Mesh* const &external_mesh_pt,
                                             unsigned& loc_p,
                                             EXT_ELEMENT* new_el_pt,
                                             int& n_cont_inter_values,
                                             Problem* problem_pt);


  /// \short Helper function which constructs a new external halo node 
  /// (on an element) with the information sent from the haloed process
  template<class EXT_ELEMENT>
   void construct_new_external_halo_node_helper(Node* &new_nod_pt,
                                                unsigned& loc_p,
                                                unsigned& node_index,
                                                EXT_ELEMENT* new_el_pt,
                                                Mesh* const &external_mesh_pt,
                                                Problem* problem_pt);

  /// \short Helper function which constructs a new external halo master node
  /// with the information sent from the haloed process
  template<class EXT_ELEMENT>
   void construct_new_external_halo_master_node_helper
   (Node* &new_master_nod_pt,Node* &nod_pt,unsigned& loc_p,
    EXT_ELEMENT* new_el_pt,Mesh* const &external_mesh_pt,Problem* problem_pt);

#endif

  /// Helper function that clears all the intermediate information used
  /// during the external storage creation at the end of the procedure
  void clean_up();

 }


}

#endif




 
