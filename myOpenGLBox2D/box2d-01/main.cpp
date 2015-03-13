//
//  main.cpp
//  box2d-01
//
//  Created by Yongduek Seo on 2015. 1. 4..
//  Copyright (c) 2015년 Yongduek Seo. All rights reserved.
//

//
// A small 2D square is drawn with OpenGL Version > 3.3 in Mac OS X 10.10.1
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "../common/shader.hpp"
#include "../common/myControl.hpp"

// window size
const float width=1024, height=768;


// 2d box
const float  sideLength=30;

// unit 2d square at (0,0)
const GLuint nVertex=4;
GLfloat g_vertex_buffer_data[3*nVertex] = {
    -.5f, -.5f, 0.f,
     .5f, -.5f, 0.f,
     .5f,  .5f, 0.f,
    -.5f,  .5f, 0.f
};

// Cube color
GLfloat g_color_buffer_data[] = {
    1.0, 1.0, 0.8
};

GLuint vertexbuffer;
GLuint colorbuffer;

GLuint programID;
GLuint MatrixID_MVP;
GLuint uniformID_Color;
GLuint VertexArrayID;

my::MVPTransform mvpt;
my::FPSControl fps;


void myDrawRect(int x0, int y0, int w, int h, float degree) {
    float scalex = w, scaley=h;
    
    // mvpt.Projection does not change
    
    glm::mat4 Scale= glm::mat4(1.0f);
    Scale[0][0]=scalex; Scale[1][1]=scaley;
    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(x0,y0,0));
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), degree, glm::vec3(0,0,1));
    
    mvpt.Model = T*R*Scale;
    
    // Send our transformation to the currently bound shader,
    glUniformMatrix4fv(MatrixID_MVP, 1, GL_FALSE, mvpt.pMVP());
    
    // now, draw command.
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
}

void myDrawSquare(int x0, int y0, int side, float degree) {
    myDrawRect(x0, y0, side, side, degree);
}

void myDrawRect(int x0, int y0, int w, int h) {
    float scalex = w, scaley=h;

    // mvpt.Projection does not change
    
    mvpt.Model
    =
    glm::scale(glm::mat4(1.0f), glm::vec3(scalex, scaley, 1.f))  *
    glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.f));
    
    // Send our transformation to the currently bound shader,
    glUniformMatrix4fv(MatrixID_MVP, 1, GL_FALSE, mvpt.pMVP());
    
    // now, draw command.
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
}

void setupOpenGL() {
    
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    //    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    // Create and compile our GLSL program from the shaders
//    programID = LoadShaders( "../shaders/SimpleVertexShader.vertexshader",
//                             "../shaders/SimpleFragmentShader.fragmentshader" );

    programID = LoadShaders( "../shaders/box2d-01.vert", "../shaders/box2d-01.frag" );
    
    // Get a handle for our "MVP" uniform
    MatrixID_MVP = glGetUniformLocation(programID, "MVP");
    
    //    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    //    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, nVertex*3*sizeof(float), g_vertex_buffer_data, GL_STATIC_DRAW);
    
    //    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
    
    fps.setup(60/*fps*/);
}

float transX = 0;
void displayOpenGL() {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use our shader
    glUseProgram(programID);
    
    // the vertex info. in the graphic card.
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
                          0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                          3,                  // # of coordinates per vertex
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    // drawing a moving square
    {
    // Projection matrix : this is a 2d image plane
    mvpt.Projection = glm::ortho(.0f, width, 0.f, height, -1.f, 1.f);
    // Camera motion matrix
    mvpt.View       = glm::mat4(1.0f);
    // object motion matrix
    float dx=1;
    mvpt.Model = glm::translate( glm::mat4(1.0f), glm::vec3(transX+=dx,200,0) )
               * glm::scale(glm::mat4(1.0f), glm::vec3(sideLength, sideLength, sideLength));;
    if (transX>width) transX=0;
    
    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(MatrixID_MVP, 1, GL_FALSE, mvpt.pMVP());
    
    // now, draw command.
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    
    myDrawRect(0,0, width-1, 10); // big bar at the bottom
    
    static float degree=0;
    myDrawSquare(500, 700, 100, degree+=5);
    
    // done.
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    
}


int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
        {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
        }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow( width, height, "Tutorial 04 - Colored Cube", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    
    setupOpenGL();
    
    do{
        
        displayOpenGL();
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        fps.wait();
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    
    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
}