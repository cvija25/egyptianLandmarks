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
uniform bool shadows;

float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // ise the fragment to light vector to sample from the depth map
    float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);

    return shadow;
}

void main()
{
    vec3 lightColor = vec3(0.3);
    vec3 colorDI = texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
    vec3 colorSP = texture(material.texture_specular1, fs_in.TexCoords).rgb;

    vec3 normal = normalize(fs_in.Normal);

    vec3 viewDir = normalize(viewPosition - fs_in.FragPos);

    vec3 lightDir = normalize(lightPos - fs_in.FragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading blinn
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    vec3 ambient = 0.3 * colorDI;
    vec3 diffuse = diff * lightColor;
    vec3 specular = spec * lightColor;

    float shadow = shadows ? ShadowCalculation(fs_in.FragPos) : 0.0;
    vec3 lighting = ambient*vec3(colorDI) + (1.0 - shadow) * (diffuse*vec3(colorDI) + specular*vec3(colorSP));

    FragColor = vec4(lighting, 1.0);
}