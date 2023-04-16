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

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform samplerCube depthMap;

uniform Material material;
uniform Light light;

uniform vec3 viewPosition;

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
    vec3 fragToLight = fragPos - light.position;
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
    vec3 colorDI = texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
    vec3 colorSP = texture(material.texture_specular1, fs_in.TexCoords).rgb;

    vec3 normal = normalize(fs_in.Normal);

    vec3 viewDir = normalize(viewPosition - fs_in.FragPos);

    vec3 lightDir = normalize(light.position - fs_in.FragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading blinn
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * colorDI;
    vec3 diffuse = diff * light.diffuse * colorDI;
    vec3 specular = spec * light.specular * colorSP;

    float distance = length(light.position - fs_in.FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    float shadow = ShadowCalculation(fs_in.FragPos);
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);

    FragColor = vec4(lighting, 1.0);
}