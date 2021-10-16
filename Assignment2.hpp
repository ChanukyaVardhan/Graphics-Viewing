/*
  A program which opens a window and draws the "color cube."

  Use the arrow keys and PgUp,PgDn, 
  keys to make the cube move.

  Written by - 
               Parag Chaudhuri
*/
#ifndef _COLORCUBE_HPP_
#define _COLORCUBE_HPP_

// Defining the ESCAPE Key Code
#define ESCAPE 27
// Defining the DELETE Key Code
#define DELETE 127

#include "gl_framework.hpp"
#include "shader_util.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <cstring>
#include <algorithm>

// Translation Parameters
GLfloat xpos=0.0,ypos=0.0,zpos=0.0;
// Rotation Parameters
GLfloat xrot=0.0,yrot=0.0,zrot=0.0;
// Camera position and rotation Parameters
// GLfloat c_xpos = 0.0, c_ypos = 0.0, c_zpos = 2.0;
GLfloat c_xpos = 0.0, c_ypos = 0.0, c_zpos = 2.0;
GLfloat c_up_x = 0.0, c_up_y = 1.0, c_up_z = 0.0;
GLfloat c_xrot=0.0,c_yrot=0.0,c_zrot=0.0;
//Running variable to toggle culling on/off
bool enable_culling=true;
//Running variable to toggle wireframe/solid modelling
bool solid=true;
//Enable/Disable perspective view
bool enable_perspective=false;

glm::vec4 points[10000], colors[10000];
glm::vec4 centroid_each_object[3];
int number_points_object[3];
int no_points = 0;
glm::vec3 scaling[3], rotation[3], translation[3];
int num_points_frustum = 34;
glm::vec4 points_frustum[34], colors_frustum[34];
glm::vec3 eye, lookat, up;
float L, R, T, B, N, F;

glm::vec3 u, v, n;
glm::mat4 wcs_vcs, vcs_ccs, ccs_ndcs, ndcs_dcs;

glm::vec4 red(1.0f, 0.0f, 0.0f, 1.0f);
glm::vec4 magenta(1.0f ,0.0f ,1.0f ,1.0f);
glm::vec4 cyan(0.0f, 1.0f, 1.0f, 1.0f);

double Lw = -2, Rw = 2, Bw = -2, Tw = 2;

int key_pressed = 0;

glm::vec4 frustum_corners[8]; // cloclwise starting from bottomleft first near then far

glm::vec4 points_clipped[10000], colors_clipped[10000];
glm::vec4 points_final[40000], colors_final[40000];
int number_points_object_final[3];
int no_points_final = 0;

bool flag_clipped = 0;
bool counter = 0;

glm::vec4 temp_clip_pt1, temp_clip_pt2;

bool key_7_check = 0;
//-------------------------------------------------------------------------

#endif
