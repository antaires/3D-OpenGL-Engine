// compute an output color for current pixel
// request GLSL 3.3
#version 330

// 4 components of RGBA
out vec4 outColor;

void main()
{
  // set to blue
  outColor = vec4(0.0, 0.0, 1.0, 1.0);
}
