#!MC 1000



#$!GETUSERINPUT |lostep| INSTRUCTIONS = "Loop. First Step??"
$!VARSET  |lostep|=0
#$!GETUSERINPUT |dlstep| INSTRUCTIONS = "Loop. Step Increment?"
$!VARSET  |dlstep|=1
$!GETUSERINPUT |nstep| INSTRUCTIONS = "Loop. Number of Steps??"

$!LOOP |nstep|
$!VarSet |nnstep| = |LOOP|
$!VarSet |nnstep| -= 1
$!VarSet |iistep| = |dlstep|
$!VarSet |iistep| *= |nnstep|
$!VarSet |iistep| += |lostep|
$!NEWLAYOUT
$!DRAWGRAPHICS FALSE
#    $!IF |iistep| < 10 
#      $!VARSET |istep|='00|iistep|'
#    $!ENDIF
#    $!IF |iistep| > 9 
#      $!VARSET |istep|='0|iistep|'
#    $!ENDIF
#    $!IF |iistep| > 99 
#      $!VARSET |istep|=|iistep|
#    $!ENDIF
$!VARSET |istep|=|iistep|
#$!VARSET |istep|*=3
$!READDATASET "soln|istep|.dat " 
  READDATAOPTION = NEW
  RESETSTYLE = YES
  INITIALPLOTTYPE = CARTESIAN2D

$!TWODAXIS YDETAIL{TITLE{TITLEMODE = USETEXT}}
$!TWODAXIS YDETAIL{TITLE{TEXT = 'y'}}
$!TWODAXIS XDETAIL{TITLE{TITLEMODE = USETEXT}}
$!TWODAXIS XDETAIL{TITLE{TEXT = 'x'}}
$!TWODAXIS GRIDAREA{DRAWBORDER = YES}
$!TWODAXIS YDETAIL{RANGEMIN = -2.1}
$!TWODAXIS YDETAIL{RANGEMAX = 2.1}
$!TWODAXIS XDETAIL{AUTOGRID = NO}
$!TWODAXIS XDETAIL{GRSPACING = 1}
$!TWODAXIS VIEWPORTPOSITION{X1 = 30.274884429}
$!TWODAXIS VIEWPORTPOSITION{X2 = 73.857667098}
$!TWODAXIS VIEWPORTPOSITION{Y2 = 89}
$!TWODAXIS VIEWPORTPOSITION{Y1 = 12}

$!TWODAXIS XDETAIL{RANGEMIN = -0.6}
$!TWODAXIS XDETAIL{RANGEMIN = -0.8}
$!TWODAXIS XDETAIL{RANGEMAX = 1.9}
$!TWODAXIS YDETAIL{RANGEMIN = -2.1}
$!TWODAXIS YDETAIL{RANGEMAX = 2.1}

$!REDRAWALL 




$!EXPORTSETUP EXPORTFORMAT = PNG
$!EXPORTSETUP IMAGEWIDTH = 600
$!EXPORTSETUP EXPORTFNAME = 'circle_as_element|LOOP|.png'
$!EXPORT
     EXPORTREGION = ALLFRAMES


$!EndLoop