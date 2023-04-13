#version 330 core
out vec4 FragColor;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;

    float shininess;
};
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform DirLight dirLight;
uniform Material material;

uniform vec3 viewPosition;

void main()
{
    vec3 normal = normalize(fs_in.Normal);

    vec3 viewDir = normalize(viewPosition - fs_in.FragPos);

    vec3 lightDir = normalize(-dirLight.direction);

    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading blinn
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    vec3 ambient = dirLight.ambient * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
    vec3 diffuse = dirLight.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
    vec3 specular = dirLight.specular * spec * vec3(texture(material.texture_specular1, fs_in.TexCoords));

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}