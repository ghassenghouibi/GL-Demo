#version 330

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float texRepeat;
uniform int inv;
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;
 
out vec2 vsoTexCoord;

void main(void) {
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vsiPosition.xyz, 1.0);
  vsoTexCoord = texRepeat * vec2(vsiTexCoord.x, (inv != 0) ? 1.0 - vsiTexCoord.y : vsiTexCoord.y);
}
