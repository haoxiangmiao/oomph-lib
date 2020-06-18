/**

\mainpage The data structure

\section intro Introduction
 
\c oomph-lib is big! This document gives a "bottom up" overview of
the library's data structure and discusses how the various objects interact. 
In addition to the detailed discussion provided below, the following
doxygen-generated lists/indices provide quick access to 
the documentation of \c oomph-lib's classes: clicking on the 
hyperlinks associated with a class takes you directly to a 
detailed description of its inheritance structure and
its members.
 
<ul>

<li><a href="../../the_data_structure/html/classes.html"> Class index </a></li>

<li><a href="../../the_data_structure/html/functions.html"> Class member index </a></li>

</ul>

 
 This rest of this document provides a "bottom up" overview of the data 
 structure in \c oomph-lib and discusses how the various objects interact. 
 For brevity, we usually replace the list of arguments to functions 
 by `(...)' and explain the main input and 
 output parameters in words. The full specifications of the 
 interfaces may be found in the individual class documentation,
 accessible via the links at the top of this page.
   

 \subsection technical_terms Technical terms
 Throughout this document, certain commonly used terms have a specific
 technical meaning: 
 - \b Value: A \e value is a (double precision) number (e.g. a "nodal 
   value"). A value can either be an \e unknown in the problem 
   or be determined by a boundary condition. 
 - \b Unknown: An \e unknown is a \e value that is not determined by a
   boundary condition.
 - \b Degree \b of \b freedom: Synonym for \e unknown; often
   abbreviated as "dof".
 - \b History \b value: \e History \e values are (double precision)
   numbers that are used by \c TimeSteppers to calculate 
   time-derivatives of values. For instance, history values are often,
   but \b not \b always, the values at previous timesteps.
 - \b Pinned/free: \e Values that are (are not) determined by boundary
   conditions are known as pinned (free) values.
 - \b Adapt: \e Mesh \e adaptation refines/un-refines meshes
   by adding or deleting selected nodes and elements. 

 \section general_data_structure Overview of the basic data structure

 The main components of \c oomph-lib are \c Data, \c Node, 
\c GeneralisedElement, \c Mesh and \c Problem.

 \subsection data Data
 The most elementary data structure in \c oomph-lib is \c Data (ha!).

 Consider the solution of a scalar PDE 
 (e.g. a Poisson equation) with certain boundary conditions. 
 The numerical solution of this problem requires the computation of 
 the function values (double precision numbers) at a finite number 
 of spatial positions (the \c Nodes). Typically, these values fall into two
 categories: those that are known \e a \e priori (i.e. are enforced by 
 boundary conditions), and those that must be determined as part of the 
 solution.
 
 \c Data stores a value --- a double precision number.
 Typically,  the values of the unknowns are determined by
 the solution of a system of algebraic equations. The solution
 of this system usually requires a (linear) 
 numbering of the unknowns and associated equations. Hence, \c Data also
 stores a (long) integer that represents the number of the unknown
 in the global numbering scheme.  \b Convention: If the \c Data value 
 is pinned, we set the equation number to the static member data
 \c Data::Is_pinned, a negative number.

 The number of an unknown is related to the
 number of the equation that `determines its value', so we use
 the terms `equation number' and `number of the unknown'
 interchangeably. In fact, because the term `number of the unknown'
 is rather tedious, we \e only use the term `equation number'. 
  
 Two observations motivate a straightforward extension
 of this basic data structure:
 
 - In time-dependent problems, the approximation of time-derivatives
   requires the storage of a certain number of auxiliary values
   (e.g. the values of the unknowns at a number of previous timesteps).
 - In many problems, we are not dealing with scalars (i.e. individual 
   doubles) but with vector-valued unknowns; for instance, in a 
   3D fluids problem, we need to store three velocity components 
   at every node. 
 
 Therefore, \c Data allows the storage of multiple values 
 (all of which can either be pinned or free, and all of which have their
 own global equation number); \c Data can also store a certain number of 
 auxiliary (history) values used for timestepping.
 Finally, \c Data stores a pointer to a \c TimeStepper whose member
 functions relate the history values to the values' time-derivatives.

 Direct, pointer-based read/write access to the \c Data values is provided by
 the functions \code Data::value_pt(i) \endcode
 which returns a pointer to the i-th value at the present time, and by
 \code Data::value_pt(t,i) \endcode
 which returns a pointer to the \c t -th history value associated with
 value \c i. 
 Read-only access is also provided by the functions 
 \code
 Data::value(i)
 \endcode
 and its time-dependent counterpart
 \code
 Data::value(t,i)
 \endcode
 We recommend using these functions instead of the 
 pointer-based access functions for read access 
 because the two \c Data::value(...) functions are overloaded
 in the \c Node class (discussed below) so that they return
 suitably constrained nodal values if a \c Node is hanging.
 The \c Data::value(...) functions cannot be used to set \c Data
 values. For this purpose we provide the functions
 \code
 Data::set_value(i,val)
 \endcode
 which sets the \c i -th \c Data value to the double precision number
 \c val;  and its time-dependent counterpart
 \code
 Data::set_value(t,i,val)
 \endcode
 which sets the \c t -th history value associated with the 
 \c i -th \c Data value to the double precision number
 \c val.

 The general convention for all time-dependent data is 
 that the index t=0 refers to values at the present time, whereas
 the values associated with t>0 correspond to history values.
 In many cases (e.g. BDF schemes)
 these history values are simply the values at previous timesteps, but
 this is not guaranteed. See the section \ref timestepping
 for further details.

 \subsection nodes Nodes
 
 In FE computations, most (but not all; see below) \c Data are associated
 with nodal points. Conversely, all \c Nodes in a finite element
 mesh have \c Data associated with them. \c Nodes are therefore derived
 from \c Data, but also store a spatial position, specified 
 by a certain number of spatial (Eulerian) coordinates.

 The nodal positions are accessed by the member function
 \code Node::x(i) \endcode
 which returns the current value of i-th nodal coordinate or
 \code Node::x(t,i) \endcode
 which returns the value of i-th nodal coordinate at the present
 (t=0) timestep or a history value, if (t>0); again, note that the
 history values are not necessarily positions at previous timesteps.


 \subsubsection advanced_features_of_nodes Advanced features:
 
 \c Nodes have the following additional features:

 - In moving-mesh problems, we must evaluate mesh velocities, which
   requires the storage of the nodal position at a number of previous
   timesteps. Storage for the positional history values is allocated 
   by the \c Node constructor. A different \c TimeStepper may be used to 
   represent time-derivatives of nodal position, so
   \c Nodes store a separate pointer to a positional \c TimeStepper.
   [Note: By default, we allocate the same amount of storage for
   the history of the nodal positions as we do for the history of
   the nodal values;  e.g. if a \c BDF<2> scheme is used to evaluate
   the time-derivatives of the fluid velocities, we assume that
   the same timestepping scheme is used (and the same amount of storage 
   required) to determine the mesh velocities from the nodal
   positions.]
 - For finite elements in which the 
   global position of a point in an element is determined by interpolation 
   from the position of the element's \c Nodes (Lagrange-type elements), 
   we need only store
   the spatial position of the \c Nodes. In many other elements (e.g.
   Hermite-type elements), the interpolation of the geometry requires
   additional quantities, representing, e.g. the derivative of the
   mapping between local and global coordinates.
   Therefore, we allow the storage of additional positional variables 
   at each \c Node, so that, in general, every \c Node
   has a number of \e generalised coordinates for each spatial coordinate
   direction. For instance, for nodes in 1D Hermite elements,
   the nodal coordinate of type '0' stores the global position at the
   node; the nodal coordinate of type '1' stores the derivative
   of the global position w.r.t. to the element's local
   coordinate. The member function \code Node::x_gen(k,i) \endcode
   returns the \c i -th coordinate of the \c k -th coordinate type. 
 - In the context of mesh refinement, nodes can become hanging nodes
   (i.e. nodes on an element's edge/face that are not shared by the 
   adjacent element). The "hanging" 
   status of a \c Node is indicated by the pointer to its \c HangInfo
   object. For ordinary (non-hanging) \c Nodes this pointer is NULL, 
   the default setting; see the section \ref Hanging_Nodes for a more
   detailed discussion of hanging nodes; in particular, the role of
   the member functions \c Node::position() and \c Node::value()
 - Mesh unrefinement can render nodes obsolete
   and we use a boolean flag to indicate this status.
   By default, \c Nodes are not obsolete.
 - \b BoundaryNodes: \c Nodes "know" the domain/mesh boundaries on which 
   they are located. A \c Node can be located
   on none, one or multiple boundaries; the latter case arises if the
   \c Node is located on edges and corners of the mesh.    
   Storage of this information facilitates the automatic
   determination of boundary conditions for new \c Nodes that are
   created during mesh refinement.\n 
   The majority of the \c Nodes will \b not be located on boundaries,
   however, and providing storage for the boundary information 
   in every \c Node object is rather wasteful. The derived class
   \c BoundaryNode adds the
   required additional storage to the \c Node class and it follows that  
   all \c Nodes that
   could lie on boundaries must be \c BoundaryNodes; see \ref meshes for
   further details.
 - \b SolidNodes: Many solid mechanics problems 
   are formulated in Lagrangian coordinates. 
   In this case, the governing equations are discretised in a fixed
   Lagrangian domain; the nodal coordinates represent the \c Nodes' fixed
   positions in this domain. When the elastic body deforms,
   material points (and hence the \c Nodes) are displaced 
   to new Eulerian positions. The \c SolidNode class is derived from
   \c Node and contains storage for 
   the \c Nodes'
   \b fixed positions (i.e. the Lagrangian coordinates)
   AND their \b variable  positions (i.e. the
   Eulerian coordinates), which can be unknowns in the problem. 
   To avoid confusion between the two, the access function for the nodal 
   position, \code Node::x(...) \endcode \b always refers to a \c Node's 
   Eulerian position/coordinate. In \c SolidNodes, we provide 
   a wrapper \code SolidNode::xi(..) \endcode that provides access 
   to the Lagrangian coordinates. Similarly, in the case of generalised
   coordinates \code Node::x_gen(...) \endcode always refers to the
   generalised Eulerian position and \code SolidNode::xi_gen(...)
   \endcode refers to the generalised Lagrangian position.

 \subsection elements Elements
  
 \subsubsection Overview
 Most (finite-)elements in \c oomph-lib have a four-level inheritance 
structure which separates:
 -# the basic functionality that is shared by all (generalised) elements.
 -# the functionality that is shared by all finite elements.
 -# the implementation of the finite element geometry (i.e. the 
    shape-function-based mapping between local and global coordinates).
 -# the representation of the mathematics that describes a specific problem.
 .
 The distinction between geometry and `maths' greatly 
 facilitates code-reuse as a (geometric) quad-element, say, can
 form the basis for elements that solve a variety of equations
 (e.g. Poisson, Advection--Diffusion, Navier--Stokes, ...).
 We shall now discuss the four levels of the element
 hierarchy in more detail.
 
 \subsubsection Generalised_Element Level 0: GeneralisedElement
 The class \c GeneralisedElement forms the base class for all
 elements in \c oomph-lib. It incorporates the basic functionality that
 all \b elements must have. The interfaces at this level are 
 so general that a \c GeneralisedElement can represent discrete algebraic 
 constraints (or even finite-difference stencils).

   The main role of elements is to provide contributions to
   a global residual vector and to the global Jacobian matrix.
   The two virtual functions
   \code  virtual void GeneralisedElement::get_residuals(...) \endcode
   \code  virtual void GeneralisedElement::get_jacobian(...) \endcode
   specify the appropriate interfaces.

   In multi-physics problems, the elemental residuals and
   Jacobian will be a combination of the residuals vectors and 
   Jacobian matrices of the constituent
   single-physics elements. An obvious implementation is to use
   multiple inheritance and function overloading 
   \code
   class MultiPhysicsElement : public virtual SinglePhysicsOneElement,
                               public virtual SinglePhysicsTwoElement
    {   

     [...]
                        
     void get_residuals(...) 
      {
       SinglePhysicsOneElement::get_residuals(...);
       SinglePhysicsTwoElement::get_residuals(...);
      }

     [...]

    };
    \endcode
    where the \c MultiPhysicsElement inherits from \c
   SinglePhysicsOneElement and \c SinglePhysicsTwoElement.
 
   A problem with this implementation arises when we consider where to
   initialise the residuals vector. If the second single-physics 
   \c get_residuals(...) function initialises the residuals
   vector, then the contribution of the first single-physics element
   will be negated. When writing a single-physics element, however,
   we cannot know whether it will ever be used as part of a
   multi-physics element and, if so, in which order the \c get_residuals(...)
   functions will be called. The solution adopted in \c oomph-lib is
   to provide the two additional virtual functions
   \code 
   virtual void GeneralisedElement::fill_in_contribution_to_residuals(...)
   \endcode
   \code 
   virtual void GeneralisedElement::fill_in_contribution_to_jacobian(...)
   \endcode
   which \b must \b not initialise the residuals or Jacobian, but merely add
   the contribution of the element to the vector or matrix.

   We then use the default implementation
   \code 
   virtual void GeneralisedElement::get_residuals(Vector<double> &residuals) 
    {
     //Zero the residuals vector
     residuals.initialise(0.0);
  
     //Add the elemental contribution to the residuals vector
     fill_in_contribution_to_residuals(residuals);
     }
    \endcode
    which permits a simple multi-physics re-implementation
    \code
     class MultiPhysicsElement : public virtual SinglePhysicsOneElement,
                                 public virtual SinglePhysicsTwoElement
      {   

       [...]

      void get_residuals(Vector<double> &residuals) 
       {
        //Zero the residuals vector
        residuals.initialise(0.0);

        //Add the first elemental contribution to the residuals vector
        SinglePhysicsOneElement::fill_in_contribution_to_residuals(residuals);

        //Add the second elemental contribution to the residuals vector   
        SinglePhysicsTwoElement::fill_in_contribution_to_residuals(residuals); 
       }

       [...]

     };
     \endcode

    The default implementation of \c fill_in_contribution_to_jacobian(...)
    uses finite differences to calculate the Jacobian matrix. Hence,
    the simplest possible implementation of a new element requires
    only the specification of \c fill_in_contribution_to_residuals(...).

 - When computing element residuals and Jacobian matrices, we need to 
   know which \c Data affects the residuals (and hence the Jacobian matrix). 
   For a \c GeneralisedElement such \c Data exists in two forms, accessed 
   via pointers:
   - \c Data that is internal to each element is accessed via pointers
     to `Internal \c Data'. For instance, in fluid (finite) elements with
     discontinuous pressure interpolations, the pressure degrees of freedom 
     are local to each element and are stored in 
     the element's 'Internal \c Data'.
   - \c Data that is external to the element. An example is
     a load parameter such as the external pressure that acts
     on a shell structure. Such \c Data is accessed via pointers
     to `External \c Data'.  
   - As discussed above, all \c Data contains values that are either 
     free (i.e. unknown) or pinned (i.e. prescribed by boundary 
     conditions). Free/unknown values have a non-negative global
     (equation) number. When assembling an element's local 
     contribution to the global residual vector and the Jacobian matrix,
     we refer to the unknowns by their local (equation) 
     numbers. In order to add the elemental contribution to the appropriate 
     global degree of freedom, every element has a lookup table that 
     establishes the relation between local and global equation numbers.
     This lookup table is automatically generated by the 
     element's member function 
     \code GeneralisedElement::assign_local_eqn_numbers() \endcode
     Access to the lookup scheme is provided by
     the member function \c eqn_number(...) so that
     \code 
 
     unsigned i_local;
     GeneralisedElement el;

     [...]
 
     unsigned i_global=el.eqn_number(i_local); 

     \endcode 
     returns the global equation number \c i_global corresponding 
     to the local equation number \c i_local. 
     The local equation numbers of the internal and external \c Data are
     stored in the private arrays \c Internal_local_eqn and \c
     External_local_eqn, accessed by the functions 
     \c GeneralisedElement::internal_local_eqn(...) 
     and \c GeneralisedElement::external_local_eqn(...), respectively. Thus, 
     \c GeneralisedElement::internal_local_eqn(i_internal,i_value) 
     returns the local
     equation number of the \c i_value -th value stored in the
     \c i_internal -th internal \c Data object. 
   - All elements have a pointer to a global \c Time object which 
     allows the evaluation of time-dependent
     coefficients. 
   .
.

 \subsubsection Finite_Element Level 1: FiniteElement
 The class \c FiniteElement is derived from \c GeneralisedElement and
 incorporates the basic functionality that all \b finite elements must have.

 - All \c FiniteElements have a certain
   number of \c Nodes. We access the \c Nodes (and their associated values) 
   via pointers and identify them via their (local) node numbers so that
   \code  FiniteElement::Node_pt[n] \endcode
   or the access function
   \code FiniteElement::node_pt(n) \endcode
   returns a pointer to the element's \c n -th local \c Node.
  
  - The \c FiniteElement class provides wrapper functions that 
    give direct access to an element's
    (possibly generalised) nodal positions at the present timestep or,
    where appropriate, its positional history values, so that rather than
    \code FiniteElement::Node_pt[n]->x(i) \endcode
    we can write
    \code FiniteElement::nodal_position(n,i) \endcode
    Not only does this make the code more readable but also
    allows us to formulate the "mathematics" in general terms.\n 
    [Furthermore,
    the function \c FiniteElement::nodal_position(n,i) accesses 
    the nodal positions indirectly
    via  \c Node::position(...) which ensures that the nodal position 
    is consistent with any constraints associated with the \c Node's 
    hanging status; see section \ref Hanging_Nodes for further details.]

  - When \c Nodes are created in a (templated) finite element mesh, 
    it is important that \c Nodes of the correct type with the appropriate
    amount of storage are created.
    For instance, Poisson elements require \c Nodes that provide storage
    for a single value at each \c Node, whereas 2D Taylor-Hood, Navier-Stokes 
    elements require storage for three values (two velocities and one
    pressure) at the corner \c Nodes, 
    but only two values (the two velocities) at all others. 
    The member function
    \code FiniteElement::construct_node(...) \endcode 
    creates a \c Node, stores a pointer to the \c Node in the 
    \c FiniteElement::Node_pt 
    vector and returns a pointer to the newly created \c Node. The
    function is overloaded in elements that require a different type
    of \c Node, for example \c SolidElement::construct_node(...) creates
    a \c SolidNode rather than a \c Node. \n
    The function 
    \c FiniteElement::construct_node(...) determines
    the necessary parameters for the node construction from virtual
    functions or internal data that must have been set during
    construction of the particular element. 
    \anchor set_commands
    The spatial dimension of the \c Node and the number of
    generalised coordinates must be set in the constructor of a
    geometric \c FiniteElement (level 2 in the element hierarchy) 
    by using the appropriate protected member
    functions. The only function that \b must be called is
    \code FiniteElement::set_dimension(dim), \endcode
    which sets the spatial dimension of the element; by default the
    spatial dimension of the \c FiniteElement's \c Nodes is assumed
    to be the same. For example,
    \code FiniteElement::set_dimension(2) \endcode sets both the
    spatial dimension of the element and the spatial dimension of 
    its \c Nodes to be two. If the nodal dimension is not the same as the
    dimension of the element the member function
    \code FiniteElement::set_nodal_dimension(dim_node) \endcode
    should be used to change the value of the nodal dimension. By
    default, \c FiniteElements interpolate a single position type, the
    position itself. If generalised coordinates are used, the number of
    generalised coordinates should be set using the function
    \code FiniteElement::set_n_nodal_position_type(n_pos_type). \endcode
    The number of values stored at each \c Node is determined from the
    virtual member function
    \code FiniteElement::required_nvalue(...)  \endcode
    In its default implementation this function returns zero so the 
    function must be overloaded in specific derived \c FiniteElements
    that require storage for some values at its \c Nodes. 
    
    See section \ref meshes for a full explanation of how 
    and when \c Nodes are created.

  - The \c FiniteElement class also defines standard interfaces 
    for member functions 
    that compute the shape functions and their derivatives with 
    respect to the local and global (Eulerian) coordinates,
    \code FiniteElement::shape(...) \endcode
    \code FiniteElement::dshape_local(...) \endcode
    The mappings from local to global Eulerian coordinates are
    implemented in complete generality in the class. The function
    \code  FiniteElement::interpolated_x(...) \endcode
    returns the interpolated Eulerian position within the element;
    \code FiniteElement::dshape_eulerian(...), \endcode
    returns the derivative of the shape functions with respect
    to the Eulerian coordinates; and
    \code  FiniteElement::J_eulerian(...) \endcode
    returns the Jacobian of the mapping from Eulerian
    coordinates to local coordinates.

  - The function \c GeneralisedElement::assign_local_eqn_numbers() 
    is overloaded in the \c FiniteElement class to ensure
    that local equation numbers are also assigned to the nodal \c Data
    (which does not necessarily exist in all \c GeneralisedElements). These
    local equation numbers are stored in the private array
    \c  FiniteElement::Nodal_local_eqn, accessed by the function \c
    \c FiniteElement::nodal_local_eqn(...).

 \subsubsection Geometric_Element Level 2: Geometric Elements

 At this level, we specify the element geometry and the
 mapping between local and global coordinates. 
 Wherever possible, templating has been (and, for any newly
 developed elements, should be) used to formulate the elements 
 in a dimension-independent way. For instance, Lagrange-type
 1D line, 2D quad and 3D brick elements are implemented in
 the doubly-templated \c QElement<DIM,NNODE_1D> class.
 The template parameters indicate the spatial
 dimension of the element and the number of nodes 
 along the element's one-dimensional edges. Hence, \c QElement<1,3>
 is a three-node line element with a quadratic mapping between
 local and global coordinates; \c QElement<3,2> is an 8 node brick element
 with a trilinear mapping between local and global coordinates. The
 dimension and the number of \c Nodes must be set by
 calling the appropriate \c set_ functions in the constructor, see
 \ref set_commands "above".

 The most important member functions implemented at this level
 include
 - Functions that evaluate the shape functions (and their derivatives) 
   at given values of the local coordinates.
 - Functions that specify the position of each \c Node inside
   its \c FiniteElement, in terms of the \c FiniteElement's local
   coordinates and, conversely, functions that determine whether 
   a \c Node is located at a particular local coordinate. 
 - Output functions that allow the element shapes to be plotted.
 .
 Finally, we specify a pointer to a spatial integration
 scheme (usually a Gauss rule). The order of the
 integration scheme is based on the order of the interpolation
 in the isoparametric mapping. If this is inappropriate for
 an element that is derived from a given geometric element, the default 
 assignment can be over-written at a higher level. This is
 discussed in more detail in a 
 <A HREF="../../optimisation/html/index.html">
 separate document.</A>

 \subsubsection Maths_Element Level 3: `The Maths'

 At this level, we implement the equations that are represented by
 the specific element. We implement the interpolation(s) for the 
 unknown function(s),
 employing either the geometric shape functions that already exist
 on level 2, or employing additional shape functions 
 defined at this level. This allows us to write further member 
 functions such as \c interpolated_u(...), say, which compute 
 the i-th velocity component at the local coordinate \c s like this
 \code
 // Create fluid element
 SomeFluidElement fluid_element;

 [...]

 // Vector of local coordinates
 Vector<double> s(3);

 // Vector of velocity components
 Vector<double> u(3);

 // Compute the velocity at local coordinate s
 fluid_element.interpolated_u(s,u);
 \endcode
 We introduce wrapper functions 
 to access function values, so that we can formulate
 "The Maths" in generic terms. Rather than referring
 to the pressure at node \c n, via
 \code
 unsigned n;
 double press = Node_pt[n]->value(3);
 \endcode
 (which forces us to remember that in this particular 3D fluid 
 element, the pressure is stored as the fourth value at all nodes...),
 say, we provide an access function \c p_fluid(...) which allows us to write
 \code
 unsigned n;
 double press = fluid_element.p_fluid(n);
 \endcode
 When writing these wrapper functions, direct access to the
 nodal values should be avoided to ensure that the element 
 remains functional in the presence of hanging nodes.
 Hence, the wrapper functions should make use of the 
 \c Node::value(...) functions as in this example
 \code
 double SomeFluidElement::p_fluid(const unsigned& n)
  { return Node_pt[n]->value(3);}
 \endcode
 [See section \ref Hanging_Nodes for a full description of hanging nodes.]

 The functions
 \code FiniteElement::assign_additional_local_eqn_numbers()  \endcode
 \code FiniteElement::get_residuals(...) \endcode
 \code FiniteElement::get_jacobian(...) \endcode

 that were defined (as virtual functions) in \c FiniteElement 
 can now be implemented for the specific system of equations that
 are represented by this element. The function \c
 assign_additional_local_eqn_numbers() is called by \c
 FiniteElement::assign_local_eqn_numbers() and may be used to assign local
 equation numbers that correspond to particular physical
 variables. For example, in QCrouzeixRaviart "fluid" elements, the
 pressure is stored as 'internal \c Data', so an internal array \c P_local_eqn
 could be defined by 
 \code P_local_eqn[i] = internal_local_eqn(0,i) \endcode
 but in QTaylorHood "fluid" elements, the pressure is stored as nodal
 \c Data, so that
 \code P_local_eqn[i] = nodal_local_eqn(Pconv[i],DIM+1) \endcode
 In the above, \c Pconv[i] is an array that returns the local node number
 at which the \c i -th pressure freedom is stored and \c DIM is the
 dimension of the element. 
 The use of such an array introduces a memory overhead, however,
 because each element must permanently store these additional
 integers. In general, we prefer to use member functions for this
 purpose. In QCrouzeixRaviart elements, for example,
 \code 
 int p_local_eqn(const unsigned &n) 
  {
   return internal_local_eqn(0,n);
  }
 \endcode
 Thus, in our standard equations the function \c
 assign_additional_local_eqn_numbers() is not used.

 Finally, the virtual function
 \code FiniteElement::required_nvalue(...)  \endcode
 should be implemented to specify the number of values that 
 are stored at each of the element's local \c Nodes. The default 
 number of values stored at a \c Node is zero. 
 
 \subsubsection advanced_features_of_elements `Advanced' features for the `Maths' level:

 It often makes sense to subdivide the `Maths' level further into
 -# A class that contains the abstract FE formulation of the mathematical
    problem.
 -# A class that combines the mathematical formulation with
    a specific geometrical element.
 
 An example is given by the \c QPoissonElements
 which inherit their maths from \c PoissonEquations (templated by the
 spatial dimension) and their geometry from \c QElement (templated by
 the spatial dimension and the number of nodes).  \c PoissonEquations
 specifies the weak form of the Poisson equation
 in terms of (virtual) shape and test functions. The \c QPoissonElements 
 turn this abstract formulation into a specific (isoparametric) 
 element, by specifying both test and shape functions 
 as the geometric shape functions defined in \c QElement.
 

 \subsubsection no_args_for_element_constructors Important convention regarding element constructors
 
 To facilitate mesh generation and adaptation it is important that element
 constructors should \b not have \b any arguments! If arguments
 must be passed to an element (e.g. function pointers to 
 source functions, etc.), this should be done \b after the mesh 
 generation, usually in the \c Problem constructor. In adaptive mesh
 refinement procedures any function/data pointers in newly created
 elements are set to be the same as those of the father
 element. If this is not the desired behaviour, arguments should be
 passed to the elements in the function \c
 Problem::actions_after_adapt().
 
 \subsection meshes Meshes

 At its most basic level, a \c Mesh is simply a collection of elements
 and \c Nodes, accessed by the vectors of pointers, 
 \c Mesh::Element_pt and \c Mesh::Node_pt, respectively. To facilitate 
 the application of  boundary conditions, we also store vectors of
 pointers to the (\c Boundary)\c Nodes that lie on the boundaries of the mesh. 
 They are accessible via the member function
 \code Mesh::boundary_node_pt(i,j) \endcode
 which returns a pointer to the \c j -th node on the \c i -th
 boundary of the mesh.

 \subsubsection Mesh_generation Mesh generation

 The \c oomph-lib data structure is designed to make the mesh generation 
 process generic so that meshes developed for one 
 particular problem can easily be re-used in others. For this reason, 
 it is generally assumed that a \c Mesh contains only elements of a 
 single type and that this element type is passed to the \c Mesh 
 constructor as a template parameter. Problems that require 
 multiple element types should use multiple meshes,
 which are discussed in more detail below. It is possible to mix types
 of elements within a single \c Mesh, if desired; this can
 be advantageous when the element types are very closely related, 
 for example "free surface" elements in a mesh of "Fluid" elements.
 
 Mesh generation (usually performed in the \c Mesh 's constructor) 
 then works as follows:
 -# Create the first element and add the pointer to it to the 
    \c Mesh::Element_pt vector. (We know which element to build 
    because we have passed its type as a template parameter.)
    Since element constructors do not take any 
    arguments, this step is completely generic 
    and ensures that a \c Mesh that was originally
    created to solve a Poisson equation, say, can also be used to solve
    Navier-Stokes equations (provided the element topology
    is the same, i.e. provided the elements are derived from the
    same type of geometric element (e.g. quad or triangle). 
    The element now exists but does not know anything about its 
    \c Nodes etc.
 -# Loop over the element's \c Nodes and for each Node:
    -# Create the \c Node using
       the element's \code FiniteElement::construct_node(...) \endcode 
       member function. 
       As discussed above, this function creates \c Nodes of exactly the right
       type and fills in the  element's own \c Node_pt vector.
    -# \c FiniteElement::construct_node(...) returns a pointer to the
       newly created \c Node; add this to the \c Mesh::Node_pt vector.
    -# Assign the nodal coordinates.
    .
    The \c Node is now fully functional and (by default) all values
    that are stored with it are free (i.e. not pinned). 
 -# If the \c Node is located on a mesh boundary then it must be a
    \c BoundaryNode. \c BoundaryNodes can be created using the function
    \code FiniteElement::construct_boundary_node(...) \endcode
    in place of \c FiniteElement::construct_node(...).
    Alternatively, if the \c Node has already been created, it can 
    be upgraded to a \c BoundaryNode by using the function
    \code Mesh::convert_to_boundary_node(...) \endcode
    The function
    \code Mesh::add_boundary_node(i,&node) \endcode 
    should then be used to add (a pointer to) the \c BoundaryNode to 
    the \c Mesh's boundary-storage scheme.
    In addition, the function 
    \c Mesh::add_boundary_node() passes boundary
    information to the \c Node itself.
 -# Create the next element and loop over its \c Nodes. Some \c Nodes will
    already exist (because they have been created by the first 
    element).
    For such \c Nodes, we merely add the pointer to the existing \c Nodes
    to the element's \c Node_pt vector. If a \c Node does not 
    exist yet, we create it, as discussed above.
 -# Keep going until all the elements and \c Nodes have been built.
  
 \subsubsection Node_DOF_numbering Equation/DOF numbering at the mesh level

 Now that the \c Mesh is assembled, we can set up the numbering scheme for 
 the unknowns that are associated with the \c Nodes and with the
 elements' `internal \c Data '. (For problems that 
 involve `external \c Data ', i.e. \c Data that is not associated with 
 \c Nodes and elements, a further step is required; see section \ref problems 
 below). 
 As discussed above, whenever a \c Data object is created (either 
 as part of a \c Node in the mesh or as `internal \c Data ' inside an 
 element), its values are assumed to be free (i.e. not pinned).
 Before we can set up the equation numbering scheme, we must
 pin all those \c Data values that are prescribed by boundary
 conditions. This is generally done at the \c Problem level (see below) and for
 the subsequent discussion, we assume that this step has already taken
 place.
 
 The equation numbering scheme must achieve two
 things:
 - Every unknown value needs to be associated with a unique, non-negative
   global equation number.
 - All elements need to set up the lookup tables that establish
   the relation between local and global equation numbers.

 These tasks are performed in two steps. The function
 \code Mesh::assign_global_eqn_numbers(...) \endcode
 whose argument is a vector of pointers to doubles, \c dof_pt,
 assigns the (global) equation numbers for the element's internal \c Data 
 and for all \c Data associated with the \c Mesh's \c Nodes.
 On return from this function, \c dof_pt[i] points to 
 the value of the i-th global degree of freedom.  
 The setup scheme (which is fully implemented in \c oomph-lib) works 
 as follows:
 -# Loop over all \c Nodes in the \c Mesh.
    - Loop over all values that are stored at that \c Node.
      - If the value is free (i.e. not pinned), add the
        pointer to the value to the \c dof_pt vector.
 -# Loop over all elements.
    - In every element, loop over the `internal \c Data '
      - For every instance of `internal \c Data ', loop over its values.
        - If the value is free (i.e. not pinned), add
          pointer to the value to the \c dof_pt vector.
        .
      .
    .
 .
 Once this has been done (for all \c Meshes, if there are multiple ones),
 the function
 \code Mesh::assign_local_eqn_numbers(...) \endcode
 loops over all elements and executes their
 \code FiniteElement::assign_local_eqn_numbers(); \endcode 
 member function to set up the elements' lookup table that translates
 between local and global equation numbers.
 
 \subsection problems Problems
 
 Finally, we reach the highest level of the \c oomph-lib hierarchy, the 
 \c Problem itself. The generic components of \c Problem, 
 provided in the base class of that name, are:
 - A pointer to the global \c Mesh (which can represent a number of
   submeshes, see below).
 - A pointer to the (discrete) \c Time (see section on \ref timestepping).
 - A vector of pointers to (possibly multiple) \c TimeSteppers.
 - A vector that holds pointers to any `global \c Data ', i.e. \c Data 
   that is not associated with elements or \c Nodes. 
 - A vector \c Dof_pt that stores the pointers to all the unknown 
   values (the degrees of freedom) in the problem. 
 

 \b Multiple \b Meshes

  The mesh generation process previously described was for meshes
 that contain only elements of a single type. The process 
 could easily be generalised to meshes that contain multiple element
 types by providing multiple template parameters.  
 When solving a fluid-structure interaction (FSI)
 problem we could, therefore, create a single mesh that discretises 
 both the fluid and the solid domains with appropriate elements.
 However, to facilitate code re-use it is desirable to keep meshes
 as simple as possible so that a mesh that was originally
 developed for a pure fluids problem can also be used in an FSI 
 context. For this reason, the \c Problem class 
 allows a problem to have multiple (sub-)meshes. Pointers to each
 sub-mesh must be stored in Problem's \c Sub_mesh_pt vector by using
 the function
 \code Problem::add_sub_mesh(Mesh* const &mesh_pt) \endcode
 However, many of the generic operations within \c oomph-lib 
 (equation numbering,
 solving, output of solutions,...) involve looping over \b all elements
 and \c Nodes in the problem. Therefore, if a \c Problem contains multiple 
 (sub-)meshes, the sub-meshes must be combined into a single
 global \c Mesh whose \c Element_pt and \c Node_pt vectors
 provide ordered access to the elements and \c Nodes in \b all
 submeshes. The function
 \code Problem::build_global_mesh() \endcode
 combines the sub-meshes into the global \c Mesh and
 must be called once all the sub-meshes have been
 constructed and "added" to the \c Problem.

 \b Important: Many operations (such as the shifting of 
 history values in time-stepping) must be performed exactly
 once for each \c Node (or \c Data). Therefore, the vector of (pointers to) 
 nodes in the global \c Mesh must not contain any duplicate entries. 
 When copying (pointer to) \c Nodes from the submeshes into the global 
 \c Mesh, the function \c Problem::build_global_mesh() ignores
 any \c Nodes that have already been copied from a previous submesh.
 [The \c Mesh::self_test()
 function checks for duplicates in the \c Mesh::Node_pt vector.]
 
 \b Convention: Recall that (\c Boundary)Nodes are `told' about the number
 of the boundary they live on when the (sub-)meshes are constructed.
 In the context of multiple meshes, this raises the question
 if this number should continue to refer to the boundary number within
 the submesh or be updated to a boundary number within the global \c Mesh.
 We adopt the convention that boundary numbers remain those that were 
 originally assigned when the submeshes were constructed.
 
 \b Multiple \b meshes \b and \b adaptivity:
 Mesh adaptation is performed separately for each submesh. Following the
 adaptation of one or more submeshes (in the process of which various
 \c Nodes/elements will have been created/deleted), we must also update
 the global \c Mesh. This is done by calling
 \code  Problem::rebuild_global_mesh(); \endcode
 and this function is executed automatically if the mesh 
 adaptation/refinement is performed by
 \code  Problem::refine_uniformly(); \endcode
 or
 \code  Problem::adapt(); \endcode
 
 \subsubsection prob_setup Problem Construction

 Here's an overview of how \c Problems are set up and solved in \c oomph-lib. 
 For simplicity, we illustrate the process for a problem with 
 a single \c Mesh that contains elements of a single type.
 -# Define the element type and the \c TimeStepper (if the problem
    is time-dependent).
 -# Build the \c Mesh, passing the element type as a template parameter
    and the \c TimeStepper as an argument to the Mesh constructor.
    (Typically \c Mesh constructors take a pointer to a \c TimeStepper
    as an argument since the \c TimeStepper needs to be passed
    to the element's \c FiniteElement::construct_node(...) function.
    If the \c Problem has no time-dependence, we can pass a pointer to
    the static \c Mesh::Steady timestepper; many \c Mesh constructors
    use a pointer to this \c TimeStepper as the default argument). 
 -# Create the \c Problem 's global \c Data (if it has any).
 -# Apply the essential boundary conditions by pinning 
    the appropriate values; all other values remain free.
 -# We now have a fully assembled \c Mesh and all elements know
    their constituent \c Nodes. However, because element constructors
    are not allowed to have any arguments, all but the simplest of
    elements will now have to be provided with additional information.
    For instance, we might need to set pointers to 
    source functions, etc. 
 -# Assign the equation numbers to all unknowns in the problem.
    This is accomplished in a two-stage process by
    \code Problem::assign_eqn_numbers(...) \endcode
    which 
    -# Assigns the equation numbers for all global \c Data (if any)
    -# Loops over the submeshes to
       perform the global equation numbering for all values associated
       with the \c Meshes (i.e. \c Nodes and elements), using   
      \code Mesh::assign_global_eqn_numbers(...) \endcode
    -# Loops over the submeshes again to
       perform the local equation numbering for all elements associated
       with the meshes using   
      \code Mesh::assign_local_eqn_numbers(...) \endcode
    .
    [Note that we have to assign the global equation numbers for
    all meshes before we assign any local equation numbers. This is
    because the external \c Data of elements in one mesh might be
    nodal \c Data
    in another mesh -- think of fluid-structure interaction problems].
    At the end of this step, we will have filled in the \c Problem's
    \c Dof_pt vector which holds the pointers to the unknowns. [Note:
    The equation numbering scheme that is generated by the above procedure
    is unlikely to be optimal; it can (and in the interest of efficiency
    probably should) be changed afterwards 
    by a problem-specific renumbering function.]
 -# Before we can solve the \c Problem we will usually have to 
    perform a few additional (problem-dependent) initialisation steps.
    For instance, we might want to assign initial and/or boundary 
    conditions and provide initial guesses for the unknowns.
    For time-dependent problems, the \c Problem class provides a member 
    function
    \code Problem::assign_initial_values_impulsive() \endcode
    which creates a past history for all time-dependent
    unknowns, assuming an impulsive start. If you want a `smooth'
    start from a given previous time-history, you will have to implement
    this yourself; in this case, consult the section 
    \ref timestepping which outlines the time-stepping procedures 
    in \c oomph-lib.
 -# Now we can solve the \c Problem by calling 
    \code Problem::newton_solve() \endcode
    which employs the Newton-Raphson method to solve the nonlinear system
    of equations that is specified (formally) by the global Jacobian matrix
    and the global residual vectors. The function 
    \c Problem::newton_solve() 
    employs a linear solver which may be specified by 
    \c Problem::linear_solver_pt(). The default linear solver is
    <a href="http://crd.lbl.gov/~xiaoye/SuperLU/"> SuperLU </a>.
    The Newton iteration proceeds
    until the maximum residual falls below 
    \c Problem::Newton_solver_tolerance. [If the number of iterations
    exceeds \c Problem::Max_newton_iterations or if the
    the maximum residual exceeds \c Problem::Max_residuals, the
    Newton solver throws an error]. When a solution has been found, 
    all unknowns in the problem (which are accessible to the \c Problem via
    its \c Dof_pt vector) are up-to-date. 
    The \c Problem class also provides the function
    \code Problem::unsteady_newton_solve(...) \endcode
    for time-dependent problems. Given the current values of the
    unknowns (at time \c t = \c Problem::Time_pt->time() ) 
    and their past histories, this function
    determines the solution at the advanced time \c t+dt. See 
    section \ref timestepping  for more details on the conventions 
    used in timestepping.
 .
 The \c Problem class also has member functions which assemble the 
 global residual vector and the global Jacobian matrix.


 \b Important: \n \n Since the unknowns in the \c Problem are accessed
 directly via pointers, their values are automatically updated
 during the Newton iteration. If the \c Problem has any auxiliary
 parameters that depend on the unknowns, their values need
 to be updated whenever an unknown might have changed (i.e.
 after every step of the Newton iteration).
 For such cases, the \c Problem class provides the four (empty) 
 virtual functions
 \code Problem::actions_before_newton_step() \endcode
 \code Problem::actions_after_newton_step() \endcode
 \code Problem::actions_before_newton_solve() \endcode
 and
 \code Problem::actions_after_newton_solve() \endcode
 which are executed before/after every step of the Newton iteration
 and before/after the nonlinear solve itself, respectively. 
 In addition, the virtual function
 \code Problem::actions_before_newton_convergence_check() \endcode
 is executed before the residuals are calculated in the Newton solver.

 When you  formulate your own \c Problem, you will have to decide what 
 (if anything)  should live in these functions. Typical examples of 
 actions that should
 be taken before a solve are the update of any boundary conditions. If
 the boundary conditions depend upon variables in the problem, they must
 be updated before every Newton step and should therefore be placed in
 \c Problem::actions_before_newton_step(). Actions that take 
 place after a Newton step
 or solve would include things like updating the nodal positions,
 writing output or any other
 post-processing/solution monitoring. For 
 example, if the solution after each Newton step were to be documented
 this could be accomplished by calling a suitable output function in the
 \c Problem::actions_after_newton_step() function. 

 In many cases, only the 
 \c Problem::actions_before_newton_convergence_check() function is
 required. On entry to the Newton solver, the initial residuals are
 computed and checked, so the function is executed \b before any Newton steps
 are taken. After each Newton step, the residuals
 vector is recomputed and checked, so the function is also called
 after every Newton step (or before the next Newton
 step). Nonetheless, we provide all five functions for the greatest
 possible flexibility.

 \section timestepping Time-stepping 

 \subsection time Time

 Time-derivatives are generally evaluated by finite difference
 expressions, e.g. by BDF schemes. Hence, within the code, functions are
 only ever evaluated at discrete time levels. The class \c Time contains 
 (a pointer to) the `current' value of the continuous time and a 
 vector of current and previous timesteps so that the value of the 
 continuous time at any previous timestep can easily be reconstructed. 
 This is useful/necessary if there are any explicitly time-dependent 
 parameters in the problem. The general convention within all timestepping
 procedures is to associate the values at the `present' time (i.e. the 
 time for which a solution is sought)  with time level `0',
 those at the previous time level (where the solution is
 already known) with time levels `1', `2' etc. The function
 \code Time::time(t) \endcode
 therefore returns the `current' value of the continuous time if t=0,
 the continuous time before the previous timestep if t=1, etc.

 \subsection basic_timestepping Basic Time-stepping

 The base class \c TimeStepper provides the basic functionality 
 required to evaluate time derivatives, and to keep track of the time 
 histories of the unknowns. Primarily, a \c TimeStepper stores the coefficients
 (weights) that allow the evaluation of time-derivatives (up to a certain
 order) in terms of the history values stored in \c Data. Synchronisation of
 multiple \c TimeStepper s is ensured by providing them with pointers to the
 \c Problem's (single) \c Time object. 

 Here's an illustration of the time-stepping procedure for
 an implicit scheme. 
 - \b Stage \b 1: \b Initialise
   -# Add a \c TimeStepper to the \c Problem. If a \c Time object has 
      not yet been
      created, the \c Problem::add_time_stepper_pt(...) function 
      creates one with the necessary storage. 
      If the \c Time object already exists and the new \c TimeStepper 
      requires more storage than presently exists,
      the storage in the \c Time object is resized. The function also passes
      a pointer to the global \c Time object to each \c TimeStepper.
   -# Set up the \c Problem as discussed in section \ref problems 
   -# Initialise the history of the previous timesteps by calling
      \c Problem::initialise_dt(...).
   -# Provide initial values for all unknowns. \b Note: In many cases,
      the initial values may be the result of a steady calculation. The
      function \c Problem::steady_newton_solve() should be used to
      calculate these values. The function sets the weights of
      the time-stepping scheme such that the time derivatives are zero
      and a steady problem is solved, even when the \c Problem's
      \c TimeStepper is not the dummy timestepper, \c Steady.
   -# Provide time histories for the values (pinned \e and free!),
      either by imposing an impulsive start or by setting history
      values according to some given time-dependence:
      When generating the initial time history for the values
      from a given (`exact') solution, assign the current values and
      the history values so that the solution is represented correctly
      at the initial time, \c Time::time(). At the end of this stage,
      \c Data::value(...) must return the current values and  
      \c TimeStepper::time_derivative(...) must return their
      time-derivatives at the initial time.
 - \b Stage \b 2: \b Perform \b one \b timestep: 
    \n A timestep is performed by using the 
    \code  Problem::unsteady_newton_solve(...) \endcode function, 
     which implements the following steps:
   -# Shift the time values back using
      \code Problem::shift_time_values() \endcode
      (For BDF timesteppers, this simply moves the history values
      back by one level; see below for a detailed discussion of
      how the shifting of the history values is performed).
   -# Solve the \c Problem at the advanced time by performing the
      following steps
       -# Choose a timestep \c dt.
       -# Advance the \c Problem's global time
          and (re-)calculate the weights for the \c TimeStepper(s). 
       -# Update any time-dependent boundary conditions etc., 
          usually via \code Problem::actions_before_newton_solve() \endcode
          or \code Problem::actions_before_implicit_timestep()
          \endcode
       -# Call the nonlinear solver to find the unknowns at the
          current (advanced) time. 
 - \b Stage: \b 3 \b Document \b the \b solution \n
    We now have a completely consistent representation of the
    current and history values of the system's unknowns
    for the current value of \c Time::time().
    This is an excellent moment to dump the solutions to
    disk or do any other post-processing. These steps may be 
    included in \code Problem::actions_after_newton_solve() \endcode 
    or  \code Problem::actions_after_implicit_timestep() \endcode 
 - Now return to \b Stage \b 2 for the next timestep.
 
 It is important to understand how the shifting of the timesteps
 (in preparation for the next timestep) is performed because certain default
 operations will not be appropriate for all \c Elements/ \c
 Problems. Recall that
 in time-dependent problems, \c Data::value_pt(i) points to the current
 (and, as yet, unknown) \c Data values, while \c Data::value_pt(t,i) for 
 t>0 points to the history values that the \c TimeStepper uses to
 work out time-derivatives. When we move to the next
 timestep, the history values need to be adjusted in the 
 manner that is appropriate for the timestepping scheme. For instance, 
 for BDF schemes, all values need to be pushed back by one time level.
 This operation is performed by the function
 \code  TimeStepper::shift_time_values(...) \endcode In the case of 
 \c Nodes,
 the shifting of values associated with the nodal positions is
 performed by the function 
 \code TimeStepper::shift_time_positions(...) \endcode
 To ensure that all \c Data in the \c Problem is shifted once (and only once!)
 \c Problem::shift_time_values()
 performs the following operations:
 -# Shift the values of the time history stored in the \c Time object.
 -# Shift the time values in the global \c Mesh. This involves the
    following steps:
    -# Loop over all elements and call 
       \c TimeStepper::shift_time_values(...) for
       the \c TimeStepper corresponding to each internal \c Data value. This
       leads to the slightly ugly construction \code
       internal_pt(i)->time_stepper_pt()->shift_time_values(internal_pt(i))
        \endcode but there appears to be no way to avoid this.
    -# Loop over all \c Nodes in the mesh and
       -#  call \c TimeStepper::shift_time_values(...) 
           for each \c Node 's \c TimeStepper.
       -#  call \c TimeStepper::shift_time_positions(...) for each 
           \c Node's positional \c TimeStepper.
 -# Shift the time values for all global \c Data.
 

 \subsection adaptive_time_stepping Adaptive time-stepping

 In adaptive time-stepping, the size of the timestep is adjusted
 automatically, so that the global (temporal) error estimate,
 computed by the Problem member function
 \code Problem::global_error_norm() \endcode
 remains below a preset threshold. However, the function 
 \c Problem::global_error_norm() must be implemented for
 each specific problem. The error norm is usually 
 constructed from the (estimated) errors of individual
 \c Data values. Estimates for these quantities are given by
 the differences between the actual value and 
 a predicted value, as determined by
 \code TimeStepper::error_in_value(...) \endcode 
 and the errors in positional \c Data values, found by
 \code TimeStepper::error_in_position(...) \endcode 
 
 In moving mesh problems, a suitable norm is the root-mean-square
 of the errors in all positional coordinates at every \c Node. In fluid
 problems, the error is usually based on the velocity components, etc.

 Once a suitable norm has been chosen, a single adaptive timestep is
 taken by the function
 \code Problem::adaptive_unsteady_newton_solve(...) \endcode
 This function returns a double precision number that is the value 
 of \c dt that should be
 taken at the next timestep. A typical calling loop is thus
 \code 
  // Initial suggestion for timestep
  double dt=0.001;
  for(unsigned t=0;t<100;t++)
   {
    // Try to take a timestep dt forward; if the computed
    // solution is not sufficiently accurate, reduce the
    // the timestep and repeat... Return the suggestion for
    // next timestep
    double dt_next = adaptive_unsteady_newton_solve(dt,...);
    dt = dt_next;
   }
  \endcode

 Within the \c Problem::adaptive_unsteady_newton_solve(..) function,
 if the global error norm is too large, the step is rejected, the timestep
 reduced and the step repeated. If the timestep falls below a 
 preset tolerance \c Problem::Minimum_dt (which has the default value 
 \c 1.0e-12), the program will terminate. It is also possible to
 set a maximum timestep by over-writing the (large) default
 for \c Problem::Maximum_dt (initialised to  \c 1.0e12) with a smaller
 value.

 \subsection restarts Restarts
 Time-dependent simulations can consume a lot of computer time, and
 it is essential to be able to restart simulations rather than having
 to re-do them from scratch if a computer crashes during the code
 execution. For this purpose 
 the \c Problem class provides the two member functions
 \code Problem::dump(...) \endcode
 and 
 \code Problem::read(...) \endcode
 which write/read the generic \c Problem components (i.e. the \c Data 
 values and
 history values, the history of previous timestep increments, etc) to/from
 disk. These generic functions are typically called from within a specific \c
 Problem's own dump/read functions which also deal with any additional, 
 problem-specific data that has to be recorded/reset to allow a proper 
 restart.

 
 \section mesh_refinement Problem/Mesh adaptation

 \subsection RefineableProblems Adaptation at the Problem level

 The ability to adaptively refine/unrefine the \c Problem's mesh(es)
 in regions in which the solution undergoes rapid/slow variations 
 is of crucial importance for the overall efficiency of the 
 solution process. Mesh-adaptation involves the following steps:
 -# Compute an estimate of the error in the computed solution
    for all elements in the mesh.
 -# Label the elements whose error estimate is the above the maximum
    (or below the minimum) permissible error.
 -# Perform the actual mesh adaptation: elements whose error is too large
    are subdivided into `son' elements; elements whose error is too 
    small are merged with their `siblings', provided the `siblings'
    are also scheduled for de-refinement. Usually, certain bounds are 
    imposed on the maximum and minimum refinement levels 
    to prevent excessive mesh refinement (e.g. near singularities)
    or de-refinement. (While the use of very large elements might be 
    permissible in regions in which the solution varies little, such
    elements will provide a poor representation of the domain shape
    and thus lead to unsatisfactory post-processing.)
 -# Represent the previously computed solution on the newly created
    \c Nodes / elements.
 -# Once all the \c Problem's submeshes have been adapted, update the 
    \c Problem itself by updating the global \c Mesh, re-generating 
    the equation numbering scheme, etc.
 -# Solve the adapted \c Problem.
 
 The \c Problem class provides several
 functions that perform these tasks completely automatically. For 
 instance, the function
 \code  Problem::refine_uniformly() \endcode
 performs one uniform mesh refinement step for all (refineable)
 submeshes in the \c Problem. Similarly, the function
 \code  Problem::adapt(...) \endcode
 performs one mesh adaptation step.
 In both functions, mesh adaptation is followed by the update of 
 the global \c Mesh and the re-assignment of the equation numbers
 so that, on return from these functions, the \c Problem can immediately
 be solved again.
 
 The \c Problem class also provides overloaded versions
 of the steady and unsteady Newton solvers
 \code Problem::newton_solve(...) \endcode
 and 
 \code Problem::unsteady_newton_solve(...) \endcode
 that automatically perform mesh adaptations until the computed 
 solution satisfies the specified error bounds on all submeshes
 (or until a max. number of adaptations has been performed).
 The (empty) virtual member function
 \code Problem::set_initial_condition() \endcode
 establishes the interface to the function that sets
 all \c Data to their initial conditions. This function must be overloaded if
 nontrivial initial conditions are to be applied. (If mesh adaptations
 are performed while the first timestep is computed, the initial 
 conditions on the adapted mesh can usually be represented more accurately
 by re-assigning them, rather than by interpolation from the 
 coarse initial mesh). 
 
 \subsection RefineableMeshes Adaptation at the Mesh level

 The ability to perform the adaptation at the \c Problem level relies
 on the availability of standardised interfaces to functions 
 that handle the adaptation on the \c Mesh level. These 
 interfaces are provided in the class \c RefineableMeshBase.
\c  RefineableMeshBase is derived from \c Mesh and stores
 a pointer to a spatial error estimator, as well as
 double precision numbers representing the target
 error levels for the adaptation. The member function
 \code  RefineableMeshBase::refine_uniformly(...) \endcode
 performs one uniform mesh refinement step. Similarly,
 \code  RefineableMeshBase::adapt(...) \endcode
 adapts the mesh according to the specified error bounds, using the mesh's
 spatial error estimator to compute the elemental errors.

 The details of the mesh adaptation process depend on the
 mesh topology; currently the virtual functions in
 \c  RefineableMeshBase are implemented in a general form for quad and
 brick elements. 
 We shall discuss the mesh adaptation process 
 in detail for meshes of a particular type: The \c RefineableQuadMesh
 class implements the mesh adaptation procedures for two-dimensional, 
 block-structured 
 meshes which consist of the refineable variant of 2D \c QElements.
 The description provides a template for the development of mesh refinement
 procedures for meshes with different element topologies (e.g. triangular
 elements, or 3D \c QElements). 

 \subsection RefineableQuadMesh_section Mesh adaptation for 2D quadrilateral meshes

 \subsubsection RefineableQElementsInmeshes Refineable QElements
 
 The abstract base class \c RefineableQElement<2> `upgrades' 
 existing elements of (geometric) type \c QElement<2,NNODE_1D> to versions 
 that are suitable for use in adaptive computations. 'Upgrading'
 is achieved via inheritance so that, e.g., refineable 
 Poisson elements are defined as:
 \code 
 template <unsigned DIM, unsigned NNODE_1D>
 class RefineableQPoissonElement : public QPoissonElement<DIM,NNODE_1D>,
 public virtual RefineablePoissonEquations<DIM>,
 public virtual RefineableQElement<DIM>
 {
  [...]
 }
 \endcode
 The abstract base class \c RefineableQElement<2> defines
 virtual interfaces for those \c FiniteElement member functions
 that might have to be overloaded in the refineable version.
 In most cases, these member functions must be 
 re-implemented to deal with the possible presence of hanging nodes,
 see below.
 
 \subsubsection QuadTreeInMeshes QuadTree procedures for mesh refinement
 
 Many of the mesh adaptation procedures for meshes of type 
 \c RefineableQuadMesh use 
 quadtree representations of the mesh. The quadtree navigation 
 and search algorithms are based on those described in H. Samet's "The 
 design and analysis of spatial data structures" (Addison-Wesley, 1990).
 [Note: Unfortunately, in the usual tree terminology, quadtrees are 
 made of \e "nodes" which are, of
 course, completely unrelated to the \c Nodes in the finite element
 mesh! The context and -- within this document -- the different
 typefaces should make it clear which is which...]
 It is important to understand that each \c RefineableQElement<2> has an
 associated \c QuadTree and each \c QuadTree has an associated
 \c RefineableQElement<2>. This two-way "has a" relationship permits a
 "clean" implementation of the (generic) \c QuadTree algorithms, although
 it does incur the cost of two additional pointers.

 To illustrate the way in which \c RefineableQuadMeshes are represented 
 by \c QuadTrees, the figure below shows a simple finite element mesh 
 together with its quadtree-representation.
 There are two different types of quadtree classes: \c QuadTrees and
 \c QuadTreeRoots, which inherit from \c QuadTrees.
 The overall structure of the quadtree
 is defined by pointers between its \e "nodes". 
 Each \e "node" (an object of type \c QuadTree, shown in
 pink) in the quadtree
 has a pointer to its \e "father" (if this pointer is 
 NULL, the \e "node" is the \e "root node") and a vector of pointers to its
 four \e "sons" (if the vector is empty, the "node" is a \e "leaf 
 \e node"). This data structure is sufficient to navigate the
 quadtree (e.g. identify the quadtree's \e "leaf nodes", determine a 
 \e "node"'s 
 neighbours, etc.) Each \c QuadTree also stores a pointer
 to an associated \e "object" of type \c RefineableElement (shown
 in light blue). The finite element mesh that is used in the computations
 only comprises those \c RefineableElements that are associated with
 \e "leaf nodes". We refer to these elements as "active elements". 
 In the diagram below, the active elements are identified by thick blue
 boundaries and the blue element numbers correspond to those in the mesh.
 
 \image html QuadTree.gif "Illustration of a QuadTree and the associated RefineableQuadMesh" 
 

 \subsubsection RefineableQuadMesh_setup_section Mesh generation

 Any \c Mesh that is designed to contain 2D \c QElement s forms a suitable 
 basis for a \c RefineableQuadMesh mesh and the initial stages of 
 the mesh generation process
 can be performed exactly as described in section \ref meshes above. 
 Typically, the constructor for a \c RefineableQuadMesh
 will produce a relatively 
 coarse background mesh (sometimes referred to as a "root mesh") 
 which can subsequently be refined as required. As
 discussed before,  the type of element will typically be passed as 
 a template parameter and it is assumed that in any concrete 
 instantiation of the \c RefineableQuadMesh class, 
 the element is derived from the class \c RefineableQElement<2>.

 Following the generic setup of the mesh (creating elements, \c Nodes, etc), 
 the constructor of the \c RefineableQuadMesh must associate each 
 \c RefineableQElement<2> in the mesh with a \c QuadTreeRoot and vice versa.
 The association between \c RefineableQElement<2>s
 and \c QuadTreeRoots is established by the \c QuadTreeRoot constructor which
 takes the pointer to the \c RefineableQElement<2> as its argument.
 The different \c QuadTrees must then be combined into a \c QuadTreeForest,
 whose constructor establishes each \c QuadTree 's 
 N/S/W/E neighbours and their relative orientation. Here is an 
 illustration:
 
 \image html QuarterCircleMesh.gif "Mesh consisting of three RefineableQElement<2>s/ QuadTrees. Element/QuadTree numbers are blue, and the QuadTree's `northern' direction is indicated by the red arrows. The QuadTrees are combined to a QuadTreeForest which establishes their adjacency (QuadTree 2 is QuadTree 0's northern neighbour etc.) and their relative orientation (QuadTree 1's North coincides with QuadTree 2's West, etc.) Note that the RefineableQElement<2>'s local coordinates  s_0 and s_1  are aligned with the QuadTree 's W-->E and S-->N directions, respectively." 

 \image latex QuarterCircleMesh.eps "Mesh consisting of three RefineableQElements<2>/ QuadTrees. Element/QuadTree numbers are blue, and the QuadTree's `northern' direction is indicated by the red arrows. The QuadTrees are combined into a QuadTreeForest, which establishes their adjacency (QuadTree 2 is QuadTree 0's northern neighbour etc.) and their relative orientation (QuadTree 1's North coincides with QuadTree 2's West, etc.) Note that the RefineableQElement<2>'s local coordinates  s_0 and s_1  are aligned with the QuadTree 's W-->E and S-->N directions, respectively." 

 The virtual member function 
 \code RefineableQuadMesh::setup_quadtree_forest() \endcode
 creates the \c QuadTreeForest automatically and should be called at the end
 of the \c RefineableQuadMesh's constructor. 
 Finally, the mesh must be given a pointer to a spatial error estimator.
 
 \subsubsection Hanging_Nodes Hanging Nodes

 Before explaining the details of the mesh adaptation process we
 discuss how hanging nodes are treated in \c oomph-lib. The figure below
 shows a \c RefineableQuadMesh that 
 originally consisted of  four 4-node \c RefineableQElements 
 and nine nodes (nodes 0 to 8). The mesh was adapted twice: During the first
 adaptation, the top right element was subdivided into 
 four son elements and five new \c Nodes (nodes 9 to 13) were created.
 Then one of the newly created elements was subdivided again
 and in the process \c Nodes 14 to 18 were created. \n
 On this mesh, inter-element continuity is not ensured unless the
 hanging \c Nodes (=internal \c Nodes not shared by an adjacent 
 element -- here \c Nodes 9, 10, 14, 15, 17 and 18) are suitably constrained.
 For instance, the nodal values at \c Node 10 must be linear
 combinations of the values at nodes 4 and 7 if the solution
 is to remain continuous across the eastern edge of element 2.
 We refer to nodes 4 and 7 as the "master nodes" of \c Node
 10. \n
 The hanging node status of the \c Nodes can be determined 
 by the following procedure:
 - Loop over all elements in the mesh. For each element \b E do:
   - Loop over the edges of element.  For each edge \b e_E do:
     - Identify the neighbouring element, \b N, that faces edge \b e_E. 
     - If the neighbouring element, \b N, has the
       same size as element \b E then the hanging node 
       status of the \c Nodes that are located on edge \b e_E does 
       not need to be changed.
     - If the neighbouring element, \b N, is larger than  \b E, 
       determine the edge \b e_N of element \b N that faces the
       edge \b e_E. Make all \c Nodes on edge \b e_N 
       master nodes for all hanging \c Nodes on edge \b e_E.
     .
   . 
 .
 If the difference in the refinement levels of two adjacent elements
 is greater than one, some of the master nodes determined by the
 above procedure will themselves be hanging nodes and
 these now need to be eliminated recursively. For instance, \c Nodes 9 and 11
 are initially classified as master nodes for \c Node 17. However, 
 \c Node 9 is a hanging node which depends on \c Nodes 4 and 5. 
 Hence, the ultimate master  nodes for \c Node 17 are \c Nodes 4, 5 and 11.
 

 \image html HangingNodes.gif "Illustration of hanging nodes"

 \image latex HangingNodes.eps "Illustration of hanging nodes"

 The presence of hanging nodes must not affect the 
 node-based representations for the unknown function(s) and for the 
 geometric 
 mapping within the elements. Hence, within any
 element \f$ E \f$, a scalar function \f$ u \f$ should continue to 
 be representable as
 \f[ u = \sum_{j} U_{J(j,E)} \psi_j(s) \mbox{\hspace{5cm} (1)} \f]
 while the mapping between local and global coordinates should retain 
 the form
  \f[   {\bf x} = \sum_{j} {\bf X}_{J(j,E)} \psi_j(s),
        \mbox{\hspace{5cm} (2)} \f]
 where the sums are taken over the nodes \f$ j \f$ of 
 the element. \f$ J(j,E) \f$
 represents the global node number of local node \f$ j \f$ in element 
 \f$ E \f$ and \f$ U_J \f$ and \f$ {\bf X}_J \f$ represent 
 the function value at and 
 the position vector to global node  \f$ J \f$, respectively.
 To ensure inter-element continuity of \f$ u \f$ and  \f$ {\bf x} \f$, 
 we constrain the nodal values and 
 positions of the hanging nodes so that for every hanging
 node \f$ J \f$ we have
  \f[ U_J = \sum_{K} U_{K} \omega_{JK}  \f] 
 and 
  \f[ {\bf X}_J = \sum_{K} {\bf X}_{K} \omega_{JK} \f]
 where the sum is taken over the hanging node's master nodes \f$ K \f$
 and the \f$ \omega_{JK} \f$ are suitable weights.
 It is precisely this representation of the nodal positions and
 values that is implemented in \c Node::value(...) and \c Node::position().
 [Note that different nodal values can have different hanging node
 constraints; e.g. in Taylor-Hood elements where the pressure and 
 velocities are interpolated by linear and quadratic basis functions.]

 For simply hanging nodes (e.g. \c Nodes 9, 10, 14 and 15 in the above sketch)
 the weights \f$ \omega_{JK} \f$ are determined as follows:
 - Find the neighbour element \b N that faces the hanging node \f$ J \f$.
 - Let \f$ s_J \f$ be the local coordinate of hanging node  \f$ J \f$ 
   in the neighbour element \b N.
 - For each master node \f$ K \f$  in \b N, the weight \f$  \omega_{JK} \f$
   is then given by the shape function associated with master node
   \f$ K \f$, evaluated at \f$ s_J \f$, evaluated in element \b N.
   \f[ \omega_{JK} = \psi_K(s_J) \f]
 .
 For multiply hanging nodes (\c Nodes 17 and 18 in the above sketch),
 the weights of the ultimate master nodes are determined 
 recursively, e.g. \f$ \omega_{17\ 4} = \omega_{17\ 9}  
 \times \omega_{9 \ 4}. \f$
 
 For \c RefineableQuadMesh meshes, 
 the above procedures are fully implemented.
 Executing the function
 \code  RefineableQElement<2>::setup_hanging_nodes() \endcode
 for each element in the mesh establishes
 which of the elements' \c Nodes are hanging and determines their 
 primary master nodes and weights. Furthermore, it pins 
 the values at the hanging nodes -- because the values are constrained,
 they cannot be unknowns in the problem. When this function has been 
 executed for all elements in the mesh, the recursive
 elimination of hanging master nodes is performed by
 calling 
 \code RefineableMesh::complete_hanging_nodes(); \endcode

 As mentioned above, the possible occurrence of hanging nodes 
 needs to be reflected in
 the element's equation numbering scheme and in the functions that
 compute the elemental Jacobian matrix because the element
 residuals now potentially depend on \c Nodes outside the element. 
 Therefore, \c RefineableQElement<2>s must re-implement
 various \c FiniteElement member functions, e.g., by re-implementing
 the virtual functions
 \code  RefineableQElement<2>::get_residuals() \endcode
 and 
 \code  RefineableQElement<2>::get_jacobian(...) \endcode
 and various others, as specified in the \c RefineableQElement<2>
 class. 

 In practice, we again distinguish between the "geometry" and the "maths"
 by writing a general \c RefineablePoissonEquations<DIM> class that
 inherits from \c PoissonEquations<DIM> and re-implements the appropriate
 member functions.


 \subsubsection RefineableQuadMesh_adaption_section Mesh adaptation
 We can now discuss the details of the mesh adaptation process
 for \c RefineableQuadMesh meshes, although the general procedure is, in fact,
 generic:
 Once a solution has been computed,
 \c Problem::adapt() loops over all refineable submeshes, and
 uses their error estimator functions to compute the vector of 
 elemental errors. The vector of errors is then passed to the
 submeshes'
 \code RefineableMesh::adapt(...) \endcode
 function which performs the actual mesh adaptation and
 entails the following steps:
 - Select all elements whose error exceeds the target maximum error 
   for refinement, provided the elements have not already been refined 
   to the maximum refinement level.
 - Select all elements whose error is smaller than the target minimum error 
   for unrefinement, provided
   - their siblings (which can be identified via (Quad)Tree procedures)
     can also be unrefined
   .
   and
   - the unrefinement would not coarsen the mesh beyond the minimum
     refinement level.
   .
 - Now loop over all elements in the mesh (traversing through the leaves
   of the \c QuadTreeForest) and split those elements that are targeted for
   refinement. This involves the following steps: 
   - Create new \c RefineableElements of the same type as the
     father element.
   - Create new \c QuadTrees --- as in the original setup, we 
     pass the pointers to the newly created \c RefineableElements to the
     \c QuadTree constructors to establish the association between each
     \c QuadTree and its \c RefineableElement. 
   - Declare the newly created \c QuadTrees to be the sons of the
     current \c QuadTree. This transforms the current \c QuadTree into a 
     \e "non-leaf node" in the \c QuadTreeForest.
   Note that the \c RefineableElement is not deleted 
   when it is split -- it retains its full functionality 
   (e.g. its pointers to its \c Nodes, etc).  This ensures that the element 
   is fully functional should its sons become scheduled for 
   unrefinement at a later stage. Note that in cases when the \c Nodes are not
 uniformly-spaced, certain \c Nodes in the father will not be
 used by the sons. These \c Nodes will be marked as obsolete and deleted
 from the \c Mesh. The pointers to these \c Nodes must be set to NULL in
 the father element, but this cannot be done until after the hanging
 node procedures have been completed, see below.
 - The newly-created elements now exist (and are accessible via
   the leaves of the \c QuadTreeForest) but they have not been `built'
   i.e. they do not have pointers to \c Nodes etc. We now 
   loop over the \e "leaf nodes" in the \c QuadTreeForest and execute
   the virtual function
   \code RefineableElement::build() \endcode 
   for all newly created elements. In the specific case of a 2D
   \c QuadMesh, the \c RefineableQElement<2>::build() function will be called.
   [\b Note: Elements that have not been built yet are identified
   by the fact that the entries in their \c Node_pt vector
   point to NULL. All other elements are ignored by the 
   \c RefineableElement::build() function.] \n
   The \c RefineableElement::build() function
   establishes the element's pointers to its \c Nodes and creates
   new \c Nodes as and when required: some \c Nodes will already have existed 
   in the old mesh; some new \c Nodes might already have been created
   by a neighbouring element, etc. If a new \c Node needs to 
   be created, it is allocated with the element's 
   \c FiniteElement::construct_node(...) or 
   \c FiniteElement::construct_boundary_node(...) functions. 
   By default, the 
   current and previous positions
   of the new \c Nodes are determined via the father element's
   geometric mapping. However, rather than referring directly to
   \c QElement::interpolated_x(...), we determine the position
   with
   \code FiniteElement::get_x(...) \endcode 
   which determines the nodal positions based on the father element's 
   macro-element representation if it exists; see section \ref MacroElements
   for a discussion of macro elements for mesh refinement in
   domains with curvilinear boundaries. \n
   By default, all values at a newly created \c Node are free (not pinned).
   If a new \c Node is located on the edge of the father element,
   we apply the same boundary conditions (if any) that apply along 
   the father element's edge. \n
   If a \c Node lies on a \c Mesh boundary, we add it to the \c Mesh 's storage
   scheme for \c BoundaryNodes. \n
   Finally, the values (and the history values) at the newly
   created \c Nodes must be assigned. This is done by using 
   the interpolated values from the father element. Since the way in which
   values are interpolated inside an element is element-specific 
   (e.g. in Taylor-Hood Navier-Stokes elements, different interpolations 
   are used for the pressure and for the velocities), 
   interpolated values are obtained from a call to the father element's
   \code  RefineableQElement<2>::get_interpolated_values(...) \endcode 
   function which returns the vector of interpolated values (or history 
   values) at a given local coordinate. This pure virtual 
   function must be 
   implemented for every specific \c RefineableElement. \n
   At this point, the generic steps in the build process are
   completed, but many particular \c RefineableElements now require further
   build operations. For instance, in Crouzeix-Raviart Navier-Stokes
   elements, the pressure interpolation is not based on nodal
   values but on internal \c Data which must be suitably initialised.
   For this purpose, we provide the interface
   \code RefineableElement::further_build(...) \endcode 
   which is executed at the end of  \c RefineableElement::build(...) 
   and can be used to perform any element-specific further build
   operations.
 - All new \c Nodes and elements have now been created.
   In the course of the mesh refinement, some 
   of the previously existing \c Nodes that are (still) marked as hanging
   might have become non-hanging. Therefore, we now 
   update the hanging nodes' values and coordinates 
   so that their entries
   are consistent with their current hanging-node constraints and
   then reset their hanging-node status to non-hanging. Finally, we 
   free (unpin) their nodal values. The hanging-node status 
   of all \c Nodes will be re-assessed later, when the de-refinement 
   phase is completed.
 - Now we loop over \b all \e "nodes" in the 
   \c QuadTreeForest. If the sons of any \e "non-leaf node" in the
   \c QuadTreeForest are scheduled for de-refinement, we merge them
   into their father element. This entails the following steps:
   - First we execute the father element's 
     \code RefineableElement::rebuild_from_sons() \endcode
     member function, which can be used, e.g., to determine 
     suitable values for the father element's internal \c Data from
     the values of its sons. In addition, if any of the father's
     \c Nodes have been deleted during refinement, they must be
     re-created during the merge procedure.
   - Next, we 'unbuild' the son elements by marking those of its \c Nodes
     that do not exist in their father element as obsolete (this
     classification can later be over-ruled by other elements that
     need to retain the \c Node).
   - Now we delete the son \c RefineableElements and the
     associated \c QuadTrees and empty the father element's
     vector of pointers to its sons. This (re)turns the father element
     into a fully functional element. 
   .
 - Next, we empty the \c Mesh::Element_pt vector 
   and refill it with the currently active elements which are accessible via
   the \c QuadTreeForest 's \e "leaf nodes". 
 - Now we loop over all elements in the \c Mesh::Element_pt 
   vector and mark their \c Nodes as non-obsolete.
 - We then update the \c Nodes hanging node status and adjust
   the nodal positions and values of the hanging nodes to make them
   consistent with the current hanging-node constraints.
 - We loop over all non-leaf elements in the \c QuadTreeForest
   and call their \c deactivate_object() function, which sets
   \c FiniteElement::Node_pt[n] to zero for any obsolete \c Nodes.
   Any \c Nodes in the \c Mesh::Node_pt vector that are still labelled
   as obsolete are truly obsolete and are deleted by calling
   \code Mesh::prune_dead_nodes() \endcode
 - Finally, in order to facilitate dump and restart operations we
   sort the \c Nodes into a standard order using 
   \code Mesh::reorder_nodes() \endcode

 \section Macro_element_meshes Mesh adaptation in domains with curved boundaries
 
 The mesh refinement procedures outlined above are perfectly adequate
 for meshes in polygonal domains. In such meshes the generation of
 the new nodal positions and the transfer of the solution from the
 old to the new mesh can be performed by simple interpolation, using
 the `father' element's geometric mapping and shape functions. 
 However, in problems with curvilinear mesh boundaries we must
 ensure that the refined meshes provide a progressively more 
 accurate representation of the curvilinear domain boundary.

 To facilitate these steps, we now introduce the concept of 
 \c GeomObjects, \c Domains and \c MacroElements, which 
 allow a convenient and generic representation of domains that are 
 bounded by time-dependent and/or curvilinear boundaries.
 
 \subsection GeomObjects Geometric Objects
 
 Here are two examples of curvilinear boundaries
 that are frequently encountered in computations on moving domains:
 - In many problems the boundary  \f$\partial D\f$ of 
   the moving domain \f$D\f$ is given explicitly in terms of a
   position vector \f[ {\bf r}_{_{\partial D}}(\boldmath{\xi},t), \f]
   where \f$ t \f$ is the (continuous) time and the components of the 
   vector \f$ \boldmath{\xi} \f$  parametrise the boundary.
   We have \f$DIM({\bf r}_{_{\partial D}}) = DIM(\boldmath{\xi}) + 1\f$. 
   For instance, the surface of a cylinder \f$C\f$ with time-dependent 
   radius \f$R(t)\f$ can be represented as
   \f[ {\bf r}_{_{\partial C}} = \left( \begin{array}{c} 
                         R(t) \cos(\xi_2) \\
                         R(t) \sin(\xi_2) \\ 
                         \xi_1 \\ 
                         \end{array} \right). \f]
 - In fluid problems with free surfaces or in fluid-structure
   interaction (FSI) problems, the domain boundary might have to be determined
   as part of the solution. In such cases, the boundary will have some
   computational representation. For instance, in an FSI computation
   in which a shell structure bounds the fluid domain, each shell 
   finite element defines a small part of the fluid domain boundary and
   the shell element's local coordinates provide a parametrisation
   of some part of the boundary in a form similar to the one 
   shown above.

 The common feature of these two examples is that, in both cases, the
 boundary is represented by a parametrised position vector.
 The \c GeomObject base class provides an abstract interface for such
 situations. In its most basic form, a `geometric object'
 simply provides a parametrisation of its shape by
 implementing the \c GeomObject's pure virtual member function
 \code
 GeomObject::position(xi,r)
 \endcode
 which computes the position vector \c r at the coordinates
 specified by the vector \c xi. \c GeomObject also provides 
 a large number of additional interfaces in the form of
 (broken) virtual functions. The most important of these is the
 time-dependent version of the above function
 \code
 GeomObject::position(xi,t,r)
 \endcode
 which computes the position vector \c r at the coordinates
 specified by the vector \c xi at the \b previous \b discrete
 time level \c t. We follow the usual convention that 
 - for \c t=0 the vector \c r is the position vector at the current time,
   <CODE> time = Time::time() = Time::time(0) </CODE>
 - for \c t=1 it represents the position at the previous (discrete) 
   time level \c t, i.e. at the continuous time 
   <CODE> time = Time::time() - Time::dt() = Time::time(1) </CODE>
 - etc. 
 .
 By default, the virtual member function \c GeomObject::position(xi,t,r)
 calls the steady version  \c GeomObject::position(xi,r), so it only 
 needs to be overloaded for genuinely time-dependent geometric objects 
 (by default, the
 code execution terminates if the time-dependent version is called 
 for \f$ {\tt t} \ne 0; \f$ the function needs to be overloaded if this is 
 not appropriate).

 Further virtual member functions provide interfaces for the determination
 of the first and second spatial and temporal derivatives of the 
 position vector, allowing the computation of boundary velocities,
 accelerations and curvatures, etc. These interfaces are provided as
 broken virtual functions, so they only need to be overloaded if/when the
 functionality is actually required in a particular application.

 Typically, the shape of a \c GeomObject depends on a certain number of
 parameters (in the above examples, the radius \f$ R(t) \f$
 of the cylinder and the displacements of the shell element, respectively)
 which can be unknowns in the problem. We therefore store these parameters
 as (geometric) \c Data, whose values can be pinned or free.

 \subsection MacroElements Domains and Macro Elements
 
 For the purposes of mesh generation, we represent curvilinear 
 domains by objects that are derived from the base class \c Domain 
 and use \c GeomObjects to represent the curvilinear boundaries. 
 For instance, the \c QuarterCircleSectorDomain sketched
 in the figure below is bounded by the geometric object \c Ellipse 
 that parametrises the domain's curved boundary, shown in red.
 
 Consider the coarse discretisation of the domain 
 shown in the Fig. (a) and assume that element 2 (a four-node quad 
 element from the \c QElement family) is scheduled for 
 refinement into four son elements. As discussed above, by default the
 \c Nodes
 of the son elements are established/created as follows:
 - If a \c Node already exists in the father element we store the pointer
   to the existing \c Node in the son element's \c Node_pt vector.
 - If a \c Node needs to be created, we determine its position from the 
   geometric mapping of the father element. Thus the five new \c Nodes
   that need to be created when element 2 is refined,
   are placed at their father element's (i.e. element 2's) local 
   coordinates (0,0), (0,1), (0,-1), (1,0) and (-1,0). The
   father element's
   \code RefineableQElement<2>::get_x(...) \endcode
   determines the nodal position via a call to 
   \c QElement::interpolated_x(...).
   Similarly, the nodal values of the new \c Nodes are determined by
   using the interpolated values in the father element. 

 \image html MacroElementSketch.gif "Illustration of mesh refinement with and without macro elements"

 For the element 0, this procedure would be
 perfectly adequate, as the domain boundary after refinement 
 would (still) be captured exactly.
 However, when refining element 2 by this procedure, the 
 new \c Node on the boundary is positioned on the straight line 
 between the two boundary \c Nodes in the father
 element and not on the curved boundary itself, as shown in Fig. (b).
 Hence repeated mesh refinement will not lead to a better 
 representation of the domain geometry and preclude convergence 
 to the exact solution. 

 To overcome this problem, objects of type \c Domain
 provide a decomposition of the domain into a number
 of so-called \c MacroElements, as sketched in Fig. (c). Boundaries
 of the \c MacroElements are either given by (parts of) the (exact) 
 curvilinear boundaries (as specified by the \c Domain's \c GeomObjects)
 or by (arbitrary and usually straight) internal edges (or, in 3D, faces).
 In 2D, \c MacroElements provide a mapping \f$ {\bf r}(S_0,S_1) \f$
 from a local coordinate system \f$ (S_0, S_1) \f$,  with
 \f$ S_i \in [-1..1]^2 \f$, to the points inside the \c MacroElement
 via their member function
 \code MacroElement::macro_map(S,r) \endcode
 The mapping needs to be chosen such that for \f$ S_1\in [-1..1]\f$ 
 the position vector \f$ {\bf r}(S_0=-1,S_1) \f$  sweeps
 along the `southern' edge of the \c MacroElement, etc.; see Fig. (e).
 The form of the macro-element mapping is obviously not unique and it
 depends on the \c MacroElement's topology. The class 
 \c QMacroElement<2> provides a mapping that is suitable for 2D quadrilateral 
 elements and can therefore be used with \c RefineableQElement<2> s. 

 The constructors for objects of
 type \c Domain typically require a vector of pointers to the 
 \c GeomObjects that define its boundaries. The \c Domain constructor
 then usually employs function pointers to the 
 \c GeomObject::position(...) function to define the \c MacroElement 
 boundaries. Once built, the \c MacroElements are accessible 
 via the member function \code Domain::macro_element_pt(...). \endcode

 We illustrate the macro-element based mesh generation and adaptation 
 process for the case of \c RefineableQuadMesh meshes. 
 Assume that the domain 
 is represented by an object of type \c Domain. 
 We build a coarse initial mesh which contains as many
 \c RefineableQElement<2>s as there are \c QMacroElements in the \c Domain.
 We associate each \c RefineableQElement<2> with one of the \c QMacroElements
 by storing a pointer to the \c QMacroElement in 
 \c FiniteElement::Macro_elem_pt. Next, we use the \c QMacroElements'
 macro map to determine the \c RefineableQElement<2>'s nodal positions such 
 that, e.g., the \c RefineableQElement<2>'s SW node is placed at the 
 \c QMacroElement 's local coordinates \f$(S_0,S_1)=(-1,-1)\f$, etc.
 The fraction of the \c QMacroElement that is spanned by each
 \c RefineableQElement<2> is represented by the maximum and minimum
 values of the \c QMacroElement's local coordinates; the 
 \c RefineableQElement<2>
 constructor initially sets these values to the defaults of +1.0 and -1.0, 
 respectively, indicating that the \c RefineableQElement<2> spans the
 entire area of the corresponding \c QMacroElement.
 
 With this additional information, we modify the  
 \c RefineableQElement<2>::build() process as follows:
 - After creating the son elements, we set their 
   \c FiniteElement::Macro_elem_pt to that of their father.
 - We adjust the son's maximum and 
   minimum \c MacroElement coordinates so that they span the 
   appropriate fraction of the \c MacroElement.
   For instance, for the SW son element of element 2 in the above
   sketch, we set
   \f$ S_0^{min}=-1, \  S_0^{max}=0, \ S_1^{min}=-1, \  S_1^{max}=0 \f$.
   Should this element get refined again, we set its NW son element's
   coordinates to \f$ S_0^{min}=-1, \  S_0^{max}=-0.5,
   \ S_1^{min}=-0.5, \  S_1^{max}=0 \f$, etc.
 - The nodal positions of newly created \c Nodes are
   determined via calls to the 
   father element's \c RefineableQElement<2>::get_x(...) function.
   If the father element is associated with a \c MacroElement
   (indicated by a non-NULL \c RefineableQElement<2>::Macro_elem_pt pointer),
   this function does not 
   refer to \c QElement::interpolated_x(...) (i.e. the FE mapping)
   but places the new \c Node at the appropriate point inside the 
   father's \c MacroElement. This ensures that \c Nodes that are created 
   on the \c Mesh boundaries get placed on the \c Domain boundary,
   as shown in Fig. (d).
 - In time-dependent, moving mesh problems, the history of the new \c Nodes'
   position is established by calls to the time-dependent version of the
   \c MacroElement::macro_map(...) function, which in turn refers to
   the time-dependent \c GeomObject::position(...) function of the 
   \c GeomObjects that define the \c Domain boundaries. Hence for all new 
   \c Nodes, \c Node::x(t,i) for \c t>0, returns the position the \c Node
   would have had if it had already existed at the previous time level t. 
 - We retain the original procedure for initialising the current and 
   previous values at the new \c Nodes and continue to determine them by 
   interpolation from the father element, based on the father 
   element's local coordinates. (We cannot determine the function 
   values at the exact new nodal positions because new \c Nodes can be located 
   outside their father elements.)

 \subsection moving_mesh Macro-element-based node updates in moving mesh problems 

  Once a \c Mesh is associated with a \c Domain, the function
  \c Mesh::node_update() updates the nodal positions in response
  to a change in the shape of the \c GeomObjects that define the
  \c Domain boundaries. [Note: This function updates all nodal positions
  first and then updates the positions of the hanging nodes
  to subject them to the hanging node constraints.] 
  Alternative procedures for the update of the nodal positions in 
  response to changes in the domain boundary are implemented
  in the \c AlgebraicMesh, \c SpineMesh and \c SolidMesh classes. 


<hr>
<hr>
\section pdf PDF file
A <a href="../latex/refman.pdf">pdf version</a> of this document is available.
**/

