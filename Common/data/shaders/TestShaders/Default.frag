/*
 *  Doyoon Kim
 *  Graphic Class Project
 *  CS 250
 *  Spring 2021
 */

#version 450 core

layout (location = 0) in vec4 vPosition;

layout (location = 0) out vec4 fFragClr;

void main () {
  fFragClr = vec4(0.f, 1.f, 0.f, 1.f);
}
