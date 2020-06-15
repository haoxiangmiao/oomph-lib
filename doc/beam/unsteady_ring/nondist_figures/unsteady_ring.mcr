#!MC 1000

$!VARSET |PNG|=0


#$!GETUSERINPUT |lostep| INSTRUCTIONS = "Loop. First Step??"
$!VARSET  |lostep|=0
#$!GETUSERINPUT |dlstep| INSTRUCTIONS = "Loop. Step Increment?"
$!VARSET  |dlstep|=1
#$!GETUSERINPUT |nstep| INSTRUCTIONS = "Loop. Number of Steps??"
$!VARSET |nstep| = 186

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
#$!VARSET |istep|+=1
#$!VARSET |istep|*=10

$!DRAWGRAPHICS FALSE

#!MC 1000
$!VarSet |LFDSFN1| = '"RESLT/load|istep|.dat"'
$!VarSet |LFDSVL1| = '"V1" "V2" "V3" "V4" "V5"'
$!VarSet |LFDSFN2| = '"RESLT/ring|istep|.dat"'
$!VarSet |LFDSVL2| = '"V1" "V2" "V3" "V4" "V5"'
$!SETSTYLEBASE FACTORY
$!PAPER 
  BACKGROUNDCOLOR = WHITE
  ISTRANSPARENT = YES
  ORIENTPORTRAIT = NO
  SHOWGRID = YES
  SHOWRULER = YES
  SHOWPAPER = YES
  PAPERSIZE = A4
  PAPERSIZEINFO
    {
    A3
      {
      WIDTH = 11.693
      HEIGHT = 16.535
      }
    A4
      {
      WIDTH = 8.2677
      HEIGHT = 11.693
      LEFTHARDCLIPOFFSET = 0.125
      RIGHTHARDCLIPOFFSET = 0.125
      TOPHARDCLIPOFFSET = 0.125
      BOTTOMHARDCLIPOFFSET = 0.125
      }
    }
  RULERSPACING = ONECENTIMETER
  PAPERGRIDSPACING = ONETENTHCENTIMETER
  REGIONINWORKAREA
    {
    X1 = -0.05
    Y1 = -0.05
    X2 = 11.74
    Y2 = 8.318
    }
$!COLORMAP 
  CONTOURCOLORMAP = SMRAINBOW
$!COLORMAPCONTROL RESETTOFACTORY
### Frame Number 1 ###
$!READDATASET  '|LFDSFN1|' 
  INITIALPLOTTYPE = CARTESIAN2D
  INCLUDETEXT = YES
  INCLUDEGEOM = NO
  VARLOADMODE = BYNAME
  VARNAMELIST = '|LFDSVL1|' 
$!REMOVEVAR |LFDSVL1|
$!REMOVEVAR |LFDSFN1|
$!READDATASET  '|LFDSFN2|' 
  INITIALPLOTTYPE = CARTESIAN2D
  INCLUDETEXT = NO
  INCLUDEGEOM = NO
  READDATAOPTION = APPEND
  RESETSTYLE = NO
  VARLOADMODE = BYNAME
  VARNAMELIST = '|LFDSVL2|' 
$!REMOVEVAR |LFDSVL2|
$!REMOVEVAR |LFDSFN2|
$!FRAMELAYOUT 
  SHOWBORDER = NO
  SHOWHEADER = NO
  HEADERCOLOR = RED
  XYPOS
    {
    X = 0.26601
    Y = 0.14133
    }
  WIDTH = 7.874
  HEIGHT = 7.874
$!PLOTTYPE  = CARTESIAN2D
$!FRAMENAME  = 'Frame 001' 
$!ACTIVEFIELDZONES  =  [1-26]
$!GLOBALRGB 
  RANGEMIN = 0
  RANGEMAX = 1
$!GLOBALCONTOUR  1
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  2
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  3
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALCONTOUR  4
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  COLORMAPFILTER
    {
    CONTINUOUSCOLOR
      {
      CMIN = 0
      CMAX = 1
      }
    }
$!GLOBALTWODVECTOR 
  UVAR = 4
  VVAR = 5
  RELATIVELENGTH = 7
$!GLOBALSCATTER 
  LEGEND
    {
    XYPOS
      {
      X = 95
      }
    }
  REFSCATSYMBOL
    {
    COLOR = RED
    FILLCOLOR = RED
    }
$!FIELD  [1]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = RED
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    VECTORTYPE = HEADATPOINT
    ARROWHEADSTYLE = FILLED
    COLOR = BLUE
    }
  SCATTER
    {
    COLOR = RED
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = RED
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
$!FIELD  [2]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = GREEN
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    VECTORTYPE = HEADATPOINT
    ARROWHEADSTYLE = FILLED
    COLOR = BLUE
    }
  SCATTER
    {
    COLOR = GREEN
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = GREEN
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
$!FIELD  [3]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = BLUE
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    VECTORTYPE = HEADATPOINT
    ARROWHEADSTYLE = FILLED
    COLOR = BLUE
    }
  SCATTER
    {
    COLOR = BLUE
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = BLUE
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
$!FIELD  [4]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = CYAN
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    VECTORTYPE = HEADATPOINT
    ARROWHEADSTYLE = FILLED
    COLOR = BLUE
    }
  SCATTER
    {
    COLOR = CYAN
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = CYAN
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
$!FIELD  [5]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = YELLOW
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    VECTORTYPE = HEADATPOINT
    ARROWHEADSTYLE = FILLED
    COLOR = BLUE
    }
  SCATTER
    {
    COLOR = YELLOW
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = YELLOW
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
$!FIELD  [6]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = PURPLE
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    VECTORTYPE = HEADATPOINT
    ARROWHEADSTYLE = FILLED
    COLOR = BLUE
    }
  SCATTER
    {
    COLOR = PURPLE
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = PURPLE
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
$!FIELD  [7]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = RED
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    VECTORTYPE = HEADATPOINT
    ARROWHEADSTYLE = FILLED
    COLOR = BLUE
    }
  SCATTER
    {
    COLOR = RED
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = RED
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
$!FIELD  [8]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = GREEN
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    VECTORTYPE = HEADATPOINT
    ARROWHEADSTYLE = FILLED
    COLOR = BLUE
    }
  SCATTER
    {
    COLOR = GREEN
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = GREEN
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
$!FIELD  [9]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = BLUE
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    VECTORTYPE = HEADATPOINT
    ARROWHEADSTYLE = FILLED
    COLOR = BLUE
    }
  SCATTER
    {
    COLOR = BLUE
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = BLUE
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
$!FIELD  [10]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = CYAN
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    VECTORTYPE = HEADATPOINT
    ARROWHEADSTYLE = FILLED
    COLOR = BLUE
    }
  SCATTER
    {
    COLOR = CYAN
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = CYAN
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
$!FIELD  [11]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = YELLOW
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    VECTORTYPE = HEADATPOINT
    ARROWHEADSTYLE = FILLED
    COLOR = BLUE
    }
  SCATTER
    {
    COLOR = YELLOW
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = YELLOW
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
$!FIELD  [12]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = PURPLE
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    VECTORTYPE = HEADATPOINT
    ARROWHEADSTYLE = FILLED
    COLOR = BLUE
    }
  SCATTER
    {
    COLOR = PURPLE
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = PURPLE
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
$!FIELD  [13]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = RED
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    VECTORTYPE = HEADATPOINT
    ARROWHEADSTYLE = FILLED
    COLOR = BLUE
    }
  SCATTER
    {
    COLOR = RED
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = RED
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
$!FIELD  [14]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = GREEN
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    ARROWHEADSTYLE = FILLED
    COLOR = RED
    }
  SCATTER
    {
    COLOR = GREEN
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = GREEN
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2
$!FIELD  [15]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = BLUE
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    ARROWHEADSTYLE = FILLED
    COLOR = RED
    }
  SCATTER
    {
    COLOR = BLUE
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = BLUE
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2
$!FIELD  [16]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = CYAN
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    ARROWHEADSTYLE = FILLED
    COLOR = RED
    }
  SCATTER
    {
    COLOR = CYAN
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = CYAN
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2
$!FIELD  [17]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = YELLOW
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    ARROWHEADSTYLE = FILLED
    COLOR = RED
    }
  SCATTER
    {
    COLOR = YELLOW
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = YELLOW
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2
$!FIELD  [18]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = PURPLE
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    ARROWHEADSTYLE = FILLED
    COLOR = RED
    }
  SCATTER
    {
    COLOR = PURPLE
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = PURPLE
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2
$!FIELD  [19]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = RED
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    ARROWHEADSTYLE = FILLED
    COLOR = RED
    }
  SCATTER
    {
    COLOR = RED
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = RED
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2
$!FIELD  [20]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = GREEN
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    ARROWHEADSTYLE = FILLED
    COLOR = RED
    }
  SCATTER
    {
    COLOR = GREEN
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = GREEN
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2
$!FIELD  [21]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = BLUE
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    ARROWHEADSTYLE = FILLED
    COLOR = RED
    }
  SCATTER
    {
    COLOR = BLUE
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = BLUE
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2
$!FIELD  [22]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = CYAN
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    ARROWHEADSTYLE = FILLED
    COLOR = RED
    }
  SCATTER
    {
    COLOR = CYAN
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = CYAN
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2
$!FIELD  [23]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = YELLOW
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    ARROWHEADSTYLE = FILLED
    COLOR = RED
    }
  SCATTER
    {
    COLOR = YELLOW
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = YELLOW
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2
$!FIELD  [24]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = PURPLE
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    ARROWHEADSTYLE = FILLED
    COLOR = RED
    }
  SCATTER
    {
    COLOR = PURPLE
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = PURPLE
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2
$!FIELD  [25]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = RED
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    ARROWHEADSTYLE = FILLED
    COLOR = RED
    }
  SCATTER
    {
    COLOR = RED
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = RED
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2
$!FIELD  [26]
  MESH
    {
    COLOR = BLACK
    LINETHICKNESS = 0.8
    }
  CONTOUR
    {
    CONTOURTYPE = FLOOD
    COLOR = GREEN
    USELIGHTINGEFFECT = YES
    }
  VECTOR
    {
    ARROWHEADSTYLE = FILLED
    COLOR = RED
    }
  SCATTER
    {
    COLOR = GREEN
    }
  SHADE
    {
    COLOR = WHITE
    }
  BOUNDARY
    {
    SHOW = NO
    COLOR = GREEN
    }
  POINTS
    {
    POINTSTOPLOT = SURFACENODES
    }
  SURFACES
    {
    SURFACESTOPLOT = JPLANES
    }
  VOLUMEMODE
    {
    VOLUMEOBJECTSTOPLOT
      {
      SHOWISOSURFACES = NO
      SHOWSLICES = NO
      SHOWSTREAMTRACES = NO
      }
    }
  GROUP = 2
$!TWODAXIS 
  XDETAIL
    {
    VARNUM = 1
    }
  YDETAIL
    {
    VARNUM = 2
    }
$!VIEW FIT
$!TWODAXIS 
  AXISMODE = INDEPENDENT
  DEPXTOYRATIO = 1
$!TWODAXIS 
  XDETAIL
    {
    SHOWAXIS = NO
    RANGEMIN = -1.6000000000000000888
    RANGEMAX = 1.6000000000000000888
    GRSPACING = 0.5
    }
$!TWODAXIS 
  YDETAIL
    {
    SHOWAXIS = NO
    RANGEMIN = -1.6000000000000000888
    RANGEMAX = 1.6000000000000000888
    GRSPACING = 0.5
    }
$!GLOBALISOSURFACE 
  MARCHINGCUBEALGORITHM = CLASSICPLUS
$!GLOBALSLICE 
  BOUNDARY
    {
    SHOW = NO
    }
$!FIELDLAYERS 
  SHOWVECTOR = YES
  SHOWBOUNDARY = NO
$!ATTACHTEXT 
  ANCHORPOS
    {
    X = 1.57237534167
    Y = 1.75046690687
    }
  COLOR = RED
  TEXTSHAPE
    {
    FONT = HELV
    HEIGHT = 26
    }
  BOX
    {
    MARGIN = 10
    LINETHICKNESS = 0.4
    }
  SCOPE = GLOBAL
  MACROFUNCTIONCOMMAND = '' 
  TEXT = 'Velocity' 
$!SETSTYLEBASE CONFIG

$!DRAWGRAPHICS FALSE
$!CREATEMIRRORZONES 
  SOURCEZONES =  [1-26]
  MIRRORVAR = 'Y' 
$!CREATEMIRRORZONES 
  SOURCEZONES =  [1-26]
  MIRRORVAR = 'X' 
$!CREATEMIRRORZONES 
  SOURCEZONES =  [27-52]
  MIRRORVAR = 'X' 
$!FIELD [40-52]  GROUP = 2
$!FIELD [66-78]  GROUP = 2
$!FIELD [92-104]  GROUP = 2
$!FIELD [1-13,27-39,53-65,79-91]  VECTOR{COLOR = BLUE}
$!FIELD [1-13,27-39,53-65,79-91]  VECTOR{ARROWHEADSTYLE = FILLED}
$!FIELD [1-13,27-39,53-65,79-91]  VECTOR{VECTORTYPE = HEADATPOINT}
$!FIELD [1-13,27-39,53-65,79-91]  MESH{COLOR = BLACK}
$!FIELD [1-13,27-39,53-65,79-91]  MESH{LINETHICKNESS = 0.8}
$!FIELD [14-26,40-52,66-78,92-104]  MESH{COLOR = BLACK}
$!FIELD [14-26,40-52,66-78,92-104]  MESH{LINETHICKNESS = 0.8}
$!FIELD [14-26,40-52,66-78,92-104]  VECTOR{COLOR = RED}
$!FIELD [14-26,40-52,66-78,92-104]  VECTOR{ARROWHEADSTYLE = FILLED}


############################







$!IF |PNG|==1


        $!EXPORTSETUP EXPORTFORMAT = PNG
        $!EXPORTSETUP SUPERSAMPLEFACTOR = 3
        $!EXPORTSETUP USESUPERSAMPLEANTIALIASING = YES
        $!EXPORTSETUP IMAGEWIDTH = 600
        $!EXPORTSETUP EXPORTFNAME = 'unsteady_ring|istep|.png'
        $!EXPORT
          EXPORTREGION = CURRENTFRAME

        $!EXPORTSETUP EXPORTFORMAT = EPS
        $!EXPORTSETUP USESUPERSAMPLEANTIALIASING = NO
        $!EXPORTSETUP IMAGEWIDTH = 1423
        $!EXPORTSETUP EXPORTFNAME = 'unsteady_ring|istep|.eps'

        $!EXPORTSETUP PRINTRENDERTYPE = IMAGE
        $!EXPORTSETUP EXPORTFNAME = 'unsteady_ring|istep|.img.eps'
        $!EXPORT
          EXPORTREGION = CURRENTFRAME

$!ELSE

        $!IF |LOOP|>1
                $!EXPORTNEXTFRAME
        $!ELSE

                $!EXPORTSETUP
                 EXPORTFORMAT = AVI
                 EXPORTFNAME = "unsteady_ring.avi"
                $!EXPORTSETUP IMAGEWIDTH = 829
                $!EXPORTSETUP USESUPERSAMPLEANTIALIASING = NO
                $!EXPORTSTART
        $!ENDIF

$!ENDIF



$!EndLoop


$!IF |PNG|==0
        $!EXPORTFINISH
$!ENDIF


$!QUIT

