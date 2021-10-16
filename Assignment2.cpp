/*
  CS475 Assignment 2
*/

#include "Assignment2.hpp"

GLuint shaderProgram;
GLuint vbo, vao;
GLuint vbo_clip, vao_clip;
GLuint vbo_frustum, vao_frustum;

glm::mat4 rotation_matrix;
glm::mat4 scaling_matrix;
glm::mat4 projection_matrix;
glm::mat4 c_rotation_matrix;
glm::mat4 lookat_matrix;

glm::mat4 model_matrix;
glm::mat4 view_matrix;

glm::mat4 translation_matrix;
glm::mat4 orig_trans_matrix;
glm::mat4 centroid_trans_matrix;


glm::mat4 modelview_matrix;
GLuint uModelViewMatrix;

GLuint vPosition;
GLuint vColor;

std::ifstream file_ptr("myscene.scn");
//-----------------------------------------------------------------

double area_triangle(glm::vec4 a, glm::vec4 b, glm::vec4 c)
{
  double abx = b.x - a.x;
  double aby = b.y - a.y;
  double abz = b.z - a.z;

  double acx = c.x - a.x;
  double acy = c.y - a.y;
  double acz = c.z - a.z;

  double cross_product_x = aby*acz - acy*abz;
  double cross_product_y = abx*acz - acx*abz;
  double cross_product_z = abx*acy - acx*aby;

  double area = sqrt(cross_product_x*cross_product_x + cross_product_y*cross_product_y + cross_product_z*cross_product_z) / 2;
  return area;
}

glm::vec4 centroid_object(int start, int end) // sigma(AiCi)/sigma(Ai) 
{
  double centroid_x = 0.0, centroid_y = 0.0, centroid_z = 0.0;

  double area_total=0;
  for (int i = start; i < end; i+=3)
  {
    glm::vec4 a = points[i], b = points[i+1], c = points[i+2];
    double area = area_triangle(a, b, c);

    double centroid_triangle_x = (a.x + b.x + c.x) /3;
    double centroid_triangle_y = (a.y + b.y + c.y) /3;
    double centroid_triangle_z = (a.z + b.z + c.z) /3;

    centroid_x = (centroid_x*area_total + centroid_triangle_x*area) / (area_total+area);
    centroid_y = (centroid_y*area_total + centroid_triangle_y*area) / (area_total+area);
    centroid_z = (centroid_z*area_total + centroid_triangle_z*area) / (area_total+area);
    area_total = area_total + area;
  }

  glm::vec4 centroid_final(centroid_x, centroid_y, centroid_z, 1.0f);
  return centroid_final;
}

void initBuffersGL(void)
{
  // Load shaders and use the resulting shader program
  std::string vertex_shader_file("vshader.glsl");
  std::string fragment_shader_file("fshader.glsl");

  std::vector<GLuint> shaderList;
  shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
  shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

  shaderProgram = csX75::CreateProgramGL(shaderList);
  glUseProgram( shaderProgram );

  // getting the attributes from the shader program
  vPosition = glGetAttribLocation( shaderProgram, "vPosition" );
  vColor = glGetAttribLocation( shaderProgram, "vColor" ); 
  uModelViewMatrix = glGetUniformLocation( shaderProgram, "uModelViewMatrix");

  glEnable(GL_PROGRAM_POINT_SIZE); 
  glGenVertexArrays (1, &vao);
  glGenBuffers (1, &vbo);

  glBindVertexArray (vao);
  glBindBuffer (GL_ARRAY_BUFFER, vbo);

  glBufferData (GL_ARRAY_BUFFER, (sizeof(points[0]))*(number_points_object[2] * 2), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points[0])*number_points_object[2], points );
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(points[0])*number_points_object[2], sizeof(colors[0])*number_points_object[2], colors );

  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  glEnableVertexAttribArray( vColor );
  glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points[0])*number_points_object[2]) );

  // Frustum
  glGenVertexArrays (1, &vao_frustum);
  glGenBuffers (1, &vbo_frustum);

  glBindVertexArray (vao_frustum);
  glBindBuffer (GL_ARRAY_BUFFER, vbo_frustum);

  glBufferData (GL_ARRAY_BUFFER, (sizeof(points_frustum[0]))*(num_points_frustum * 2), NULL, GL_STATIC_DRAW);
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points_frustum[0])*num_points_frustum, points_frustum );
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(points_frustum[0])*num_points_frustum, sizeof(colors_frustum[0])*num_points_frustum, colors_frustum );

  glEnableVertexAttribArray( vPosition );
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  
  glEnableVertexAttribArray( vColor );
  glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points_frustum[0])*num_points_frustum) );

}

glm::mat4 generate_view_matrix()
{
  if(key_pressed == 0)
  {
    return glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, 0.0f,
                     0.0f, 0.0f, 0.0f, 1.0f);
  }
  else if(key_pressed == 1)
  {
    return wcs_vcs;
  }
  else if(key_pressed == 2)
  {
    return vcs_ccs * wcs_vcs;
  }
  else if(key_pressed == 3)
  {
    return ccs_ndcs * vcs_ccs * wcs_vcs;
  }
  else if(key_pressed == 4)
  {
    return ndcs_dcs * ccs_ndcs * vcs_ccs * wcs_vcs;
  }
  else if(key_pressed == 5)
  {
    return vcs_ccs * wcs_vcs;
  }
  else if(key_pressed == 6)
  {
    return ccs_ndcs * vcs_ccs * wcs_vcs;
  }
  else if(key_pressed == 7)
  {
    return ndcs_dcs * ccs_ndcs * vcs_ccs * wcs_vcs;
  }
  return ndcs_dcs * ccs_ndcs * vcs_ccs * wcs_vcs;
}

void compute_outcode_point(glm::vec4 new_point, int* oc) // Always for points_clipped
{ // TBRLFB
  for(int i=0; i<6; i++)
  {
    oc[i] = 0;
  }

  if(new_point.y - new_point.w > 0)
  {
    oc[0] = 1;
  }
  if(new_point.y + new_point.w < 0)
  {
    oc[1] = 1;
  }
  if(new_point.x - new_point.w > 0)
  {
    oc[2] = 1;
  }
  if(new_point.x + new_point.w < 0)
  {
    oc[3] = 1;
  }
  if(new_point.z - new_point.w > 0)
  {
    oc[4] = 1;
  }
  if(new_point.z + new_point.w < 0)
  {
    oc[5] = 1;
  }
}

void clip_points(int x, int y)
{
  int outcode_x[6], outcode_y[6];

  glm::vec4 temp_point_1, temp_point_2, temp_op_decider;

  temp_clip_pt1 = points_clipped[x];
  temp_clip_pt2 = points_clipped[y];
  
  int op = x;
  temp_op_decider = points_clipped[x];
  while(1)
  {
    compute_outcode_point(temp_clip_pt1, outcode_x);
    compute_outcode_point(temp_clip_pt2, outcode_y);

    int trivial_accept = 0,trivial_reject = 0;
    for(int i=0; i<6; i++)
    {
      trivial_accept += outcode_x[i] + outcode_y[i];
      trivial_reject += (outcode_x[i] * outcode_y[i]);
    }

    if(trivial_accept == 0)
    {
      flag_clipped = 1;
      return;
    }

    if(trivial_reject >= 1)
    {
      return;
    }

    if(temp_op_decider.y - temp_op_decider.w > 0)
    {
      temp_op_decider.y = ((points_clipped[x].y)*(points_clipped[y].w - points_clipped[x].w) - (points_clipped[x].w)*(points_clipped[y].y - points_clipped[x].y)) / ((points_clipped[y].w - points_clipped[x].w) - (points_clipped[y].y - points_clipped[x].y));
      temp_op_decider.w = temp_op_decider.y;
      if(points_clipped[y].y == points_clipped[x].y)
      {
        temp_op_decider.x = points_clipped[x].x + ((points_clipped[y].x - points_clipped[x].x)*(temp_op_decider.w - points_clipped[x].w)/(points_clipped[y].w - points_clipped[x].w));
        temp_op_decider.z = points_clipped[x].z + ((points_clipped[y].z - points_clipped[x].z)*(temp_op_decider.w - points_clipped[x].w)/(points_clipped[y].w - points_clipped[x].w));
      }
      else
      {
        temp_op_decider.x = points_clipped[x].x + ((points_clipped[y].x - points_clipped[x].x)*(temp_op_decider.y - points_clipped[x].y)/(points_clipped[y].y - points_clipped[x].y));
        temp_op_decider.z = points_clipped[x].z + ((points_clipped[y].z - points_clipped[x].z)*(temp_op_decider.y - points_clipped[x].y)/(points_clipped[y].y - points_clipped[x].y));
      }
    }
    else if(temp_op_decider.y + temp_op_decider.w < 0)
    {
      temp_op_decider.y = ((points_clipped[x].y)*(points_clipped[y].w - points_clipped[x].w) - (points_clipped[x].w)*(points_clipped[y].y - points_clipped[x].y)) / ((points_clipped[y].w - points_clipped[x].w) + (points_clipped[y].y - points_clipped[x].y));
      temp_op_decider.w = -temp_op_decider.y;
      if(points_clipped[y].y == points_clipped[x].y)
      {
        temp_op_decider.x = points_clipped[x].x + ((points_clipped[y].x - points_clipped[x].x)*(temp_op_decider.w - points_clipped[x].w)/(points_clipped[y].w - points_clipped[x].w));
        temp_op_decider.z = points_clipped[x].z + ((points_clipped[y].z - points_clipped[x].z)*(temp_op_decider.w - points_clipped[x].w)/(points_clipped[y].w - points_clipped[x].w));
      }
      else
      {
        temp_op_decider.x = points_clipped[x].x + ((points_clipped[y].x - points_clipped[x].x)*(temp_op_decider.y - points_clipped[x].y)/(points_clipped[y].y - points_clipped[x].y));
        temp_op_decider.z = points_clipped[x].z + ((points_clipped[y].z - points_clipped[x].z)*(temp_op_decider.y - points_clipped[x].y)/(points_clipped[y].y - points_clipped[x].y));
      }
    }
    else if(temp_op_decider.x - temp_op_decider.w > 0)
    {
      temp_op_decider.x = ((points_clipped[x].x)*(points_clipped[y].w - points_clipped[x].w) - (points_clipped[x].w)*(points_clipped[y].x - points_clipped[x].x)) / ((points_clipped[y].w - points_clipped[x].w) - (points_clipped[y].x - points_clipped[x].x));
      temp_op_decider.w = temp_op_decider.x;
      if(points_clipped[y].x == points_clipped[x].x)
      {
        temp_op_decider.y = points_clipped[x].y + ((points_clipped[y].y - points_clipped[x].y)*(temp_op_decider.w - points_clipped[x].w)/(points_clipped[y].w - points_clipped[x].w));
        temp_op_decider.z = points_clipped[x].z + ((points_clipped[y].z - points_clipped[x].z)*(temp_op_decider.w - points_clipped[x].w)/(points_clipped[y].w - points_clipped[x].w));
      }
      else
      {
        temp_op_decider.y = points_clipped[x].y + ((points_clipped[y].y - points_clipped[x].y)*(temp_op_decider.x - points_clipped[x].x)/(points_clipped[y].x - points_clipped[x].x));
        temp_op_decider.z = points_clipped[x].z + ((points_clipped[y].z - points_clipped[x].z)*(temp_op_decider.x - points_clipped[x].x)/(points_clipped[y].x - points_clipped[x].x));
      }
    }
    else if(temp_op_decider.x + temp_op_decider.w < 0)
    {
      temp_op_decider.x = ((points_clipped[x].x)*(points_clipped[y].w - points_clipped[x].w) - (points_clipped[x].w)*(points_clipped[y].x - points_clipped[x].x)) / ((points_clipped[y].w - points_clipped[x].w) + (points_clipped[y].x - points_clipped[x].x));
      temp_op_decider.w = -temp_op_decider.x;
      if(points_clipped[y].x == points_clipped[x].x)
      {
        temp_op_decider.y = points_clipped[x].y + ((points_clipped[y].y - points_clipped[x].y)*(temp_op_decider.w - points_clipped[x].w)/(points_clipped[y].w - points_clipped[x].w));
        temp_op_decider.z = points_clipped[x].z + ((points_clipped[y].z - points_clipped[x].z)*(temp_op_decider.w - points_clipped[x].w)/(points_clipped[y].w - points_clipped[x].w));
      }
      else
      {
        temp_op_decider.y = points_clipped[x].y + ((points_clipped[y].y - points_clipped[x].y)*(temp_op_decider.x - points_clipped[x].x)/(points_clipped[y].x - points_clipped[x].x));
        temp_op_decider.z = points_clipped[x].z + ((points_clipped[y].z - points_clipped[x].z)*(temp_op_decider.x - points_clipped[x].x)/(points_clipped[y].x - points_clipped[x].x));
      }
    }
    else if(temp_op_decider.z - temp_op_decider.w > 0)
    {
      temp_op_decider.z = ((points_clipped[x].z)*(points_clipped[y].w - points_clipped[x].w) - (points_clipped[x].w)*(points_clipped[y].z - points_clipped[x].z)) / ((points_clipped[y].w - points_clipped[x].w) - (points_clipped[y].z - points_clipped[x].z));
      temp_op_decider.w = temp_op_decider.z;
      if(points_clipped[y].z == points_clipped[x].z)
      {
        temp_op_decider.y = points_clipped[x].y + ((points_clipped[y].y - points_clipped[x].y)*(temp_op_decider.w - points_clipped[x].w)/(points_clipped[y].w - points_clipped[x].w));
        temp_op_decider.x = points_clipped[x].x + ((points_clipped[y].x - points_clipped[x].x)*(temp_op_decider.w - points_clipped[x].w)/(points_clipped[y].w - points_clipped[x].w));
      }
      else
      {
        temp_op_decider.y = points_clipped[x].y + ((points_clipped[y].y - points_clipped[x].y)*(temp_op_decider.z - points_clipped[x].z)/(points_clipped[y].z - points_clipped[x].z));
        temp_op_decider.x = points_clipped[x].x + ((points_clipped[y].x - points_clipped[x].x)*(temp_op_decider.z - points_clipped[x].z)/(points_clipped[y].z - points_clipped[x].z));
      }
    }
    else if(temp_op_decider.z + temp_op_decider.w < 0)
    {
      temp_op_decider.z = ((points_clipped[x].z)*(points_clipped[y].w - points_clipped[x].w) - (points_clipped[x].w)*(points_clipped[y].z - points_clipped[x].z)) / ((points_clipped[y].w - points_clipped[x].w) + (points_clipped[y].z - points_clipped[x].z));
      temp_op_decider.w = -temp_op_decider.z;
      if(points_clipped[y].z == points_clipped[x].z)
      {
        temp_op_decider.y = points_clipped[x].y + ((points_clipped[y].y - points_clipped[x].y)*(temp_op_decider.w - points_clipped[x].w)/(points_clipped[y].w - points_clipped[x].w));
        temp_op_decider.x = points_clipped[x].x + ((points_clipped[y].x - points_clipped[x].x)*(temp_op_decider.w - points_clipped[x].w)/(points_clipped[y].w - points_clipped[x].w));
      }
      else
      {
        temp_op_decider.y = points_clipped[x].y + ((points_clipped[y].y - points_clipped[x].y)*(temp_op_decider.z - points_clipped[x].z)/(points_clipped[y].z - points_clipped[x].z));
        temp_op_decider.x = points_clipped[x].x + ((points_clipped[y].x - points_clipped[x].x)*(temp_op_decider.z - points_clipped[x].z)/(points_clipped[y].z - points_clipped[x].z));
      }
    }
    else
    {
      op = y;
      temp_op_decider = points_clipped[y];
    }
    
    if(op == x)
    {
      temp_clip_pt1 = temp_op_decider;
    }
    else
    {
      temp_clip_pt2 = temp_op_decider;
    }
  }
}

void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Creating the lookat and the up vectors for the camera
  c_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(c_xrot), glm::vec3(1.0f,0.0f,0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_yrot), glm::vec3(0.0f,1.0f,0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_zrot), glm::vec3(0.0f,0.0f,1.0f));

  glm::vec4 c_pos = glm::vec4(c_xpos,c_ypos,c_zpos, 1.0)*c_rotation_matrix;
  glm::vec4 c_up = glm::vec4(c_up_x,c_up_y,c_up_z, 1.0)*c_rotation_matrix;
  //Creating the lookat matrix
  lookat_matrix = glm::lookAt(glm::vec3(c_pos),glm::vec3(0.0),glm::vec3(c_up));

  //creating the projection matrix
  if(enable_perspective)
    projection_matrix = glm::frustum(-1.0, 1.0, -1.0, 1.0, 1.0, 5.0);
  else
    // projection_matrix = glm::ortho(-2.0, 2.0, -2.0, 2.0, -5.0, 5.0);
    projection_matrix = glm::ortho(Lw, Rw, Bw, Tw, -5.0, 10.0);

  view_matrix = generate_view_matrix();

  // Frustum
  glBindVertexArray (vao_frustum);
  modelview_matrix = projection_matrix*lookat_matrix*view_matrix;
  glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));
  glDrawArrays(GL_LINES, 0, num_points_frustum);
  glDrawArrays(GL_POINTS,0,1);


  if(key_pressed == 5 || key_pressed == 6 || key_pressed == 7)
  {
    if(key_pressed == 7){
      key_7_check = 1;
      key_pressed = 5;
      view_matrix = generate_view_matrix();
    }

    if(counter == 0)
    {
      for(int i=0; i<3; i++)
      {
        int start, end;
        if(i == 0)
        {
          start = 0;
          end = number_points_object[0];
        }
        else
        {
          start = number_points_object[i-1];
          end = number_points_object[i];
        }

        scaling_matrix = glm::scale(glm::mat4(), scaling[i]);

        centroid_each_object[i] = centroid_object(start, end);
        centroid_each_object[i] = scaling_matrix*centroid_each_object[i];

        rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation[i].x), glm::vec3(1.0f,0.0f,0.0f));
        rotation_matrix = glm::rotate(rotation_matrix, glm::radians(rotation[i].y), glm::vec3(0.0f,1.0f,0.0f));
        rotation_matrix = glm::rotate(rotation_matrix, glm::radians(rotation[i].z), glm::vec3(0.0f,0.0f,1.0f));

        orig_trans_matrix = glm::translate(glm::mat4(), glm::vec3(-centroid_each_object[i].x,-centroid_each_object[i].y,-centroid_each_object[i].z));
        centroid_trans_matrix = glm::translate(glm::mat4(), glm::vec3(centroid_each_object[i].x,centroid_each_object[i].y,centroid_each_object[i].z));

        translation_matrix = glm::translate(glm::mat4(), translation[i]);

        model_matrix = translation_matrix*centroid_trans_matrix*rotation_matrix*orig_trans_matrix*scaling_matrix;

        modelview_matrix = projection_matrix*lookat_matrix*view_matrix*model_matrix;

        for(int j=start; j<end; j++)
        {
          points_clipped[j] = view_matrix*model_matrix*points[j];
          colors_clipped[j] = colors[j];
        }

        for(int j=start; j<end; j+=3)
        {
          flag_clipped = 0;
          glm::vec4 temp_points[6];
          clip_points(j, j+1);
          temp_points[0] = temp_clip_pt1;
          temp_points[1] = temp_clip_pt2;
          clip_points(j+1, j+2);
          temp_points[2] = temp_clip_pt1;
          temp_points[3] = temp_clip_pt2;
          clip_points(j+2, j);
          temp_points[4] = temp_clip_pt1;
          temp_points[5] = temp_clip_pt2;

          if(flag_clipped)
          {

            int oc0[6], oc1[6], oc2[6], oc3[6], oc4[6], oc5[6];
            compute_outcode_point(temp_points[0], oc0);
            compute_outcode_point(temp_points[1], oc1);
            compute_outcode_point(temp_points[2], oc2);
            compute_outcode_point(temp_points[3], oc3);
            compute_outcode_point(temp_points[4], oc4);
            compute_outcode_point(temp_points[5], oc5);

            int sum_oc0 = 0, sum_oc1 = 0, sum_oc2 = 0, sum_oc3 = 0, sum_oc4 = 0, sum_oc5 = 0;
            for(int k=0; k<6; k++)
            {
              sum_oc0 += oc0[k];
              sum_oc1 += oc1[k];
              sum_oc2 += oc2[k];
              sum_oc3 += oc3[k];
              sum_oc4 += oc4[k];
              sum_oc5 += oc5[k];
            }

            if(sum_oc0 != 0)
            {
              temp_points[0] = temp_points[5];
            }
            else if(sum_oc5 != 0)
            {
              temp_points[5] = temp_points[0];
            }
            if(sum_oc1 != 0)
            {
              temp_points[1] = temp_points[2];
            }
            else if(sum_oc2 != 0)
            {
              temp_points[2] = temp_points[1];
            }
            if(sum_oc3 != 0)
            {
              temp_points[3] = temp_points[4];
            }
            else if(sum_oc4 != 0)
            {
              temp_points[4] = temp_points[3];
            }


            points_final[no_points_final] = temp_points[0];
            points_final[no_points_final] = glm::inverse(view_matrix*model_matrix)*points_final[no_points_final];
            colors_final[no_points_final] = colors_clipped[j];
            no_points_final++;
            points_final[no_points_final] = temp_points[1];
            points_final[no_points_final] = glm::inverse(view_matrix*model_matrix)*points_final[no_points_final];
            colors_final[no_points_final] = colors_clipped[j+1];
            no_points_final++;
            points_final[no_points_final] = temp_points[2];
            points_final[no_points_final] = glm::inverse(view_matrix*model_matrix)*points_final[no_points_final];
            colors_final[no_points_final] = colors_clipped[j+1];
            no_points_final++;
            points_final[no_points_final] = temp_points[0];
            points_final[no_points_final] = glm::inverse(view_matrix*model_matrix)*points_final[no_points_final];
            colors_final[no_points_final] = colors_clipped[j];
            no_points_final++;
            points_final[no_points_final] = temp_points[2];
            points_final[no_points_final] = glm::inverse(view_matrix*model_matrix)*points_final[no_points_final];
            colors_final[no_points_final] = colors_clipped[j+1];
            no_points_final++;
            points_final[no_points_final] = temp_points[3];
            points_final[no_points_final] = glm::inverse(view_matrix*model_matrix)*points_final[no_points_final];
            colors_final[no_points_final] = colors_clipped[j+2];
            no_points_final++;
            points_final[no_points_final] = temp_points[0];
            points_final[no_points_final] = glm::inverse(view_matrix*model_matrix)*points_final[no_points_final];
            colors_final[no_points_final] = colors_clipped[j];
            no_points_final++;
            points_final[no_points_final] = temp_points[3];
            points_final[no_points_final] = glm::inverse(view_matrix*model_matrix)*points_final[no_points_final];
            colors_final[no_points_final] = colors_clipped[j+2];
            no_points_final++;
            points_final[no_points_final] = temp_points[4];
            points_final[no_points_final] = glm::inverse(view_matrix*model_matrix)*points_final[no_points_final];
            colors_final[no_points_final] = colors_clipped[j+2];
            no_points_final++;
            points_final[no_points_final] = temp_points[0];
            points_final[no_points_final] = glm::inverse(view_matrix*model_matrix)*points_final[no_points_final];
            colors_final[no_points_final] = colors_clipped[j];
            no_points_final++;
            points_final[no_points_final] = temp_points[4];
            points_final[no_points_final] = glm::inverse(view_matrix*model_matrix)*points_final[no_points_final];
            colors_final[no_points_final] = colors_clipped[j+2];
            no_points_final++;
            points_final[no_points_final] = temp_points[5];
            points_final[no_points_final] = glm::inverse(view_matrix*model_matrix)*points_final[no_points_final];
            colors_final[no_points_final] = colors_clipped[j];
            no_points_final++;
          }
        }
        number_points_object_final[i] = no_points_final;
      }

      glGenVertexArrays (1, &vao_clip);
      glGenBuffers (1, &vbo_clip);

      glBindVertexArray (vao_clip);
      glBindBuffer (GL_ARRAY_BUFFER, vbo_clip);

      glBufferData (GL_ARRAY_BUFFER, (sizeof(points_final[0]))*(number_points_object_final[2] * 2), NULL, GL_STATIC_DRAW);
      glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points_final[0])*number_points_object_final[2], points_final );
      glBufferSubData( GL_ARRAY_BUFFER, sizeof(points_final[0])*number_points_object_final[2], sizeof(colors_final[0])*number_points_object_final[2], colors_final );

      glEnableVertexAttribArray( vPosition );
      glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
      
      glEnableVertexAttribArray( vColor );
      glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points_final[0])*number_points_object_final[2]) );

    }

    if(key_7_check){
      key_7_check = 0;
      key_pressed = 7;
      view_matrix = generate_view_matrix();  
    }
    counter = 1;
  }
   
  // Models
  glBindVertexArray(vao);
  for(int i=0; i<3; i++)
  {
    int start, end;
    if(i == 0)
    {
      start = 0;
      end = number_points_object[0];
    }
    else
    {
      start = number_points_object[i-1];
      end = number_points_object[i];
    }
    scaling_matrix = glm::scale(glm::mat4(), scaling[i]);

    centroid_each_object[i] = centroid_object(start, end);
    centroid_each_object[i] = scaling_matrix*centroid_each_object[i];

    rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation[i].x), glm::vec3(1.0f,0.0f,0.0f));
    rotation_matrix = glm::rotate(rotation_matrix, glm::radians(rotation[i].y), glm::vec3(0.0f,1.0f,0.0f));
    rotation_matrix = glm::rotate(rotation_matrix, glm::radians(rotation[i].z), glm::vec3(0.0f,0.0f,1.0f));

    orig_trans_matrix = glm::translate(glm::mat4(), glm::vec3(-centroid_each_object[i].x,-centroid_each_object[i].y,-centroid_each_object[i].z));
    centroid_trans_matrix = glm::translate(glm::mat4(), glm::vec3(centroid_each_object[i].x,centroid_each_object[i].y,centroid_each_object[i].z));

    translation_matrix = glm::translate(glm::mat4(), translation[i]);

    model_matrix = translation_matrix*centroid_trans_matrix*rotation_matrix*orig_trans_matrix*scaling_matrix;

    modelview_matrix = projection_matrix*lookat_matrix*view_matrix*model_matrix;

    glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(modelview_matrix));

    if(key_pressed == 5 || key_pressed == 6 || key_pressed == 7)
    {
      glBindVertexArray(vao_clip);
      if(i == 0)
      {
        start = 0;
        end = number_points_object_final[0];
      }
      else
      {
        start = number_points_object_final[i-1];
        end = number_points_object_final[i];
      }
      glDrawArrays(GL_TRIANGLES, start, end-start);
    }
    else
      glDrawArrays(GL_TRIANGLES, start, end-start);
  }
}

glm::vec3 load_each_line()
{
  float x, y, z;
  std::string line;
  getline(file_ptr, line, ' ');
  x = strtof(line.c_str(), NULL);

  getline(file_ptr, line, ' ');
  y = strtof(line.c_str(), NULL);

  getline(file_ptr, line);
  z = strtof(line.c_str(), NULL);

  return glm::vec3(x, y, z);
}

void load_l_r_t_b_n_f()
{
  std::string line;
  getline(file_ptr, line, ' ');
  L = strtof(line.c_str(), NULL);

  getline(file_ptr, line, ' ');
  R = strtof(line.c_str(), NULL);

  getline(file_ptr, line, ' ');
  T = strtof(line.c_str(), NULL);

  getline(file_ptr, line);
  B = strtof(line.c_str(), NULL);

  getline(file_ptr, line, ' ');
  N = strtof(line.c_str(), NULL);

  getline(file_ptr, line);
  F = strtof(line.c_str(), NULL);
}

void load_model(std::string line, int object_index)
{
  std::ifstream file_model(line.c_str());
  if(file_model.is_open())
  {
    float x, y, z, r, g, b;
    while(getline(file_model, line, ','))
    {
      x = strtof(line.c_str(), NULL);
      getline(file_model, line, ',');

      y = strtof(line.c_str(), NULL);
      getline(file_model, line, ',');

      z = strtof(line.c_str(), NULL);
      getline(file_model, line, ',');

      r = strtof(line.c_str(), NULL);
      getline(file_model, line, ',');

      g = strtof(line.c_str(), NULL);
      getline(file_model, line);

      b = strtof(line.c_str(), NULL);

      points[no_points] = glm::vec4(x, y, z, 1.0f);
      colors[no_points] = glm::vec4(r, g, b, 1.0f);
      no_points++;
    }
    number_points_object[object_index] = no_points;
  }
}

void load_scene()
{
  if(file_ptr.is_open())
  {
    std::string line;
    float x, y, z;
    for(int j=0; j<3; j++)
    {
      getline(file_ptr, line);
      load_model(line, j); // Should load the pts and clr and also the number of points(or vertices)

      scaling[j] = load_each_line();
      rotation[j] = load_each_line();
      translation[j] = load_each_line();
    }

    eye = load_each_line();
    lookat = load_each_line();
    up = load_each_line();

    load_l_r_t_b_n_f();

    file_ptr.close();
  }
}

void create_frustum()
{
  n = glm::normalize(eye - lookat);
  u = glm::normalize(glm::cross(up, n));
  v = glm::cross(n, u);

  glm::vec4 eye_vec4(eye.x, eye.y, eye.z, 1.0f);
  frustum_corners[0] = glm::vec4(eye + L*u + B*v - N*n, 1.0f);
  frustum_corners[1] = glm::vec4(eye + L*u + T*v - N*n, 1.0f);
  frustum_corners[2] = glm::vec4(eye + R*u + T*v - N*n, 1.0f);
  frustum_corners[3] = glm::vec4(eye + R*u + B*v - N*n, 1.0f);

  frustum_corners[4] = ((frustum_corners[0] - eye_vec4) * (F/N)) + eye_vec4;
  frustum_corners[5] = ((frustum_corners[1] - eye_vec4) * (F/N)) + eye_vec4;
  frustum_corners[6] = ((frustum_corners[2] - eye_vec4) * (F/N)) + eye_vec4;
  frustum_corners[7] = ((frustum_corners[3] - eye_vec4) * (F/N)) + eye_vec4;

  points_frustum[0] = eye_vec4;
  colors_frustum[0] = red;
  points_frustum[1] = eye_vec4;
  colors_frustum[1] = red;

  for(int i=2; i<10; i+=2)
  {
    points_frustum[i] = eye_vec4;
    colors_frustum[i] = magenta;
    points_frustum[i+1] = frustum_corners[(i-2)/2];
    colors_frustum[i+1] = magenta;
  }

  for(int i=10, j=0; i<18; i+=2, j++)
  {
    points_frustum[i] = frustum_corners[j%4];
    colors_frustum[i] = cyan;
    points_frustum[i+1] = frustum_corners[(j + 1)%4];
    colors_frustum[i+1] = cyan;
  }

  for(int i=18, j=0; i<26; i+=2, j++)
  {
    points_frustum[i] = frustum_corners[j%4];
    colors_frustum[i] = cyan;
    points_frustum[i+1] = frustum_corners[j + 4];
    colors_frustum[i+1] = cyan;
  }

  for(int i=26, j=0; i<34; i+=2, j++)
  {
    points_frustum[i] = frustum_corners[(j % 4) + 4];
    colors_frustum[i] = cyan;
    points_frustum[i+1] = frustum_corners[((j + 1)%4) + 4];
    colors_frustum[i+1] = cyan;
  }
}

void generate_matrices()
{ // Note the column major order
  wcs_vcs = glm::mat4(u.x, v.x, n.x, 0.0f,
                      u.y, v.y, n.y, 0.0f,
                      u.z, v.z, n.z, 0.0f,
                      -glm::dot(u, eye), -glm::dot(v, eye), -glm::dot(n, eye), 1.0);

  vcs_ccs = glm::mat4(2*N/(R-L), 0.0f, 0.0f, 0.0f,
                      0.0f, 2*N/(T-B), 0.0f, 0.0f,
                      (R+L)/(R-L), (T+B)/(T-B), -(F+N)/(F-N), -1.0f,
                      0.0f, 0.0f, -2*F*N/(F-N), 0.0f);

  ccs_ndcs = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                       0.0f, 1.0f, 0.0f, 0.0f,
                       0.0f, 0.0f, 1.0f, 0.0f,
                       0.0f, 0.0f, 0.0f, 1.0f);

  // ndcs_dcs = glm::mat4((Rw-Lw)/2, 0.0f, 0.0f, 0.0f,
  //                      0.0f, (Tw-Bw)/2, 0.0f, 0.0f,
  //                      0.0f, 0.0f, 0.5f, 0.0f,
  //                      (Rw+Lw)/2, (Tw+Bw)/2, 0.5f, 1.0f);
  ndcs_dcs = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                       0.0f, 1.0f, 0.0f, 0.0f,
                       0.0f, 0.0f, 0.0f, 0.0f,
                       0.0f, 0.0f, 0.0f, 1.0f);
                       // (Rw+Lw)/2, (Tw+Bw)/2, 0.5f, 1.0f);
}

int main(int argc, char** argv)
{
  //! The pointer to the GLFW window
  GLFWwindow* window;

  //! Setting up the GLFW Error callback
  glfwSetErrorCallback(csX75::error_callback);

  //! Initialize GLFW
  if (!glfwInit())
    return -1;

  //We want OpenGL 4.0
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //This is for MacOSX - can be omitted otherwise
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
  //We don't want the old OpenGL 
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

  //! Create a windowed mode window and its OpenGL context
  window = glfwCreateWindow(512, 512, "CS475/CS675 Assignment 2", NULL, NULL);
  if (!window)
    {
      glfwTerminate();
      return -1;
    }
  
  //! Make the window's context current 
  glfwMakeContextCurrent(window);

  //Initialize GLEW
  //Turn this on to get Shader based OpenGL
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err)
    {
      //Problem: glewInit failed, something is seriously wrong.
      std::cerr<<"GLEW Init Failed : %s"<<std::endl;
    }

  //Print and see what context got enabled
  std::cout<<"Vendor: "<<glGetString (GL_VENDOR)<<std::endl;
  std::cout<<"Renderer: "<<glGetString (GL_RENDERER)<<std::endl;
  std::cout<<"Version: "<<glGetString (GL_VERSION)<<std::endl;
  std::cout<<"GLSL Version: "<<glGetString (GL_SHADING_LANGUAGE_VERSION)<<std::endl;

  //Keyboard Callback
  glfwSetKeyCallback(window, csX75::key_callback);
  //Framebuffer resize callback
  glfwSetFramebufferSizeCallback(window, csX75::framebuffer_size_callback);

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  //Initialize GL state
  csX75::initGL();

  // Loading the files and generating frustum and matrices
  load_scene();
  create_frustum();
  generate_matrices();

  initBuffersGL();

  // Loop until the user closes the window
  while (glfwWindowShouldClose(window) == 0)
    {
       
      // Render here
      renderGL();

      // Swap front and back buffers
      glfwSwapBuffers(window);
      
      // Poll for and process events
      glfwPollEvents();
    }
  
  glfwTerminate();
  return 0;
}

//-------------------------------------------------------------------------

