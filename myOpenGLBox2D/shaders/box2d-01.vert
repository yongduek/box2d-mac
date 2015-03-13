//
//  box2d-01.vert
//  myOpenGLBox2D
//
//  Created by Yongduek Seo on 2015. 1. 4..
//  Copyright (c) 2015년 Yongduek Seo. All rights reserved.
//

#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){
    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
}

