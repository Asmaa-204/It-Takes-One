#version 330 core

#define MAX_LIGHTS 20

// Light Types
#define DIRECTIONAL 1
#define POINT 3
#define SPOT 2

//##########################################
// Data structures
//##########################################

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Fade {
    float constant;
    float linear;
    float quadratic;
};

struct Light {
    int type;

    // Fading Features
    Fade fade;

    // Light Features
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // Spatial Features
    vec3 position;
    vec3 direction;

    // Spot Light Angles
    float cutOff;
    float outerCutOff;
};

struct FragInfo {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 normal;
    vec3 emission;
    float shininess;
};

//##########################################
// Functions
//##########################################

float calculateAttenuation(float distance, Fade fade)
{
    return 1.0 / (fade.constant + fade.linear * distance + fade.quadratic * (distance * distance));
}

vec3 processDirectionalLight(Light light, FragInfo info, vec3 viewDir)
{
    vec3 normal = info.normal;
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), info.shininess);

    vec3 ambient = light.ambient * info.diffuse;
    vec3 diffuse = light.diffuse * diff * info.diffuse;
    vec3 specular = light.specular * spec * info.specular;

    return (ambient + diffuse + specular);
}

vec3 processPointLight(Light light, FragInfo info, vec3 fragPos, vec3 viewDir)
{
    vec3 normal = info.normal;
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), info.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = calculateAttenuation(distance, light.fade);

    vec3 ambient = light.ambient * info.diffuse;
    vec3 diffuse = light.diffuse * diff * info.diffuse;
    vec3 specular = light.specular * spec * info.specular;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 processSpotLight(Light light, FragInfo info, vec3 fragPos, vec3 viewDir)
{
    vec3 normal = info.normal;
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), info.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = calculateAttenuation(distance, light.fade);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * info.diffuse;
    vec3 diffuse = light.diffuse * diff * info.diffuse;
    vec3 specular = light.specular * spec * info.specular;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}

//##########################################
// Inputs / Outputs
//##########################################

in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
    vec3 frag_pos;
} fs_in;

out vec4 frag_color;

//##########################################
// Uniforms
//##########################################

uniform Material material;
uniform vec3 cameraPos;

uniform int n_lights;
uniform Light lights[MAX_LIGHTS];

Light test_light;

//##########################################
// Main
//##########################################

void main() 
{
    vec3 result = vec3(0.0);

    // Test Light
    test_light.type = POINT;
    test_light.fade.constant = 1.0;
    test_light.fade.linear = 0.09;
    test_light.fade.quadratic = 0.032;

    test_light.ambient = vec3(1, 1, 1);
    test_light.diffuse = vec3(1, 1, 1);
    test_light.specular = vec3(1.0, 1.0, 1.0);

    test_light.position = vec3(0.0, 5.0, 5.0);
    test_light.direction = vec3(0.0, -1.0, 0.0); // Only relevant for SPOT or DIRECTIONAL
    test_light.cutOff = cos(radians(12.5));     // Ignored for point lights
    test_light.outerCutOff = cos(radians(17.5)); // Ignored for point lights
    // Test Light

    FragInfo info;
    info.normal = normalize(fs_in.normal);
    info.diffuse = vec3(texture(material.diffuse, fs_in.tex_coord));
    info.specular = vec3(texture(material.specular, fs_in.tex_coord));
    info.shininess = material.shininess;
    info.emission = vec3(texture(material.emission, fs_in.tex_coord));

    vec3 viewDir = normalize(cameraPos - fs_in.frag_pos);

    for (int i = 0; i < n_lights; i++)
    {
        if (lights[i].type == POINT)
            result += processPointLight(lights[i], info, fs_in.frag_pos, viewDir);
        else if (lights[i].type == DIRECTIONAL)
            result += processDirectionalLight(lights[i], info, viewDir);
        else if (lights[i].type == SPOT)
            result += processSpotLight(lights[i], info, fs_in.frag_pos, viewDir);
    }


    result += info.emission;
    frag_color = vec4(result, 1.0);
}
