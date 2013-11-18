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
//A header containing useful utility classes, functions and constants

//Include guard to prevent multiple inclusions of the header
#ifndef OOMPH_UTILITIES_HEADER
#define OOMPH_UTILITIES_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif


#ifdef OOMPH_HAS_MPI
//mpi headers
#include "mpi.h"
#endif

//Standard libray headers
#include<string>
#include<sstream>
#include<fstream>
#include<cmath>
#include<map>
#include<ctime>

//oomph-lib headers
#include "Vector.h"
#include "oomph_definitions.h"
#include "communicator.h"


namespace oomph
{

//==============================================
/// Namespace for error messages for broken
/// copy constructors and assignment operators
//==============================================
namespace BrokenCopy
{

 /// Issue error message and terminate execution
 extern void broken_assign(const std::string& class_name);
 
 /// Issue error message and terminate execution
 extern void broken_copy(const std::string& class_name);
 
}

//========================================
/// Namespace for mathematical constants
///
//=======================================
namespace MathematicalConstants
{
 extern double Pi;
}


//================================================================
/// Function-type-object to perform absolute comparison of objects.
/// Apparently this inlines better
//================================================================
template <class T>
class AbsCmp
{
public:

 /// Comparison. Are the values identical or not?
 bool operator()(const T &x, const T &y) const
  {
   return std::abs(x) < std::abs(y);
  }
};


// =================================================================
/// Conversion functions for easily making strings (e.g. for filenames - to
/// avoid stack smashing problems with cstrings and long filenames).
// =================================================================
namespace StringConversion
{
 
 /// \short Conversion function that should work for anything with
 /// operator<< defined (at least all basic types).
 template<class T> std::string to_string(T object)
 {
  std::stringstream ss;
  ss << object;
  return ss.str();
 }
 
 /// \short Convert a string to lower case (outputs a copy).
 std::string to_lower(const std::string& input);

 /// \short Convert a string to upper case (outputs a copy).
 std::string to_upper(const std::string& input);

 /// \short Split a string, s, into a vector of strings where ever there is
 /// an instance of delimiter (i.e. is delimiter is " " will give a list of
 /// words). Note that mutliple delimiters in a row will give empty
 /// strings.
 void split_string(const std::string &s, char delim, Vector<std::string> &elems);

 /// \short Split a string, s, into a vector of strings where ever there is
 /// an instance of delimiter (i.e. is delimiter is " " will give a list of
 /// words). Note that mutliple delimiters in a row will give empty
 /// strings. Return by value.
 Vector<std::string> split_string(const std::string &s, char delim);

}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////



//====================================================================
/// Namespace for global (cumulative) timings
//====================================================================
namespace CumulativeTimings
{

 /// (Re-)start i-th timer
 extern void start(const unsigned& i);

 /// Halt i-th timer
 extern void halt(const unsigned& i);

 /// Reset i-th timer
 extern void reset(const unsigned& i);

 /// Reset all timers
 extern void reset();

 /// Report time accumulated by i-th timer
 extern double cumulative_time(const unsigned& i);

 /// Set number of timings that can be recorded in parallel
 extern void set_ntimers(const unsigned& ntimers);

 /// Cumulative timings
 extern Vector<clock_t> Timing;

 /// Start times of active timers
 extern Vector<clock_t> Start_time;

}


 // ============================================================
 // Automatically checked casting functions (from boost)
 // ============================================================ 

 /// \short Runtime checked dynamic cast. This is the safe but slightly slower
 /// cast. Use it in any of these cases:
 /// - You aren't entirely sure the cast is always safe.
 /// - You have strange inheritence structures (e.g. the "Diamond of Death" in element inheritence).
 /// - Efficiency is not critical. 
 /// Note that if you just want to check if a pointer can be converted to
 /// some type you will need to use a plain dynamic_cast. Adapted from
 /// polymorphic_cast in boost/cast.hpp, see
 /// http://www.boost.org/doc/libs/1_52_0/libs/conversion/cast.htm for more
 /// details.
 template <class Target, class Source>
 inline Target checked_dynamic_cast(Source* x)
 {
  Target tmp = dynamic_cast<Target>(x);
#ifdef PARANOID
  if(tmp == 0)
   {
    throw OomphLibError("Bad cast.", OOMPH_CURRENT_FUNCTION,
                        OOMPH_EXCEPTION_LOCATION);
   }
#endif
  return tmp;
 }

 /// \short Checked static cast. Only use this cast if ALL of these are true:
 /// - You are sure that the cast will always succeed.
 /// - You aren't using any strange inheritence structures (e.g. the "Diamond of
 /// Death" in element inheritence, if you aren't sure just try compiling).
 /// - You need efficiency.
 /// Adapted from polymorphic_downcast in boost/cast.hpp, See
 /// http://www.boost.org/doc/libs/1_52_0/libs/conversion/cast.htm for more
 /// details.
 template <class Target, class Source>
 inline Target checked_static_cast(Source* x)
 {
#ifdef PARANOID
  // Check that the cast will work as expected.
  if(dynamic_cast<Target>(x) != x)
   {
    throw OomphLibError("Bad cast.", OOMPH_CURRENT_FUNCTION,
                        OOMPH_EXCEPTION_LOCATION);
   }
#endif
  return static_cast<Target>(x);
 }

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//====================================================================
/// Timer 
//====================================================================
class Timer
{

  public:

 /// Constructor: Specify number of timers
 Timer(const unsigned& n_timer)
  {
   set_ntimers(n_timer);
  }
 
 /// (Re-)start i-th timer
 void start(const unsigned& i)
  {
   Start_time[i]=clock();
  }
 
 /// Halt i-th timer
 void halt(const unsigned& i)
  {
   Timing[i]+=clock()-Start_time[i];
  }
 
 /// Report time accumulated by i-th timer
 double cumulative_time(const unsigned& i)
  {
   return double(Timing[i])/CLOCKS_PER_SEC;
  }
 
 /// Reset i-th timer
 void reset(const unsigned& i)
  {
   Timing[i]=clock_t(0.0);
  }
 
 /// Reset all timers
 void reset()
  {
   unsigned n=Timing.size();
   for (unsigned i=0;i<n;i++)
    {
     Timing[i]=clock_t(0.0);
    }
  }
 
 /// Set number of timings that can be recorded in parallel
 void set_ntimers(const unsigned& ntimers)
  {
   Timing.resize(ntimers,clock_t(0.0));
   Start_time.resize(ntimers,clock_t(0.0));
  }
 
  private:
 
 /// Cumulative timings
 Vector<clock_t> Timing;
 
 /// Start times of active timers
 Vector<clock_t> Start_time;

};



//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


//======================================================================
// Namespace for black-box FD Newton solver.
//======================================================================
namespace BlackBoxFDNewtonSolver
{


// Function pointer for function that specifies residuals: The arguments 
// are: Parameters, unknowns, residuals
typedef void (*ResidualFctPt)(const Vector<double>& parameters,
                              const Vector<double>& unknowns,
                              Vector<double>& residuals);

// Maximum number of Newton iterations
extern unsigned Max_iter;

// Number of Newton iterations taken in most recent invocation
extern unsigned N_iter_taken;

// Flag to indicate if progress of Newton iteration is to be documented
// (defaults to false) 
 extern bool Doc_Progress;

// Size of increment used in finite-difference calculations 
extern double FD_step;

// Tolerance (maximum allowed value of an single residual at convergence) 
extern double Tol;

// Use steplength control do make globally convergent (default false)
extern bool Use_step_length_control;

// Black-box FD Newton solver:
// Calling sequence for residual function is
// \code residual_fct(parameters,unknowns,residuals) \endcode
// where all arguments are double Vectors.
// unknowns.size() = residuals.size() 
extern void black_box_fd_newton_solve(ResidualFctPt residual_fct,
                                      const Vector<double>& params, 
                                      Vector<double>& unknowns);


// Line search helper for globally convergent Newton method
extern void line_search(const Vector<double>& x_old, 
                        const double half_residual_squared_old, 
                        const Vector<double>& gradient, 
                        ResidualFctPt residual_fct, 
                        const Vector<double>& params,
                        Vector<double>& newton_dir,
                        Vector<double>& x, 
                        double& half_residual_squared,
                        const double& stpmax);

}


//====================================================================
/// \short Collection of data structures for storing information about
/// linear solves. Currently only contains storage for the
/// iteration counts and the linear solver time.
//====================================================================
class DocLinearSolverInfo
{
 
public:

 /// \short Constructor. Initialised the Iterations_and_times vector of vector
 /// of pairs.
 DocLinearSolverInfo() : Iterations_and_times()
 {}

 /// \short Set up a new vector of pairs for a new time step.
 void setup_new_time_step()
 {
   // For each new time step, we have a new vector consisting of pairs of
   // unsigned (for the iteration) and double (for the timing results).
   Iterations_and_times.push_back(Vector<Vector<double> >());
 }

 /// \short Add a new iteration and time pair.
 void add_iteration_and_time
   (unsigned iter, double prec_setup_time, double linear_solver_time)
 {
#ifdef PARANOID
   if(Iterations_and_times.size() == 0)
   {
     std::ostringstream error_message;
     error_message << "Iterations_and_times is empty. "
                   << "Call setup_new_time_step()\n";
     throw OomphLibError(
      error_message.str(),OOMPH_CURRENT_FUNCTION,
      OOMPH_EXCEPTION_LOCATION);
   }
#endif 
   Vector<double> tmp_iter_time_vec(3,0);
   tmp_iter_time_vec[0] = (double)iter;
   tmp_iter_time_vec[1] = prec_setup_time;
   tmp_iter_time_vec[2] = linear_solver_time;
   Iterations_and_times.back().push_back(tmp_iter_time_vec);
 }

 /// The number of time steps
 unsigned current_ntime_step() const 
   {return Iterations_and_times.size();}

 /// The number of Newton steps.
 unsigned current_nnewton_step() const 
   {return Iterations_and_times.back().size();}

 /// \short Accessor function for the iteration and times.
 Vector<Vector<Vector<double> > >& iterations_and_times()
   {return Iterations_and_times;}

 /// \short Accessor function for the iteration and times (const version).
 Vector<Vector<Vector<double> > > iterations_and_times() const 
   {return Iterations_and_times;}
 
private:

 /// Storage for number of iterations during Newton steps 
 Vector<Vector<Vector<double> > > Iterations_and_times;
};

//====================================================================
/// \short Information for documentation of results: 
/// Directory and file number to enable output
/// in the form RESLT/filename11.dat, say.
/// Documentation can be switched on and off.
//====================================================================
class DocInfo
{
 
public:

 /// \short Constructor. Default settings: Current directory, step `0',
 /// label="", full documentation enabled and output directory is not required
 /// to exist when set_directory() is called.
 DocInfo() : Directory("."), Doc_flag(true), Number(0),
             Label(""), Directory_must_exist(false)
 {}

 /// \short Constructor with specific directory.
 DocInfo(const std::string& directory) :
  Directory(directory), Doc_flag(true), Number(0),
  Label(""), Directory_must_exist(false)
 {}

 /// Output directory
 std::string directory() const {return Directory;}

 /// \short Set output directory (we try to open a file in it
 /// to see if the directory exists -- if it doesn't we'll
 /// issue a warning -- or, if directory_must_exist()==true,
 /// throw and OomphLibError
 void set_directory(const std::string& directory);

 /// \short Enable documentation
 void enable_doc() {Doc_flag=true;}

 /// \short Disable documentation
 void disable_doc() {Doc_flag=false;}

 /// \short Are we documenting?
 bool is_doc_enabled() const {return Doc_flag;}

 /// Number used (e.g.) for labeling output files
 unsigned& number() {return Number;}

 /// Number used (e.g.) for labeling output files. Const version.
 unsigned number() const {return Number;}

 /// Get number as a string (useful to completely avoid C-strings).
 std::string number_as_string() const
 {
  return StringConversion::to_string(Number); 
 } 

 /// String used (e.g.) for labeling output files
 std::string& label() {return Label;}

 /// String used (e.g.) for labeling output files. Const version.
 std::string label() const {return Label;}

 /// \short Call to throw an error if directory does not exist
 void enable_error_if_directory_does_not_exist() {Directory_must_exist=true;}

 /// \short Call to issue a warning if the directory does not exists
 void disable_error_if_directory_does_not_exist() {Directory_must_exist=false;}

private:

 /// Directory name
 std::string Directory;

 /// Doc or don't?
 bool Doc_flag;

 /// Number to label output file, say
 unsigned Number;

 /// String to label output file, say
 std::string Label;
               
 
 /// Boolean flag to decide response if an output 
 /// directory doesn't exist: If true, we terminate
 /// code execution by throwing an OomphLibError rather than 
 /// just issuing a warning.
 bool Directory_must_exist;
};


//====================================================================
// Namespace for command line arguments
//====================================================================
namespace CommandLineArgs
{

 // Number of arguments + 1
 extern int Argc;

 // Arguments themselves
 extern char** Argv;

 /// Map to indicate an input flag as having been set
 extern std::map<std::string,bool> Specified_command_line_flag;

 /// Map to associate an input flag with a double -- specified via pointer
 extern std::map<std::string,std::pair<bool,double*> > 
 Specified_command_line_double_pt;

 /// Map to associate an input flag with an int -- specified via pointer
 extern std::map<std::string,std::pair<bool,int*> > 
 Specified_command_line_int_pt;

 /// Map to associate an input flag with an unsigned -- specified via pointer
 extern std::map<std::string,std::pair<bool,unsigned*> > 
 Specified_command_line_unsigned_pt;

 /// Map to associate an input flag with a string -- specified via pointer
 extern std::map<std::string,std::pair<bool,std::string*> > 
 Specified_command_line_string_pt;

 // Set values
 extern void setup(int argc, char** argv);

 // Doc the command line arguments
 extern void output();

 /// Specify possible argument-free command line flag
 extern void specify_command_line_flag(const std::string& command_line_flag);

 /// \short Specify possible command line flag that specifies a double, accessed
 /// via pointer
 extern void specify_command_line_flag(const std::string& command_line_flag,
                                       double* arg_pt);

 /// \short Specify possible command line flag that specifies an int, accessed
 /// via pointer
 extern void specify_command_line_flag(const std::string& command_line_flag,
                                       int* arg_pt);
 /// \short Specify possible command line flag that specifies an unsigned, 
 /// accessed via pointer
 extern void specify_command_line_flag(const std::string& command_line_flag,
                                       unsigned* arg_pt);

 /// \short Specify possible command line flag that specifies a string, 
 /// accessed via pointer
 extern void specify_command_line_flag(const std::string& command_line_flag,
                                       std::string* arg_pt);

 /// \short Check if specified flag has been set (the associated 
 /// value will have been assigned directly)
 extern bool command_line_flag_has_been_set(const std::string& flag);

 /// Document specified command line flags
 extern void doc_specified_flags();

 /// Document available command line flags
 extern void doc_available_flags();

 /// Helper function to check if command line index is legal
 extern void check_arg_index(const int& argc,const int& arg_index);

 /// \short Parse command line, check for recognised flags and assign 
 /// associated values
 extern void parse_and_assign(int argc, char *argv[], bool throw_on_unrecognised_args=false);

 /// \short Parse previously specified command line, check for 
 /// recognised flags and assign associated values
 extern void parse_and_assign(bool throw_on_unrecognised_args=false);

}

// forward declaration of OomphCommunicator class
class OomphCommunicator;

#ifdef OOMPH_HAS_MPI
//========================================================================
/// MPI output modifier: Preceeds every output by 
/// specification of the processor ID. Output can be restricted
/// to a single processor.
//========================================================================
class MPIOutputModifier : public OutputModifier
{

private:

 /// \short Rank of single processor that produces output (only used
 /// if  Output_from_single_processor=true
 unsigned Output_rank; 

 /// Boolean to control if output is performed only on a single
 /// processor (default: false)
 bool Output_from_single_processor;

 /// Communicator
 OomphCommunicator* Communicator_pt;

public:


 /// Constructor -- initialise flags for output from all processors
 MPIOutputModifier() : Output_rank(0), 
  Output_from_single_processor(false)
  {}

 /// Return pointer to communicator
 OomphCommunicator*& communicator_pt() { return Communicator_pt; }

 /// Precede the output by the processor ID but output everything
 virtual bool operator()(std::ostream &stream);

 /// Switch to ensure output is only produced from a single
 /// processor (default: Master node, i.e. rank 0)
 void restrict_output_to_single_processor(const unsigned& output_rank=0)
  {
   Output_from_single_processor=true;
   Output_rank=output_rank;
  }


 /// Switch to ensure output is only produced from a single
 /// processor (default: Master node, i.e. rank 0)
 void allow_output_from_all_processors()
  {
   Output_from_single_processor=false;
  }


};


//========================================================================
/// Single (global) instantiation of the mpi output modifier
//========================================================================
extern MPIOutputModifier oomph_mpi_output;

//=== Forward DenseMatrix class
template <class T>
class DenseMatrix;

// forward declaration of oomph-communicator class
//class OomphCommunicator;

#endif



//======================================================================
/// \short MPI_Helpers class contains static helper methods to support MPI 
/// within oomph-lib. The methods init(...) and finalize() initialize and 
/// finalize MPI in oomph-lib and manage the oomph-libs global communicator
/// communicator_pt().
/// NOTE: This class encapsulates static helper methods and instances of it 
/// CANNOT be instantiated.
//=====================================================================
class MPI_Helpers
{

  public:

 /// \short initialise mpi (oomph-libs equivalent of MPI_Init(...)) 
 /// Initialises MPI and creates the global oomph-lib communicator.
 /// If optional boolean flag is set to false, we use
 /// MPI_COMM_WORLD itself as oomph-lib's communicator. Defaults to true.
 static void init(int argc, char **argv, 
                  const bool& make_duplicate_of_mpi_comm_world=true);

 /// finalize mpi (oomph-lib equivalent of MPI_Finalize()) 
 /// Deletes the global oomph-lib communicator and finalizes MPI.
 static void finalize();

 /// access to global communicator. This is the oomph-lib equivalent of
 /// MPI_COMM_WORLD
 static OomphCommunicator* communicator_pt();

 /// return true if MPI has been initialised
 static bool mpi_has_been_initialised() { return MPI_has_been_initialised; }

  private:

 /// \short private default constructor definition (to prevent instances of 
 /// the class being instantiated)
 MPI_Helpers();

 /// \short private copy constructor definition (to prevent instances of 
 /// the class being instantiated)
 MPI_Helpers(const MPI_Helpers&);

 /// Bool set to true if MPI has been initialised
 static bool MPI_has_been_initialised;

 /// the global communicator
 static OomphCommunicator* Communicator_pt;
};


//====================================================================
// Namespace for flagging up obsolete parts of the code
//====================================================================
namespace ObsoleteCode
{
 
 // Flag up obsolete parts of the code
 extern bool FlagObsoleteCode;

 // Output warning message
 extern void obsolete();

 // Output specified warning message
 extern void obsolete(const std::string &message);

}

//====================================================================
// Namespace for tecplot stuff
//====================================================================
namespace TecplotNames
{

 // Tecplot colours 
 extern Vector<std::string> colour;

 // Setup tecplot colours namespace
 extern void setup();

}


#ifdef LEAK_CHECK

//====================================================================
// Namespace for leak check: Keep a running count of all instantiated
// objects -- add your own if you want to...
//====================================================================
namespace LeakCheckNames
{

 extern long QuadTree_build;
 extern long OcTree_build;
 extern long QuadTreeForest_build;
 extern long OcTreeForest_build;
 extern long RefineableQElement<2>_build;
 extern long RefineableQElement<3>_build;
 extern long MacroElement_build;
 extern long HangInfo_build;
 extern long Node_build;
 extern long GeomReference_build;
 extern long AlgebraicNodeNodeUpdateInfo_build;
 extern long AlgebraicNode_build;

 // Reset counters
 extern void reset();


 // Doc counters
 extern void doc();
}

#endif

//====================================================================
// Namespace for pause() command
//====================================================================
namespace PauseFlags
{
 
 // Flag to enable pausing code
 extern bool PauseFlag;

}

/// Pause and dump out message
void pause(std::string message);



//=============================================================================
/// Helper for recording execution time.
//=============================================================================
namespace TimingHelpers
{

 /// returns the time in seconds after some point in past
 double timer();

}//end of namespace TimingHelpers





////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////



//===============================================================
/// Namespace with helper functions to assess total memory usage
/// on the fly using system() -- details are very machine specific! This just
/// provides the overall machinery with default settings for
/// our own (linux machines). Uses the system command
/// to spawn a command that computes the total memory usage
/// on the machine where this is called.  [Disclaimer: works 
/// on my machine(s) -- no guarantees for any other platform; 
/// linux or not. MH]
//===============================================================
namespace MemoryUsage
{
 
 /// \short Boolean to suppress synchronisation of doc memory
 /// usage on processors (via mpi barriers). True (i.e. sync is
 /// is suppressed by default because not all processors may
 /// execute the reach the relevant doc memory usage statements
 /// causing the code to hang).
 extern bool Suppress_mpi_synchronisation;

  /// \short String containing system command that obtains memory usage
  /// of all processes.
  /// Default assigment for linux. [Disclaimer: works on my machine(s) --
  /// no guarantees for any other platform; linux or not. MH]
 extern std::string My_memory_usage_system_string;

 /// \short Bool allowing quick bypassing of ALL operations related
 /// to memory usage monitoring -- this allows the code to remain
 /// "instrumented" without incurring the heavy penalties associated
 /// with the system calls and i/o. Default setting: false.
 extern bool Bypass_all_memory_usage_monitoring;
 
 /// \short String containing name of file in which we document
 /// my memory usage -- you may want to change this to allow different
 /// processors to write to separate files (especially in mpi 
 /// context). Note that file is appended to 
 /// so it ought to be emptied (either manually or by calling
 /// helper function empty_memory_usage_file()
 extern std::string My_memory_usage_filename;
 
 /// \short Function to empty file that records my memory usage in
 /// file whose name is specified by My_memory_usage_filename.
 void empty_my_memory_usage_file();
 
#ifdef OOMPH_HAS_UNISTDH

 /// \short Doc my memory usage, prepended by string (which allows
 /// identification from where the function is called, say) that records 
 /// memory usage in file whose name is specified by My_memory_usage_filename.
 /// Data is appended to that file; wipe it with empty_my_memory_usage_file().
 /// Note: This requires getpid() which is defined in unistd.h so if you
 /// don't have that we won't build that function!
 void doc_my_memory_usage(const std::string& prefix_string=0);

#endif
 
 /// \short String containing system command that obtains total memory usage.
 /// Default assigment for linux. [Disclaimer: works on my machine(s) --
 /// no guarantees for any other platform; linux or not. MH]
 extern std::string Total_memory_usage_system_string;
 
 /// \short String containing name of file in which we document total
 /// memory usage -- you may want to change this to allow different
 /// processors to write to separate files (especially in mpi 
 /// context). Note that file is appended to 
 /// so it ought to be emptied (either manually or by calling
 /// helper function empty_memory_usage_file()
 extern std::string Total_memory_usage_filename;
 
 /// \short Function to empty file that records total memory usage in
 /// file whose name is specified by Total_memory_usage_filename.
 void empty_total_memory_usage_file();
 
 /// \short Doc total memory usage, prepended by string (which allows
 /// identification from where the function is called, say) that records 
 /// mem usage in file whose name is specified by Total_memory_usage_filename.
 /// Data is appended to that file; wipe it with empty_memory_usage_file().
 void doc_total_memory_usage(const std::string& prefix_string="");
 
 /// \short Function to empty file that records total and local memory usage
 /// in appropriate files
 void empty_memory_usage_files();
 
 /// \short Doc total and local memory usage, prepended by string (which allows
 /// identification from where the function is called, say)
 void doc_memory_usage(const std::string& prefix_string="");
 



 /// \short String containing system command that runs "top" (or equivalent)
 /// "indefinitely" and writes to file specified in Top_output_filename.
 /// Default assigment for linux. [Disclaimer: works on my machine(s) --
 /// no guarantees for any other platform; linux or not. MH]
 extern std::string Top_system_string;
 
 /// \short  String containing name of file in which we document "continuous"
 /// output from "top" (or equivalent)-- you may want to change this to 
 /// allow different processors to write to separate files (especially in mpi 
 /// context). Note that file is appended to 
 /// so it ought to be emptied (either manually or by calling
 /// helper function empty_top_file()
 extern std::string Top_output_filename;
 
 /// \short Function to empty file that records continuous output from top in
 /// file whose name is specified by Top_output_filename
 void empty_top_file();
 
 /// \short Start running top continously and output (append) into 
 /// file specified by Top_output_filename. Wipe that file  with 
 /// empty_top_file() first if you wish. Note that this is again 
 /// quite linux specific and unlikely to work on other operating systems.
 /// Insert optional comment into output file before starting.
 void run_continous_top(const std::string& comment="");

 /// \short Stop running top continously. Note that this is 
 /// again quite linux specific and unlikely to work on other operating 
 /// systems.
 /// Insert optional comment into output file before stopping.
 void stop_continous_top(const std::string& comment="");
  
 /// \short Insert comment into running continous top output
 void insert_comment_to_continous_top(const std::string& comment);



 
} // end of namespace MemoryUsage



}
#endif
