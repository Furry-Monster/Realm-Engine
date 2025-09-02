#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main() {
  vec3 color = texture(texture_diffuse1, TexCoord).rgb;

  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);

  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
  vec3 specular = spec * lightColor;

  vec3 ambient = 0.15 * color;
  vec3 result = ambient + diffuse * color + specular;

  FragColor = vec4(result, 1.0);
}