//
//  main.cpp
//  box2d-03
//
//  Created by Yongduek Seo on 1/9/15.
//  Copyright (c) 2015 Yongduek Seo. All rights reserved.
//

// now draws a circle
//
// box2d-02
// Box2D physical simulation for rectangular objects.
// - body creation
// - body deletion
// - Meter to Pixel scaling and vice versa.
// - Single colored.

// Mouse click callback for GLFW3
//

#include <vector>
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

#include <Box2D/Box2D.h>
const float M2P=20; // Meter to Pixel
const float P2M=1/M2P;
b2World* b2world;


// window size
const float width=1920, height=1080;

// 2d box

GLfloat g_color_buffer_data[] = {
    1.0, 1.0, 0.8
};

// unit 2d square at (0,0)
const GLuint nVertex=4;
const GLfloat g_vertex_buffer_data[3*nVertex] = {
    -.5f, -.5f, 0.f,
    .5f, -.5f, 0.f,
    .5f,  .5f, 0.f,
    -.5f,  .5f, 0.f
};


GLuint vertexbuffer;
GLuint colorbuffer;

GLuint programID;
GLuint MatrixID_MVP;
GLuint uniformID_Color;
GLuint VertexArrayID;

my::MVPTransform mvpt;
my::FPSControl fps;


double random(int l, int h) {
    double d = rand()/(double)RAND_MAX; // [0,1)
    double y=(h-l)*d + l;
    return y;
}

// unit 2d circle at (0,0)
GLuint nVertexCircle;
GLfloat *g_vertex_buffer_circle; // recommand: std::vector
                                 // Cube color - not used yet.

void makeCircle(int sides, float radius=1.f) {
    nVertexCircle = sides;
    g_vertex_buffer_circle = new GLfloat[(nVertexCircle=3*(sides+1))]; // side + center
    g_vertex_buffer_circle[0] = g_vertex_buffer_circle[1]=g_vertex_buffer_circle[2]=0.0f; // center location
    for (int i=0; i<sides; i++) {
        g_vertex_buffer_circle[3*(1+i)+0] = radius * cos(i* M_PI/180.f * 360.f/(float)sides);
        g_vertex_buffer_circle[3*(1+i)+1] = radius * sin(i* M_PI/180.f * 360.f/(float)sides);
        g_vertex_buffer_circle[3*(1+i)+0] = 0.f;
    }
}

void myDrawCircle(int x, int y, int r, float degree)
{

}
void myDrawRect(int x0, int y0, int w, int h, float degree)
{
    float scalex = w, scaley=h;
    
    // mvpt.Projection does not change
    
    glm::mat4 Scale= glm::mat4(1.0f);
    Scale[0][0]=scalex; Scale[1][1]=scaley;
    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(x0,y0,0));
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), degree, glm::vec3(0,0,1));
    
    mvpt.Model = T*R*Scale;
    
    // Send our transformation to the currently bound shader,
    glUniformMatrix4fv(MatrixID_MVP, 1, GL_FALSE, mvpt.pMVP());
    
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

    // now, draw command.
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}


void myDrawSquare(int x0, int y0, int side, float degree) {
    myDrawRect(x0, y0, side, side, degree);
}


void myDrawRect (float *pMVP)
{
    // Send our transformation to the currently bound shader,
    glUniformMatrix4fv(MatrixID_MVP, 1, GL_FALSE, pMVP);

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
    
    // now, draw command.
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

b2Body* addRect(int x/*pixel*/,int y,int w,int h,bool dyn=true)
{
    b2BodyDef bodydef;
    bodydef.position.Set(x*P2M,y*P2M);
    bodydef.type= dyn? b2_dynamicBody : b2_staticBody;
    b2Body* body=b2world->CreateBody(&bodydef);
    
    b2PolygonShape shape;
    shape.SetAsBox(P2M*w/2,P2M*h/2);
    
    b2FixtureDef fixturedef;
    fixturedef.shape=&shape;
    fixturedef.density=1.0 + random(0,10); // mass
    fixturedef.restitution=0.8; // bouncy
    fixturedef.friction=0.1 + random(-0.1, 0.75);
    body->CreateFixture(&fixturedef);
    return body;
}

b2Body* addCircle(int x/*pixel*/,int y,int r, bool dyn=true)
{
    b2BodyDef bodydef;
    bodydef.position.Set(x*P2M,y*P2M);
    bodydef.type= dyn? b2_dynamicBody : b2_staticBody;
    b2Body* body=b2world->CreateBody(&bodydef);
    
    b2CircleShape shape;
    shape.m_radius = r*P2M;
    //    shape.m_p = b2Vec2(x*P2M,y*P2M);
    
    b2FixtureDef fixturedef;
    fixturedef.shape=&shape;
    fixturedef.density=1.0 + random(0,10); // mass
    fixturedef.restitution=0.8; // bouncy
    fixturedef.friction=0.1 + random(-0.1, 0.75);
    body->CreateFixture(&fixturedef);
    return body;
}

void drawCircle(b2Vec2 c, float r, float radian)
{
    printf ("circle found: (%.1f %.1f; %.1f; %.1f)\n",
            c.x*M2P, c.y*M2P, r*M2P, radian);
 
    float scale = r/1.f * M2P;
    
    // mvpt.Projection does not change
    
    glm::mat4 Scale= glm::mat4(1.0f);
    Scale[0][0]=Scale[1][1]=scale;
    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(c.x*M2P, c.y*M2P,0));
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), (float)(radian*180./M_PI), glm::vec3(0,0,1));
    
    mvpt.Model = T*R*Scale;

//    myDrawCircle(c.x, c.y, r, angle);
    myDrawRect(mvpt.pMVP());
}
void drawRect(b2Vec2 points[4],b2Vec2 center,float radian)
{
    //  these are exactly the same as the
    //    for (int i=0; i<4; i++)
    //        printf ("[%d] %.1f %.1f\n", i, points[i].x*M2P, points[i].y*M2P);
    //    printf("----------\n");
    //
    
    
    float scalex = points[2].x/0.5f * M2P,
          scaley= points[2].y/0.5f * M2P;
    
    // mvpt.Projection does not change
    
    glm::mat4 Scale= glm::mat4(1.0f);
    Scale[0][0]=scalex; Scale[1][1]=scaley;
    glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(center.x*M2P, center.y*M2P,0));
    glm::mat4 R = glm::rotate(glm::mat4(1.0f), (float)(radian*180./M_PI), glm::vec3(0,0,1));
    
    mvpt.Model = T*R*Scale;

    myDrawRect(mvpt.pMVP());
    
    return;
}

void setupBox2D()
{
    b2world=new b2World(b2Vec2(0.0,-9.81));
    addRect(width/2,50, width,30, false);   // bottom solid
    
    addRect(200,700, 20,20, true); // 40x40 square
    addCircle(900, 700, 50);
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
    
    // make a circle
    makeCircle(10,0.5);
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

void displayOpenGL()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use our shader
    glUseProgram(programID);
    
    // Projection matrix : this is a 2d image plane
    mvpt.Projection = glm::ortho(.0f, width, 0.f, height, -1.f, 1.f);
    // Camera motion matrix
    mvpt.View       = glm::mat4(1.0f);
    

    static float degree=0;
    myDrawSquare(width/2, height-20, 100, degree+=5);
    
    // ----------------------------------------------------------------------------
    // box2d
    std::vector<b2Body*> remove;
    for (b2Body *body=b2world->GetBodyList(); body!=NULL; body=body->GetNext())
    {
        b2Vec2 center=M2P*body->GetWorldCenter();
        if (center.x<0 || center.x>width || center.y<0)
            remove.push_back(body);

        b2Shape *shape = body->GetFixtureList()->GetShape();
        //
        // circle
        //
        if (shape->GetType()==b2Shape::e_circle) {
            b2CircleShape *c = (b2CircleShape*)shape;
            drawCircle(body->GetWorldCenter(),
                       c->m_radius, body->GetAngle());
        }
        else if (shape->GetType() == b2Shape::e_polygon) {
            b2Vec2 points[4];
            for (int i=0; i<4; i++)
                points[i] = ((b2PolygonShape*)(shape))->GetVertex(i);
            drawRect (points, body->GetWorldCenter(), body->GetAngle());
        }
    }
    // remove boxes outside the view-rectangle
    for (int i=0; i<remove.size(); i++)
        b2world->DestroyBody(remove[i]);
    // ----------------------------------------------------------------------------
    
    // done.
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y); // top-left is (0,0)
    y = height-y;
    if (action==GLFW_PRESS) {
        printf("mouse: %d %d\n", (int)x, (int)y);
        for (int i=0; i<10; i++)
            addRect(x+random(-50,50),y+random(-50,50),
                    10+random(-3,55), 20+random(-15,15), true); // 40x40 square
    }
    
}

int main( void )
{
    srand((unsigned int)time(0));
    // Initialise GLFW
    if( !glfwInit() ) {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // necessary for Mac OS X
    
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow( width, height, "Box2D + OpenGL4: Boxes falling", NULL, NULL);
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
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    
    
    setupBox2D();
    setupOpenGL();
    
    do{
        // random box2d rectangles are falling freely
        //
        addRect(random(width/3-50,width/3), random(height-100,height),
                10+random(-3,55), 20+random(-15,15), true);
        addCircle(random(width/3-50,width/3), random(height-100,height), random(5,50), true);
        // rendering
        //
        displayOpenGL();
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // update box2d physics
        //
        b2world->Step(1.0/30.0,8,3);
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