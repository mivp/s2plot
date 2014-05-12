! s2plot.inc
!
! Copyright 2006-2012 David G. Barnes, Paul Bourke, Christopher Fluke
!
! This file is part of S2PLOT.
!
! S2PLOT is free software: you can redistribute it and/or modify it
! under the terms of the GNU General Public License as published by
! the Free Software Foundation, either version 3 of the License, or
! (at your option) any later version.
!
! S2PLOT is distributed in the hope that it will be useful, but
! WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
! General Public License for more details.
!
! You should have received a copy of the GNU General Public License
! along with S2PLOT.  If not, see <http://www.gnu.org/licenses/>. 
!
! We would appreciate it if research outcomes using S2PLOT would
! provide the following acknowledgement:
!
! "Three-dimensional visualisation was conducted with the S2PLOT
! progamming library"
!
! and a reference to
!
! D.G.Barnes, C.J.Fluke, P.D.Bourke & O.T.Parry, 2006, Publications
! of the Astronomical Society of Australia, 23(2), 82-93.
!

!***********************************************************************
!*   OPENING, CLOSING AND SELECTING DEVICES
!***********************************************************************

      INTEGER s2open
      INTEGER s2opend
      INTEGER s2opendo

!***********************************************************************
!*   WINDOWS AND VIEWPORTS
!***********************************************************************

!***********************************************************************
!*   PRIMITIVES
!***********************************************************************

!***********************************************************************
!*   DRAWING ATTRIBUTES
!***********************************************************************

      INTEGER s2qci
      INTEGER s2qlw
      INTEGER s2qls
      REAL s2qch
      INTEGER ss2qxh

!***********************************************************************
!*   AXES, BOXES & LABELS
!***********************************************************************

!***********************************************************************
!*   XY(Z) PLOTS
!***********************************************************************

!***********************************************************************
!*   IMAGES / SURFACES
!***********************************************************************

      INTEGER s2icm

!***********************************************************************
!*   ISOSURFACES
!***********************************************************************

      INTEGER ns2cis
      INTEGER ns2cisc

!***********************************************************************
!*   VOLUME RENDERING
!***********************************************************************

      INTEGER ns2cvr

!***********************************************************************
!*   S2 "NATIVE" ROUTINES
!***********************************************************************

!***********************************************************************
!*   DYNAMIC-ONLY GEOMETRY FUNCTIONS
!***********************************************************************

!***********************************************************************
!*   CALLBACK AND _S2HANDLE SYSTEM
!***********************************************************************

      INTEGER cs2qhv

!***********************************************************************
!*   PANELS
!***********************************************************************

      INTEGER xs2ap
      INTEGER xs2qpa
      INTEGER xs2qcpa
      INTEGER xs2qsp

!***********************************************************************
!*   ADVANCED TEXTURE / COLORMAP HANDLING
!***********************************************************************

      INTEGER ss2lt
      CHARACTER ss2gt
      INTEGER ss2lcm
      INTEGER ss2ltt
      INTEGER ss2ftt
      INTEGER ss2ct
      INTEGER ss2ctt

!***********************************************************************
!*   ENVIRONMENT / RENDERING ATTRIBUTES
!***********************************************************************

      INTEGER ss2qsr
      INTEGER ss2qrm
      REAL ss2qfra
      INTEGER ss2qpt
      REAL ss2qar

!***********************************************************************
!*   ADVANCED CAMERA CONTROL
!***********************************************************************

      INTEGER ss2qc
      INTEGER ss2qas
      REAL ss2qss
      REAL ss2qess

!***********************************************************************
!*   IMAGE WRITING/FETCHING
!***********************************************************************

      CHARACTER ss2gpix

!***********************************************************************
!*   MISCELLANEOUS PLOTS
!***********************************************************************

!***********************************************************************
!*   MISCELLANEOUS ROUTINES
!***********************************************************************

!***********************************************************************
!*   FUNCTIONS IN TESTING / DEVELOPMENT
!***********************************************************************

