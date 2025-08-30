#version 330 core
out vec4 FragColor;

in vec4 vertexColor;
in vec2 texCoord;

uniform sampler2D tex0;
uniform sampler2D tex1;

void main() {
  vec4 TexSample0 = vec4(texture(tex0, texCoord));
  vec4 TexSample1 = vec4(texture(tex1, texCoord));
  FragColor = mix(TexSample0, TexSample1, 0.5) * vertexColor;
}