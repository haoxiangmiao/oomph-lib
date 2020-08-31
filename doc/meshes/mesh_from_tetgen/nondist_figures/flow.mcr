#!MC 1100



##################################################
# PNG OR AVI OUTPUT?
##################################################
$!VARSET |PNG|=1

##################################################
# PLOT Y SLICE TOO?
##################################################
$!VARSET |PLOT_Y_SLICE|=1

        
$!VARSET |PNG_COUNT|=0

$!DRAWGRAPHICS FALSE

$!READDATASET  '"RESLT/soln0.dat" ' 
  READDATAOPTION = NEW
  RESETSTYLE = YES
  INCLUDETEXT = NO
  INCLUDEGEOM = NO
  INCLUDECUSTOMLABELS = NO
  VARLOADMODE = BYNAME
  ASSIGNSTRANDIDS = YES
  INITIALPLOTTYPE = CARTESIAN3D

$!FRAMELAYOUT ISTRANSPARENT = NO
$!FIELDLAYERS SHOWMESH = NO
$!FIELDLAYERS SHOWEDGE = NO
$!GLOBALCONTOUR 1  VAR = 7
$!CONTOURLEVELS RESETTONICE
  CONTOURGROUP = 1
  APPROXNUMVALUES = 15
$!FIELDLAYERS SHOWCONTOUR = YES
$!GLOBALTHREEDVECTOR UVAR = 4
$!GLOBALTHREEDVECTOR VVAR = 5
$!GLOBALTHREEDVECTOR WVAR = 6
$!RESETVECTORLENGTH 
$!FIELDLAYERS SHOWVECTOR = YES
$!REDRAWALL 
$!VIEW FIT
$!FIELDMAP [1-|NUMZONES|]  VECTOR{COLOR = BLACK}
$!VIEW FIT
$!THREEDAXIS FRAMEAXIS{XYPOS{X = 12.40518257}}
$!THREEDAXIS FRAMEAXIS{XYPOS{Y = 6.796626721}}


$!DRAWGRAPHICS TRUE
$!REDRAWALL 
$!IF |PNG|==0
        $!EXPORTSETUP
          EXPORTREGION = ALLFRAMES
          EXPORTFORMAT = AVI
          EXPORTFNAME = "flow.avi"
        $!EXPORTSETUP IMAGEWIDTH = 600
        $!EXPORTSETUP USESUPERSAMPLEANTIALIASING = YES
        $!EXPORTSTART
$!ELSE
        $!EXPORTSETUP EXPORTFORMAT = PNG
        $!EXPORTSETUP IMAGEWIDTH = 600
        $!EXPORTSETUP USESUPERSAMPLEANTIALIASING = YES
        $!EXPORTSETUP EXPORTFNAME = 'flow|PNG_COUNT|.png'
        $!EXPORT
          EXPORTREGION = ALLFRAMES
        $!VARSET |PNG_COUNT|+=1
$!ENDIF
$!DRAWGRAPHICS FALSE


$!FIELDLAYERS SHOWCONTOUR = NO
$!FIELDLAYERS SHOWVECTOR = NO
$!SLICELAYERS SHOW = YES


$!VARSET |NSTEP|=10
$!LOOP |NSTEP|
        $!VARSET |X_SLICE|=(|MINX|+(|MAXX|-|MINX|)*(|LOOP|-1)/(|NSTEP|-1))
        $!SLICEATTRIBUTES 1  PRIMARYPOSITION{X = |X_SLICE|}
        $!SLICEATTRIBUTES 1  CONTOUR{USELIGHTINGEFFECT = YES}
        $!SLICEATTRIBUTES 1  VECTOR{SHOW = YES}
        $!SLICEATTRIBUTES 1  VECTOR{ISTANGENT = YES}
        $!SLICEATTRIBUTES 1  SHOWGROUP = YES
        $!IF |PLOT_Y_SLICE|==1
          $!VARSET |Y_SLICE|=(|MINY|+(|MAXY|-|MINY|)*(|LOOP|-1)/(|NSTEP|-1))
          $!SLICEATTRIBUTES 2  PRIMARYPOSITION{Y = |Y_SLICE|}
          $!SLICEATTRIBUTES 2  CONTOUR{USELIGHTINGEFFECT = YES}
          $!SLICEATTRIBUTES 2  VECTOR{SHOW = YES}
          $!SLICEATTRIBUTES 2  VECTOR{ISTANGENT = YES}
          $!SLICEATTRIBUTES 2  SHOWGROUP = YES
        $!ENDIF

        $!DRAWGRAPHICS TRUE
        $!REDRAWALL
        $!IF |PNG|==0
                $!EXPORTNEXTFRAME
        $!ELSE
                $!EXPORTSETUP EXPORTFORMAT = PNG
                $!EXPORTSETUP IMAGEWIDTH = 600
                $!EXPORTSETUP USESUPERSAMPLEANTIALIASING = YES
                $!EXPORTSETUP EXPORTFNAME = 'flow|PNG_COUNT|.png'
                $!EXPORT
                  EXPORTREGION = ALLFRAMES
                $!VARSET |PNG_COUNT|+=1
        $!ENDIF 
        $!DRAWGRAPHICS FALSE
$!ENDLOOP


$!IF |PNG|==0
        $!EXPORTFINISH
$!ENDIF