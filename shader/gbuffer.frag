#version 330 core

layout(location = 0) out vec4 gAlbedoMetallic;
layout(location = 1) out vec4 gNormalRoughness;
layout(location = 2) out vec4 gMotionShadingModel;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 PrevClipPos;
in vec4 CurrClipPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_metallic1;
uniform sampler2D texture_roughness1;

uniform float metallic     = 0.0;
uniform float roughness    = 0.5;
uniform int   shadingModel = 0;

vec2 calculateMotionVector()
{
    vec2 currNDC = (CurrClipPos.xy / CurrClipPos.w) * 0.5 + 0.5;
    vec2 prevNDC = (PrevClipPos.xy / PrevClipPos.w) * 0.5 + 0.5;
    return currNDC - prevNDC;
}

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(texture_normal1, TexCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoord);
    vec2 st2 = dFdy(TexCoord);

    vec3 N   = normalize(Normal);
    vec3 T   = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B   = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
    vec3  albedo         = texture(texture_diffuse1, TexCoord).rgb;
    float metallicValue  = texture(texture_metallic1, TexCoord).r * metallic;
    float roughnessValue = texture(texture_roughness1, TexCoord).r * roughness;

    vec3 normal       = getNormalFromMap();
    vec2 motionVector = calculateMotionVector();

    gAlbedoMetallic     = vec4(albedo, metallicValue);
    gNormalRoughness    = vec4(normal * 0.5 + 0.5, roughnessValue);
    gMotionShadingModel = vec4(motionVector, float(shadingModel) / 255.0, 1.0);
}