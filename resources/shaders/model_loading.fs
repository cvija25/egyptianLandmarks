#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;

    float shininess;
};

uniform samplerCube depthMap;

uniform Material material;

uniform vec3 viewPosition;
uniform vec3 lightPos;

uniform float far_plane;

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos) {
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPosition - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}


void main()
{
    vec3 lightColor = vec3(0.5);
    vec3 colorDI = texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
    vec3 colorSP = texture(material.texture_specular1, fs_in.TexCoords).rgb;

    vec3 normal = normalize(fs_in.Normal);

    vec3 viewDir = normalize(viewPosition - fs_in.FragPos);

    vec3 lightDir = normalize(lightPos - fs_in.FragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading blinn
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    vec3 ambient = 0.4 * colorDI;
    vec3 diffuse = diff * lightColor;
    vec3 specular = spec * lightColor;

    float shadow = ShadowCalculation(fs_in.FragPos);
    vec3 lighting = ambient*vec3(colorDI) + (1.0 - shadow) * (diffuse*vec3(colorDI) + specular*vec3(colorSP));

    FragColor = vec4(lighting, 1.0);
}