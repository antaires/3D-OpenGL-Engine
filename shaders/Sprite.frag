// compute an output color for current pixel
// request GLSL 3.3
#version 330

// tex coord input from vertex shader
in vec2 fragTexCoord;

// 4 components of RGBA
out vec4 outColor;

// for texture sampling
uniform sampler2D uTexture;

void main()
{
  // sample color from texture
  outColor = texture(uTexture, fragTexCoord);
}
