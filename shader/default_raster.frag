#version 330 core
out vec4 FragColor;

struct Material
{
    sampler2D texture_diffuse;
    sampler2D texture_normal;
    float     shininess;
};

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3  position;
    vec3  direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float diffuse;
    float specular;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 32

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform Material material;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform DirLight   dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight  spotLight;

void main()
{
    vec3 color = texture(texture_diffuse1, TexCoord).rgb;

    vec3 norm     = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff    = max(dot(norm, lightDir), 0.0);
    vec3  diffuse = diff * lightColor;

    vec3  viewDir    = normalize(viewPos - FragPos);
    vec3  reflectDir = reflect(-lightDir, norm);
    float spec       = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3  specular   = spec * lightColor;

    vec3 ambient = 0.15 * color;
    vec3 result  = ambient + diffuse * color + specular;

    FragColor = vec4(result, 1.0);
}