#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gAlbedoMetallic;
uniform sampler2D gNormalRoughness;
uniform sampler2D gMotionShadingModel;
uniform sampler2D gDepth;

uniform mat4 invViewProjection;
uniform vec3 viewPos;

struct DirectionalLight
{
    vec3  direction;
    vec3  color;
    float intensity;
};

struct PointLight
{
    vec3  position;
    vec3  color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

#define MAX_DIR_LIGHTS 4
#define MAX_POINT_LIGHTS 32

uniform int              numDirLights;
uniform int              numPointLights;
uniform DirectionalLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight       pointLights[MAX_POINT_LIGHTS];

vec3 reconstructWorldPos(vec2 texCoord, float depth)
{
    vec4 clipPos  = vec4(texCoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    vec4 worldPos = invViewProjection * clipPos;
    return worldPos.xyz / worldPos.w;
}

float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom       = 3.14159265359 * denom * denom;

    return nom / denom;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = geometrySchlickGGX(NdotV, roughness);
    float ggx1  = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) { return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0); }

vec3 calculateDirectionalLight(DirectionalLight light, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness)
{
    vec3 L        = normalize(-light.direction);
    vec3 H        = normalize(V + L);
    vec3 radiance = light.color * light.intensity;

    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);

    float NDF = distributionGGX(N, H, roughness);
    float G   = geometrySmith(N, V, L, roughness);

    vec3  numerator   = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3  specular    = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / 3.14159265359 + specular) * radiance * NdotL;
}

vec3 calculatePointLight(PointLight light, vec3 fragPos, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness)
{
    vec3  L           = normalize(light.position - fragPos);
    vec3  H           = normalize(V + L);
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3  radiance    = light.color * light.intensity * attenuation;

    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);

    float NDF = distributionGGX(N, H, roughness);
    float G   = geometrySmith(N, V, L, roughness);

    vec3  numerator   = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3  specular    = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / 3.14159265359 + specular) * radiance * NdotL;
}

void main()
{
    vec4  albedoMetallic  = texture(gAlbedoMetallic, TexCoord);
    vec4  normalRoughness = texture(gNormalRoughness, TexCoord);
    float depth           = texture(gDepth, TexCoord).r;

    if (depth >= 1.0)
    {
        discard;
    }

    vec3  albedo    = albedoMetallic.rgb;
    float metallic  = albedoMetallic.a;
    vec3  normal    = normalize(normalRoughness.rgb * 2.0 - 1.0);
    float roughness = normalRoughness.a;

    vec3 fragPos = reconstructWorldPos(TexCoord, depth);
    vec3 V       = normalize(viewPos - fragPos);

    vec3 Lo = vec3(0.0);

    for (int i = 0; i < numDirLights && i < MAX_DIR_LIGHTS; ++i)
    {
        Lo += calculateDirectionalLight(dirLights[i], normal, V, albedo, metallic, roughness);
    }

    for (int i = 0; i < numPointLights && i < MAX_POINT_LIGHTS; ++i)
    {
        Lo += calculatePointLight(pointLights[i], fragPos, normal, V, albedo, metallic, roughness);
    }

    vec3 ambient = vec3(0.03) * albedo;
    vec3 color   = ambient + Lo;

    FragColor = vec4(color, 1.0);
}