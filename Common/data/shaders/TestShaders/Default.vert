/*
 *  Doyoon Kim
 *  Graphic Class Project
 *  CS 250
 *  Spring 2021
 */

#version 450 core

layout (location=0) in vec3 vVertexPosition;

void main() 
{
    gl_Position = vec4(vVertexPosition, 1.0);
}
