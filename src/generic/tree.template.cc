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
// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

//oomph-lib headers
#include "tree.h"

#include <stdio.h>


#ifndef OOMPH_TREE_TEMPLATE_CC
#define OOMPH_TREE_TEMPLATE_CC

namespace oomph
{


//================================================================
/// If required, split the leaf element and create its sons 
///
/// Split is performed if
///
///      object_pt()->to_be_refined()
///
/// returns true.
///
/// If this is the case, then we execute
/// 
///      object_pt()->split(new_elements_pt) 
/// 
/// to create the sons. Pointers to the son objects are then inserted
/// into the son pointers of the present element. This turns the
/// present element into a "grey" (=non-leaf) node.
///
//=================================================================
template<class ELEMENT>
void Tree::split_if_required()
{


#ifdef PARANOID
   if (Son_pt.size()!=0)
    {
     std::string error_message =
      "Can't split non-leaf elements (or at least I can't see\n";
     error_message += 
      "why you would want me to... If you're sure, then hack me... \n";

     throw OomphLibError(error_message,
                         OOMPH_CURRENT_FUNCTION,
                         OOMPH_EXCEPTION_LOCATION);
    }

   if (Object_pt==0)
    {
     std::stringstream error_message;
     error_message << "No object defined in split_if_required. Father nodes:\n";

     RefineableElement* el_pt=Father_pt->object_pt();
     unsigned nnod=el_pt->nnode();
     for (unsigned j=0;j<nnod;j++)
      {
       Node* nod_pt=el_pt->node_pt(j);
       unsigned n=nod_pt->ndim();
       for (unsigned i=0;i<n;i++)
        {
         error_message << nod_pt->x(i) << " ";
        }
       error_message << "\n";
      } 
     throw OomphLibError(error_message.str(),
                         OOMPH_CURRENT_FUNCTION,
                         OOMPH_EXCEPTION_LOCATION);
    }

#endif

 // Check if refinement is required
 if (Object_pt->to_be_refined())
  {
   // Perform the split for the element in question and return Vector
   // of pointers to the newly created elements
   Vector<ELEMENT*> new_elements_pt;
   Object_pt->split(new_elements_pt);
   
   //Find the number of sons constructed by the element
   unsigned n_sons = new_elements_pt.size();

   // Turn the new elements into QuadTrees and assign
   // the pointers to the present element's sons
   Son_pt.resize(n_sons);
   Tree* father_pt = this;
   for(unsigned i_son=0;i_son<n_sons;i_son++)
    {
     Son_pt[i_son]= construct_son(new_elements_pt[i_son],
                                  father_pt,i_son);
     // Now that the son knows its position in the tree, we can set it
     // up as a proper element (e.g. p-refineable stuff)
     Son_pt[i_son]->object_pt()->initial_setup();
    }
  }
}


//================================================================
/// If required, p-refine/unrefine the leaf element
///
/// p-refinement is performed if
///
///      object_pt()->to_be_p_refined()
///
/// returns true. p-unrefinement is performed if
///
///      object_pt()->to_be_p_unrefined()
///
/// returns true.
///
/// If this is the case, then we execute
/// 
///      object_pt()->p_refine(+/-1,mesh_pt,clone_pt) 
/// 
/// to p-refine the element, where mesh_pt is a pointer to the
/// mesh that the element lives in, and clone_pt is a pointer to
/// a GeneralisedElement that has all the information that would
/// be needed from the father element during h-refinement.
///
//=================================================================
template<class ELEMENT>
void Tree::p_refine_if_required(Mesh* &mesh_pt)
{


#ifdef PARANOID
   if (Son_pt.size()!=0)
    {
     std::string error_message =
      "Can't p-refine non-leaf elements (or at least I can't see\n";
     error_message += 
      "why you would want me to... If you're sure, then hack me... \n";

     throw OomphLibError(error_message,
                         OOMPH_CURRENT_FUNCTION,
                         OOMPH_EXCEPTION_LOCATION);
    }

   if (Object_pt==0)
    {
     std::stringstream error_message;
     error_message << "No object defined in p_refine_if_required. Father nodes:\n";

     RefineableElement* el_pt=Father_pt->object_pt();
     unsigned nnod=el_pt->nnode();
     for (unsigned j=0;j<nnod;j++)
      {
       Node* nod_pt=el_pt->node_pt(j);
       unsigned n=nod_pt->ndim();
       for (unsigned i=0;i<n;i++)
        {
         error_message << nod_pt->x(i) << " ";
        }
       error_message << "\n";
      } 
     throw OomphLibError(error_message.str(),
                         OOMPH_CURRENT_FUNCTION,
                         OOMPH_EXCEPTION_LOCATION);
    }

#endif

 //Cast to p-refineable element
 PRefineableElement* p_ref_obj_pt
  = dynamic_cast<PRefineableElement*>(Object_pt);
 
 //Check if we can p-refine the element
 if (p_ref_obj_pt==0)
  {
   throw OomphLibError("Element cannot be p-refined",
                       OOMPH_CURRENT_FUNCTION,
                       OOMPH_EXCEPTION_LOCATION);
  }


 // Create a temporary clone of the element to be refined.
 // This is required so that the element can read data from "itself"
 // while it rebuilds itself with a new p-order. Only the information
 // required for the p-refinement is copied across.
   
 // Make new element (using ELEMENT's standard constructor)
 ELEMENT* clone_pt = new ELEMENT();

 // Do initial setup with myself as the clone's adopted father
 clone_pt->initial_setup(this);

 // All that is left to do is to "build" the clone. It has the same
 // nodes as me, so we can just copy across their pointers
 for(unsigned j=0; j<clone_pt->nnode(); j++)
  {
   clone_pt->node_pt(j) = p_ref_obj_pt->node_pt(j);
  }


 // Check if refinement is required
 if (p_ref_obj_pt->to_be_p_refined())
  {
   // Perform the split for the element in question and return Vector
   // of pointers to the newly created elements
   p_ref_obj_pt->p_refine(1,mesh_pt,clone_pt);
  }
 // Check if unrefinement is required
 else if (p_ref_obj_pt->to_be_p_unrefined())
  {
   // Perform the split for the element in question and return Vector
   // of pointers to the newly created elements
   p_ref_obj_pt->p_refine(-1,mesh_pt,clone_pt);
  }


 //Delete the temporary copy of the element
 delete clone_pt;

}

 
}

#endif
