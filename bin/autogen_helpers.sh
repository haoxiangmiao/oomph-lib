
# A few helper functions for autogen or other bash scripts

# Read an option using readline
OptionRead()
{
    read -e Opt
    echo $Opt
}


# Convert a string passed as argument to return true/false (ie a bool),
# return value 255 indicates error.
YNToBool()
{
    if [[ $1 == "y" || $1 == "Y" ]]; then
        return $(true) # 0 == true in bash
    elif [[ $1 == "n" || $1 == "N" ]]; then
        return $(false) # 1 is false in bash
    else
        echo "I don't understand \"$1\", y/n only please." 1>&2
        return 255
    fi
}


# Read a y/n answer from input, if the answer is not y/n then repeat until
# it is. Turns out this can be nasty when combined with set -o errexit: you
# can only call this inside an if statement otherwise bash thinks any false
# return value is an error!
YesNoRead()
{
    prompt="$1"
    default="$2"

    # Read an answer
    printf "%s y/n [default %s]: " "$1" "$2"
    read Opt

    # Convert to a bool
    bool=""
    if [[ $Opt == "" ]]; then
        YNToBool $default
        bool=$?
    else
        YNToBool $Opt
        bool=$?
    fi

    # If we didn't recognise it then try again
    if [[ $bool != 1 && $bool != 0 ]]; then
        YesNoRead $prompt $default
        return $?
    fi

    return $bool
}


# This little function takes the input, removes anything following a #
# deletes blanks lines and then replaces all newlines by spaces.
ProcessOptionsFile()
{
    sed -e 's/#.*$//' -e '/^[ \t]*$/d' | tr '\n' ' '

    # Note: We use tr for newline replacement because sed operates on each
    # line individually and so never sees the newline characters (actually
    # it can be done in sed but it's complicated).
}


#Check that the "--" options preceed other configure options.
CheckOptions()
{
    awk '
   BEGIN{encountered_first_minus_minus=0
         encountered_first_non_minus_minus_after_first_minus_minus=0}
   NF {# pattern NF ignores blank lines since it expands to 0 for empty lines!
   # Ignore any comments (first entry in row starts with "#")
   if (substr($1,1,1)!="#")
    { 
     # Does the first entry in the line start with "--"?
     if (substr($1,1,2)=="--"){encountered_first_minus_minus=1}

     # Have we encountered the first "--" entry already?
     if (encountered_first_minus_minus==1)
       {
        # Does the first entry in the line not start with "--"?
        if (substr($1,1,2)!="--")
         {
          encountered_first_non_minus_minus_after_first_minus_minus=1
         }
       }
     # Should if this is followed by another "--" entry!
     if ((encountered_first_minus_minus==1)&&
         (encountered_first_non_minus_minus_after_first_minus_minus==1))
      {
       if (substr($1,1,2)=="--")
        {
         ok=0
         print "ERROR: The entry\n\n" $0 "\n"
         print "is in the wrong place. All the \"--\" prefixed options should go first!\n\n"
        }
      }
    }
    }' $(echo $1)
}


# This little function echo's the usage information
EchoUsage()
{
    echo "  -h"
    echo "      Show this usage info"
    echo "  "
    echo "  -r"
    echo "      Force regeneration of the configuration files before compliling, this"
    echo "      should never be needed unless either your build or autogen.sh is very broken."
    echo "  "
    echo "  -c [PATH_TO_CONFIGURE_OPTIONS_FILE]"
    echo "      Set the path to the configure options file to use"
    echo "      (default: [oomph-root]/config/configure_options/current)."
    echo "  "
    echo "  -b [BUILD_DIRECTORY]"
    echo "      Set the directory to put lib and include directories in (default [oomph-root]/build)."
    echo
    echo
    echo "Flags to control make:"
    echo
    echo "  -j [NCORES]"
    echo "      Set the number of cores to use (default: 1)."
    echo "  "
    echo "  -k"
    echo "      Keep compiling as far as possible even after errors occur (this"
    echo "      can save time but makes it harder to spot error messages)." 
    echo "  "
    echo "  -s"
    echo "      Silent mode: don't output so much useless spam from make and ./configure."
    echo "  "
}

# Get the absolute path from a path that could be relative or absolute (has
# to be portable, so we can't use readlink :( ). Note that this doesn't
# resolve symlinks (because that's harder to do and we don't need it).
AbsPath()
{
    # Catch absolute paths
    if [[ "$1" == /* ]]; then
        echo "$1"
    else
        # Catch the case where the pwd is / otherwise we end up with //.
        if [[ $PWD == / ]]; then
            dir="$1"
        else
            dir="$PWD/$1"
        fi

        # remove anything followed by /.. then remove any "./" and print
        # the result.
        echo "$dir" | sed -e 's|[^/]\+/\.\./||g' -e 's|\./||g'
    fi
}


#This little function echo's the usage information
EchoUsageForInteractiveScript()
{
        echo "Usage: "
        echo "------ "
        echo " "
        echo "[without flags]: Normal \"./configure; make; make install; make check\" sequence."
        echo " "
        echo " --rebuild     : Complete re-configure, followed by normal build sequence."
        echo " "
        echo "--jobs[=N]     :  Run N make jobs simultaneously."
        echo "                  Useful for speeding up the build on multi-core processors." 
        exit
}


