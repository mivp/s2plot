/* s2plot_glodef.h
 *
 * Copyright 2006-2012 David G. Barnes, Paul Bourke, Christopher Fluke
 *
 * This file is part of S2PLOT.
 *
 * S2PLOT is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * S2PLOT is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with S2PLOT.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * We would appreciate it if research outcomes using S2PLOT would
 * provide the following acknowledgement:
 *
 * "Three-dimensional visualisation was conducted with the S2PLOT
 * progamming library"
 *
 * and a reference to
 *
 * D.G.Barnes, C.J.Fluke, P.D.Bourke & O.T.Parry, 2006, Publications
 * of the Astronomical Society of Australia, 23(2), 82-93.
 *
 * $Id: s2plot_glodef.h 5786 2012-10-09 01:47:31Z dbarnes $
 *
 */

#if !defined(S2PLOT_GLODEF_H)
#define S2PLOT_GLODEF_H

// a bunch of defines that remove "poorly" named global variables
// from global scope and make them better hidden behind an _s2 
// prefix...
#define S2_PRODUCTION_BUILD 1
#if defined (S2_PRODUCTION_BUILD)
#define options _s2x_options
#define interfacestate _s2x_interfacestate

#define ball _s2x_ball
#define nball _s2x_nball
#define ballt _s2x_ballt
#define nballt _s2x_nballt
#define disk _s2x_disk
#define ndisk _s2x_ndisk
#define cone _s2x_cone
#define ncone _s2x_ncone
#define dot _s2x_dot
#define ndot _s2x_ndot
#define line _s2x_line
#define nline _s2x_nline
#define face3 _s2x_face3
#define nface3 _s2x_nface3
#define face4 _s2x_face4
#define nface4 _s2x_nface4
#define face4t _s2x_face4t
#define nface4t _s2x_nface4t
#define label _s2x_label
#define nlabel _s2x_nlabel
#define handle _s2x_handle
#define nhandle _s2x_nhandle
#define bboard _s2x_bboard
#define nbboard _s2x_nbboard
#define face3a _s2x_face3a
#define nface3a _s2x_nface3a
#define trdot _s2x_trdot
#define ntrdot _s2x_ntrdot

#define ball_s _s2x_ball_s
#define nball_s _s2x_nball_s
#define ballt_s _s2x_ballt_s
#define nballt_s _s2x_nballt_s
#define disk_s _s2x_disk_s
#define ndisk_s _s2x_ndisk_s
#define cone_s _s2x_cone_s
#define ncone_s _s2x_ncone_s
#define dot_s _s2x_dot_s
#define ndot_s _s2x_ndot_s
#define line_s _s2x_line_s
#define nline_s _s2x_nline_s
#define face3_s _s2x_face3_s
#define nface3_s _s2x_nface3_s
#define face4_s _s2x_face4_s
#define nface4_s _s2x_nface4_s
#define face4t_s _s2x_face4t_s
#define nface4t_s _s2x_nface4t_s
#define label_s _s2x_label_s
#define nlabel_s _s2x_nlabel_s
#define handle_s _s2x_handle_s
#define nhandle_s _s2x_nhandle_s
#define bboard_s _s2x_bboard_s
#define nbboard_s _s2x_nbboard_s
#define face3a_s _s2x_face3a_s
#define nface3a_s _s2x_nface3a_s
#define trdot_s _s2x_trdot_s
#define ntrdot_s _s2x_ntrdot_s

#define ball_d _s2x_ball_d
#define nball_d _s2x_nball_d
#define ballt_d _s2x_ballt_d
#define nballt_d _s2x_nballt_d
#define disk_d _s2x_disk_d
#define ndisk_d _s2x_ndisk_d
#define cone_d _s2x_cone_d
#define ncone_d _s2x_ncone_d
#define dot_d _s2x_dot_d
#define ndot_d _s2x_ndot_d
#define line_d _s2x_line_d
#define nline_d _s2x_nline_d
#define face3_d _s2x_face3_d
#define nface3_d _s2x_nface3_d
#define face4_d _s2x_face4_d
#define nface4_d _s2x_nface4_d
#define face4t_d _s2x_face4t_d
#define nface4t_d _s2x_nface4t_d
#define label_d _s2x_label_d
#define nlabel_d _s2x_nlabel_d
#define handle_d _s2x_handle_d
#define nhandle_d _s2x_nhandle_d
#define bboard_d _s2x_bboard_d
#define nbboard_d _s2x_nbboard_d
#define face3a_d _s2x_face3a_d
#define nface3a_d _s2x_nface3a_d
#define trdot_d _s2x_trdot_d
#define ntrdot_d _s2x_ntrdot_d

#define ball_scr _s2x_ball_scr
#define nball_scr _s2x_nball_scr
#define ballt_scr _s2x_ballt_scr
#define nballt_scr _s2x_nballt_scr
#define disk_scr _s2x_scrisk_scr
#define ndisk_scr _s2x_ndisk_scr
#define cone_scr _s2x_cone_scr
#define ncone_scr _s2x_ncone_scr
#define dot_scr _s2x_scrot_scr
#define ndot_scr _s2x_ndot_scr
#define line_scr _s2x_line_scr
#define nline_scr _s2x_nline_scr
#define face3_scr _s2x_face3_scr
#define nface3_scr _s2x_nface3_scr
#define face4_scr _s2x_face4_scr
#define nface4_scr _s2x_nface4_scr
#define face4t_scr _s2x_face4t_scr
#define nface4t_scr _s2x_nface4t_scr
#define label_scr _s2x_label_scr
#define nlabel_scr _s2x_nlabel_scr
#define handle_scr _s2x_handle_scr
#define nhandle_scr _s2x_nhandle_scr
#define bboard_scr _s2x_bboard_scr
#define nbboard_scr _s2x_nbboard_scr
#define face3a_scr _s2x_face3a_scr
#define nface3a_scr _s2x_nface3a_scr
#define trdot_scr _s2x_trdot_scr
#define ntrdot_scr _s2x_ntrdot_scr

#define pmin _s2x_pmin
#define pmax _s2x_pmax
#define pmid _s2x_pmid
#define rangemin _s2x_rangemin
#define rangemax _s2x_rangemax
#define camera _s2x_camera
#define nlight _s2x_nlight
#define lights _s2x_lights
#define globalambient _s2x_globalambient
#define defaultlights _s2x_defaultlights
#define deflightpos _s2x_deflightpos
#define specularcolour _s2x_specularcolour
#define shininess _s2x_shininess
#define emission _s2x_emission
#define transparency _s2x_transparency
#define ndometop _s2x_ndometop
#define ndomebottom _s2x_ndomebottom
#define ndomeleft _s2x_ndomeleft
#define ndomeright _s2x_ndomeright
#define dometop _s2x_dometop
#define domebottom _s2x_domebottom
#define domeleft _s2x_domeleft
#define domeright _s2x_domeright
#define texturetop _s2x_texturetop
#define texturebottom _s2x_texturebottom
#define textureleft _s2x_textureleft
#define textureright _s2x_textureright
#define walltextureid _s2x_walltextureid
#define mesh _s2x_mesh
#define meshtype _s2x_meshtype
#define meshnx _s2x_meshnx
#define meshny _s2x_meshny
#define targetRoC_x _s2x_targetRoC_x
#define targetRoC_y _s2x_targetRoC_y
#define currRoC_x _s2x_currRoC_x
#define currRoC_y _s2x_currRoC_y

#endif

#endif

