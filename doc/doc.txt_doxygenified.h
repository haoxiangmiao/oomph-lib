/**

\mainpage
\htmlonly

<div id="carousel-example-generic" class="carousel slide" data-ride="carousel" >
  <!-- Indicators -->
  <!--<ol class="carousel-indicators">
    <li data-target="#carousel-example-generic" data-slide-to="0" class="active"></li>
    <li data-target="#carousel-example-generic" data-slide-to="1"></li>
    <li data-target="#carousel-example-generic" data-slide-to="2"></li>
  </ol>-->

  <!-- Wrapper for slides -->
  <div class="carousel-inner" role="listbox">
    <div class="item active">
      <img class="img-rounded" src="../figures/slide1.jpg" alt="">
    </div>
    
    <div class="item">
      <img class="img-rounded" src="../figures/slide2.png" alt="">
    </div>
  
    <div class="item">
      <img class="img-rounded" src="../figures/slide3.png" alt="">
    </div>
  </div>
   

  <!-- Controls -->
  <a class="left carousel-control" href="#carousel-example-generic" role="button" data-slide="prev">
    <span class="icon-prev" aria-hidden="true"></span>
    <span class="sr-only">Previous</span>
  </a>
  <a class="right carousel-control" href="#carousel-example-generic" role="button" data-slide="next">
    <span class="icon-next" aria-hidden="true"></span>
    <span class="sr-only">Next</span>
  </a>
   
</div>

  <div style="min-height:800px;">
  <ul class="nav nav-tabs">
  <li class="active"><a href="#about" data-toggle="tab">What is it?</a></li>
  <li><a href="#features" data-toggle="tab">Features</a></li>
  <li><a href="#get" data-toggle="tab">Get it</a></li>
  <li><a href="#use" data-toggle="tab">Get Started</a></li>
  </ul>
  <section class="tab-content" style=
  "border:1px solid #ddd; border-top:none; padding:20px; margin-bottom:100px;">
  
  
  
  <article class="tab-pane active" id="about">
  <h1>Welcome to the oomph-lib homepage</h1>
  <p><code>oomph-lib</code> is an object-oriented, open-source
  finite-element library for the simulation of multi-physics
  problems. It is developed and maintained by <a href=
  "http://www.maths.man.ac.uk/~mheil">Matthias Heil</a> and <a href=
  "http://www.maths.man.ac.uk/~ahazel">Andrew Hazel</a> of the
  <a href="http://www.maths.manchester.ac.uk/">School of
  Mathematics</a> at <a href="http://www.manchester.ac.uk/">The
  University of Manchester</a>, along with many <a href=
  "../people/html/index.html">other contributors</a>.</p>
  <p>The latest version of the library is 1.0, 
  <script>
        // This odd/hacky construction extracts the svn information automatically
         var revString = "$LastChangedRevision$";
         var dateString = "$LastChangedDate$";
         
         //Revision number should never be more than 5 digits
         revString = revString.substr(revString.indexOf(":")+1,5).trim();
         
         // The string should always be the same length
         var timeString = dateString.substr(28,8);
         
         dateString = dateString.substr(dateString.indexOf("(")+1, dateString.indexOf(")") - dateString.indexOf("(")-1);
         
         document.write("revision number ");
         document.write(revString);
         document.write(" and was committed on ");
         document.write(dateString);
         document.write(".");
  </script> See the <a href="../change_log/html/index.html">change
  log page</a> for an overview of the changes made since the previous
  release.</p>
  <h2>What is it [and what is it not]?</h2>
  <code>oomph-lib</code> <b>is</b>:
  <ul>
  <li>an object-oriented, open-source finite-element library for the
  simulation of multi-physics problems.</li>
  </ul>
  <code>oomph-lib</code> <b>is not</b>:
  <ul>
  <li>a GUI-based, mouse-driven "package".</li>
  </ul>
  You must write your own C++ driver codes to specify your problem,
  using <code>oomph-lib</code>'s high-level objects. Once the problem
  is formulated, the <code>main</code> function can be as simple as
  this:
  <div class="fragment">
  <div class="line">   main()</div>
  <div class="line">   {</div>
  <div class="line">     <span class="comment">// Create the problem object</span></div>
  <div class="line">     <span class="keywordtype">MyProblem</span> problem;</div>
  <div class="line">     </div>
  <div class="line">     <span class="comment">// Solve the problem, using oomph-lib&#39;s default Newton solver</span></div>
  <div class="line">     problem.newton_solve();</div>
  <div class="line">   }</div>
  </div>
  <code>oomph-lib</code> provides the state of the art infrastructure
  for the problem formulation and solution. The library contains a
  large number of elements, meshes, timesteppers, solvers, etc. which
  can be combined to solve any problem.
  <p>The library is supplied with extensive on-line documentation
  which includes:</p>
  <ul>
  <li>
  <h4><a href="../intro/html/index.html"><b>Theory: Finite Elements
  -- from procedural to object-oriented</b></a></h4>
  This provides a relatively brief, constructive "top-down"
  introduction to the maths of the finite element method, and
  explains its object-oriented implementation in
  <code>oomph-lib</code>.</li>
  <li>
  <h4><a href="../the_data_structure/html/index.html"><b>The Data
  Structure</b></a></h4>
  A bottom-up discussion of <code>oomph-lib's</code> overall data
  structure, including a complete, cross-referenced index of all
  <code>oomph-lib's</code> classes (generated by Doxygen).</li>
  <li>
  <h4><a href="../quick_guide/html/index.html"><b>(Not-So-)Quick
  Guide</b></a></h4>
  The <a href="../quick_guide/html/index.html">(Not-So-)Quick
  Guide</a> provides a "quick" introduction on how to create new
  instances of <code>oomph-lib's</code> fundamental objects:
  <code>Problems</code>, <code>Meshes</code>, and
  <code>Elements</code>.</li>
  <li>
  <h4><a href="../example_code_list/html/index.html"><b>Example
  Codes/Tutorials</b></a></h4>
  We provide a large number of verified and documented example
  problems.
  <ul>
  <li style="margin-top:10px;">The example problems are arranged in
  order of increasing complexity and can be used as chapters in a
  self-study course. Comments and exercises are provided to encourage
  further exploration of <code>oomph-lib's</code> capabilities.</li>
  <li style="margin-top:10px;">The individual examples are as
  self-contained as possible and can be used as quick guides to
  particular features of <code>oomph-lib</code>. ("How do I solve
  time-dependent problems?"; "How do I enable spatial adaptivity?";
  "How do I write my own meshes?"; etc.)</li>
  <li style="margin-top:10px;">The examples codes should be regarded
  as templates for your own driver codes: Examine the <a href=
  "../example_code_list/html/index.html">list of examples</a> and try
  to find one that resembles the problem that you wish to solve. For
  most "classical" problems (e.g. the solution of the Navier-Stokes
  equations with standard boundary conditions), very few
  modifications are required (e.g. adjusting the mesh to a different
  domain, changing the boundary and/or initial conditions, etc). If
  you wish to solve a non-standard problem, or one for which
  <code>oomph-lib</code> does not (yet) provide elements or meshes,
  it is easy to write your own, following the detailed instructions
  provided in the documentation.</li>
  </ul>
  </li>
  </ul>
  </article>
  
  
  
  <article class="tab-pane" id="features">  
    <h1>Features</h1>
    <ul>
    <li>Large number of fully-developed and carefully-validated element
    types for the solution of:
    <ul>
    <li>The Poisson equation</li>
    <li>The unsteady heat equation</li>
    <li>The linear wave equation</li>
    <li>The advection-diffusion equation</li>
    <li>The Navier-Stokes equations, incl. free-surfaces, in Cartesian
    and axisymmetric co-ordinates.</li>
    <li>The equations of large-displacement solid mechanics, based on
    the principle of virtual displacements in cartesian and
    axisymmetric geometries; with arbitrary constitutive
    equations.</li>
    <li>Shell and beam elements, based on a geometrically non-linear
    formulation.</li>
    <li>...and many others.</li>
    </ul>
    [<b>Note:</b> Most elements are implemented in a
    dimension-independent formulation]</li>
    <li>It is easy to formulate new elements for problems that are not
    included in the above list.</li>
    <li>It is easy to combine any of the above elements to
    "multi-physics" elements, allowing e.g.
    <ul>
    <li>the solution of fluid-structure interaction problems</li>
    <li>the simulation of unsteady heat conduction in an elastic body
    that undergoes large displacements</li>
    <li>free-surface problems</li>
    <li>etc.</li>
    </ul>
    </li>
    <li>Parallel processing capabilities.</li>
    <li>Full spatial and temporal adaptivity. All elements listed above
    support quad-tree/octree-based mesh refinement procedures and
    adaptive timestepping.</li>
    <li>A wide range of meshes; new meshes can easily be added to the
    library. <code>oomph-lib</code> can also use meshes that are
    generated by third-party (commercial) mesh generators.</li>
    <li>The ability to solve problems in domains with time-dependent,
    curvilinear boundaries.</li>
    <li><code>oomph-lib</code> treats all problems as non-linear
    problems and uses Newton's method as the default non-linear solver.
    Within this framework, linear problems are special cases for which
    the Newton iteration converges in one step. The linear systems
    arising during the Newton iteration can be solved with a variety of
    direct and iterative solvers. Continuation methods (Keller's
    arclength method and displacement-control for solid mechanics
    problems) are implemented.</li>
    <li>A large number of <a href=
    "../example_code_list/html/index.html">fully documented example
    codes.</a> All examples include an introduction to the
    mathematical/physical problem, a detailed discussion of the driver
    code, a discussion of the results (incl. comparisons against
    analytical solutions where appropriate) and exercises and
    comments.</li>
    <li>The library has extensive self-test facilities. It can be
    compiled with a PARANOID flag switched on or off. If the flag is
    set during compilation, the code performs a large number of
    self-tests during its execution. If things go wrong, code execution
    terminates gracefully and diagnostic output is generated to help
    pinpoint the problem. Of course, the tests introduce an additional
    run-time overhead, so can be switched off in production runs.
    During development, range checking on all array-based objects,
    (vectors, matrices, tensors, etc) can be enabled separately by
    using the RANGE_CHECKING flag, but introduces considerable run-time
    overhead.</li>
    </ul>
  </article>
  
  
  <article class="tab-pane" id="get">
  <h1>How do I get the code?</h1>
  <p>The latest version of the library is 1.0, 
  <script>
        // This odd/hacky construction extracts the svn information automatically
         var revString = "$LastChangedRevision$";
         var dateString = "$LastChangedDate$";
         
         //Revision number should never be more than 5 digits
         revString = revString.substr(revString.indexOf(":")+1,5).trim();
         
         // The string should always be the same length
         var timeString = dateString.substr(28,8);
         
         dateString = dateString.substr(dateString.indexOf("(")+1, dateString.indexOf(")") - dateString.indexOf("(")-1);
         
         document.write("revision number ");
         document.write(revString);
         document.write(" and was committed on ");
         document.write(dateString);
         document.write(".");
  </script> See the <a href="../change_log/html/index.html">change
  log page</a> for an overview of the changes made since the previous
  release.</p>
  <h2>Downloads</h2>
  <p>We provide <a href="../the_distribution/html/index.html">gzipped
  tar files</a> of the library. We also provide read-only access to
  the <a href="../subversion/html/index.html">subversion
  repository</a> of the oomph-lib project. For more information see
  the <a href="../the_distribution/html/index.html">installation
  guide</a>. Note that the <a href=
  "../the_distribution/html/index.html">gzipped tar files</a> , the
  latest version of the library available via the public repository
  and the webpage at <a href=
  "http://www.oomph-lib.org">http://www.oomph-lib.org</a> are
  automatically synchronised.</p>
  <h2>Prerequisites</h2>
  <p><code>oomph-lib</code> has been/is primarily being developed in
  Linux so it is well tested there. However several people
  successfully run it on Windows and Mac OS X. In order to minimise
  the dependence on external libraries we include "frozen" versions
  of certain external libraries (<a href=
  "http://www-users.cs.umn.edu/~karypis/metis/">METIS</a>, <a href=
  "http://crd.lbl.gov/~xiaoye/SuperLU">SuperLU</a>, ...) with our
  distribution. The idea is that you should only have to unpack,
  build and install one distribution to produce the fully compiled
  library, the html-based documentation and working example
  codes.</p>
  For this purpose you <b>must have</b>:
  <ul>
  <li>A computer with a linux (or unix) operating system. [<b>Note
  for windows users:</b> We believe that <code>oomph-lib</code> can
  be installed in a windows environment, using <a href=
  "http://www.cygwin.com/">cygwin</a>, though we haven't tried this
  for a long time.]</li>
  <li>Compilers for C++, C and Fortran77. The open-source <a href=
  "http://gcc.gnu.org/">GNU gcc compiler suite</a> version 4.8.4 or
  later is fine. [ <b>Note:</b> The Fortran77 and C compilers are
  only required to compile some external libraries --
  <code>oomph-lib</code> itself is written entirely in C++.]</li>
  </ul>
  The following programs are helpful but not essential:
  <ul>
  <li>GNU's <a href=
  "http://www.gnu.org/software/autoconf/">autoconf</a>, <a href=
  "http://www.gnu.org/software/automake/">automake</a> and <a href=
  "http://www.gnu.org/software/libtool/">libtool</a> are required if
  you wish to add additional features to your local copy of
  <code>oomph-lib</code>; see <a href=
  "../the_distribution/html/index.html">the <code>oomph-lib</code>
  download page</a> for more details.</li>
  <li><a href="http://www.python.org/download/">python</a> is needed
  to analyse the results of the extensive self-tests that may be
  performed at the end of the installation to verify that the build
  process was completed successfully. If python is not available, the
  self-test will compile and run all test codes but it will not be
  possible to verify the correctness of the results.</li>
  <li><a href="http://www.doxygen.org">doxygen</a> is needed to build
  a local copy of <code>oomph-lib</code>'s extensive online
  documentation. If doxygen is not installed on your system you can
  consult the latest version of the online documentation on <a href=
  "http://www.oomph-lib.org">the <code>oomph-lib</code>
  homepage.</a></li>
  </ul>
  The <code>oomph-lib</code> distribution is built under <a href=
  "http://www.gnu.org/software/autoconf/">autoconf</a> / <a href=
  "http://www.gnu.org/software/automake/">automake</a> control,
  making the installation completely straightforward. We provide two
  main installation mechanisms:
  <ul>
  <li><code>oomph-lib</code> can be built/installed/tested with GNU's
  standard <code>configure; make; make install; make check</code>
  procedure.</li>
  <li>We also provide a customised build script,
  <code>autogen.sh</code>, that guides the "non-expert" user through
  the installation. The script facilitates the specification of
  various build options, such as optimisation levels etc.</li>
  </ul>
  Subject to the minimum requirements, listed above, you should be
  able to install and use our libraries "anywhere". However, as
  anybody who has ever ported any code to a different platform knows,
  things are rarely that simple -- even with <a href=
  "http://www.gnu.org/software/autoconf/">autoconf</a>, <a href=
  "http://www.gnu.org/software/automake/">automake</a> and <a href=
  "http://www.gnu.org/software/libtool/">libtool</a>... Having said
  that, we have successfully installed and tested our installation on
  the following platforms:
  <ul>
  <li>Ubuntu (version 14.04) with gcc compilers (version 4.8.4). The
  tests included the MPI part of the library, which we compiled with
  <a href="https://www.open-mpi.org/">OpenMPI</a> (version
  1.6.5).</li>
  <li>Redhat (version 4.4.7-3) with gcc compilers (version 4.4.7) The
  tests included the MPI part of the library, which we compiled with
  <a href="https://www.open-mpi.org/">OpenMPI</a> (version
  1.5.4).</li>
  <li> Ubuntu (version 14.04) with Clang compilers (version 3.5) </li>
  <li> Arch Linux (05-2016) with gcc compilers (version 6.1.1) </li>
  <li> Ubuntu (version 16.04) with gcc compilers (version 5.3.1) </li>       
  <li> Debian Jessie (gcc 4.9)</li>
 </ul>

  <h2>Preparing a ubuntu box for <code>oomph-lib</code></h2>
  Typically, when we install <code>oomph-lib</code> on a machine with
  a new (default) installation of ubuntu, we install at least the
  following additional packages (here listed with full apt-get syntax,
  so you can cut and paste):
  <br>
  <br>
  <tt>
apt-get install subversion
  </tt>
  <br>
  <tt>
apt-get install make
  </tt>
  <br>
  <tt>
apt-get install automake
  </tt>
  <br>
  <tt>
apt-get install libtool
  </tt>
  <br>
  <tt>
apt-get install autoconf
  </tt>
  <br>
  <tt>
apt-get install doxygen
  </tt>
  <br>
  <tt>
apt-get install gfortran
  </tt>
  <br>
  <tt>
apt-get install g++
  </tt>
  <br>
  <br>
  ...or, if you want it all in one go (particularly useful if you have
  to prefix the apt-get with sudo): 
  <br>
  <br>
  <tt>
apt-get install subversion make automake libtool autoconf doxygen
  gfortran g++
  </tt>
  </article>
  
  
  
  <article class="tab-pane" id="use">
  <h1>How do I get started?</h1>
  <code>oomph-lib</code> contains a lot of code and a lot of
  documentation. How to get started obviously depends on your
  background: Are you familiar with the finite element method? How
  good is your knowledge of C++? Etc. Here are some possible
  "routemaps" around the library:
  <ul>
  <li><b>You are familiar with the finite element method and have a
  fairly good knowledge of C++</b>
  <ul>
  <li>Have a look through the <a href=
  "../example_code_list/html/index.html">list of example codes</a> to
  get a feeling for <code>oomph-lib</code>'s capabilities. Pick a
  problem that interests you and study the associated tutorial. Copy
  the driver code into your own directory and play with it.</li>
  <li>Once you have played with a few example codes, you may wish to
  to learn more about <code>oomph-lib</code>'s <a href=
  "../the_data_structure/html/index.html">overall data structure,</a>
  or find out <a href="../optimisation/html/index.html">how to
  optimise the library</a> for your particular application.</li>
  </ul>
  </li>
  <li><b>You have never used finite element methods but have a fairly
  good knowledge of C++</b>
  <ul>
  <li>Study the <a href="../intro/html/index.html">"Top Down"
  introduction</a>. This document includes a "low tech" overview of
  the mathematical/theoretical background and contrasts procedural
  implementations of the finite element method with the
  object-oriented approach adopted in <code>oomph-lib</code>.</li>
  <li>Consult the <a href=
  "../quick_guide/html/index.html">(Not-So-)Quick-Guide</a> to learn
  how to construct basic <code>oomph-lib</code> objects for your
  problem: <code>Problems</code>, <code>Meshes</code>,
  <code>FiniteElements</code>, etc.</li>
  <li>Continue with the steps suggested above.</li>
  </ul>
  </li>
  <li><b>You have never used finite element methods and are a
  newcomer to C++</b>
  <ul>
  <li>Buy <a href="http://www.math.wayne.edu/~yang/">Daoqi Yang</a>
  brilliant book <a href=
  "http://www.springeronline.com/sgw/cda/frontpage/0,11855,4-40007-22-2105335-0,00.html?changeHeader=true">
  C++ and Object-Oriented Numeric Computing for Scientists and
  Engineers.</a> Read it! Pretty much everything in this book is
  relevant for some parts of <code>oomph-lib</code>. You should at
  least understand:
  <ul>
  <li>The procedural aspects of C++ (basic types, functions and
  control structures).</li>
  <li>Namespaces.</li>
  <li>Classes (private, protected and public members; inheritance and
  multiple inheritance; virtual and pure virtual functions; base
  classes and derived classes; static and dynamic casts).</li>
  <li>Templates and template instantiations.</li>
  <li>The standard template library (STL).</li>
  </ul>
  </li>
  <li>Continue with the steps suggested above.</li>
  </ul>
  </li>
  </ul>
  </article>
  
  </section>
</div>

<!-- remove carousel from contents to allow it to span whole document -->
<!-- <script type="text/javascript">
$(".contents").removeClass("contents");
</script> -->

\endhtmlonly




<hr>
<hr>
\section pdf PDF file
A <a href="../latex/refman.pdf">pdf version</a> of this document is available.
**/

