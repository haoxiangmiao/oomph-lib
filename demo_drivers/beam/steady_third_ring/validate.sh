#! /bin/sh

# Get the OOPMH-LIB root directory from a makefile
OOMPH_ROOT_DIR=$(make -s --no-print-directory print-top_builddir)

#Set the number of tests to be checked
NUM_TESTS=1

# Setup validation directory
#---------------------------
rm -rf Validation
mkdir Validation

#######################################################################

# Validation for buckling third of a ring with displacement control
#------------------------------------------------------------------

cd Validation
mkdir RESLT

echo "Running steady third-of-a-ring validation "
../steady_third_ring > OUTPUT

echo "done"
echo " " >> validation.log
echo "Steady third-of-a-ring validation" >> validation.log
echo "---------------------------------" >> validation.log
echo " " >> validation.log
echo "Validation directory: " >> validation.log
echo " " >> validation.log
echo "  " `pwd` >> validation.log
echo " " >> validation.log
cat RESLT/ring0.dat \
    RESLT/ring5.dat \
    RESLT/ring12.dat \
    RESLT/ring20.dat \
    RESLT/trace.dat\
    > ring_results.dat

if test "$1" = "no_fpdiff"; then
  echo "dummy [OK] -- Can't run fpdiff.py because we don't have python or validata" >> validation.log
else
  ../../../../bin/fpdiff.py ../validata/ring_results.dat.gz \
   ring_results.dat >> validation.log
fi


# Append output to global validation log file
#--------------------------------------------
cat validation.log >> ../../../../validation.log

cd ..


#######################################################################


#Check that we get the correct number of OKs
# validate_ok_count will exit with status
# 0 if all tests has passed.
# 1 if some tests failed.
# 2 if there are more 'OK' than expected.
. $OOMPH_ROOT_DIR/bin/validate_ok_count

# Never get here
exit 10