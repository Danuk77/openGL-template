#version 330 core

struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light{
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 normal;
in vec3 fragmentPos;

uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

vec3 calculate_phong_lighting();
vec3 calculate_ambient_light();
vec3 calculate_diffuse_light(vec3 light_dir, vec3 normalised_normal);
vec3 calculate_specular_light(vec3 light_dir, vec3 normalised_normal);

void main()
{
    FragColor = vec4(calculate_phong_lighting(), 1.0f);
}

vec3 calculate_phong_lighting(){
    vec3 normalised_normal = normalize(normal);
    vec3 light_dir = normalize(lightPos - fragmentPos);

    vec3 ambient_light = calculate_ambient_light();
    vec3 diffuse_light = calculate_diffuse_light(light_dir, normalised_normal);
    vec3 specular_light = calculate_specular_light(light_dir, normalised_normal);

    return object_color * (ambient_light + diffuse_light + specular_light);
}


vec3 calculate_ambient_light(){
    return light.ambient * material.ambient;
}

vec3 calculate_diffuse_light(vec3 light_dir, vec3 normalised_normal){
    float diffuse_intensity_multiplier = dot(normalised_normal, light_dir);
    // The dot product may go below 0, hence handle the case
    float corrected_diffuse_intensity_multiplier = max(diffuse_intensity_multiplier, 0.0f);

    vec3 diffuse_intensity = light.diffuse * (material.diffuse * corrected_diffuse_intensity_multiplier);

    return diffuse_intensity;
}

vec3 calculate_specular_light(vec3 light_dir, vec3 normalised_normal){
    vec3 viewDir = normalize(viewPos - fragmentPos);
    vec3 reflectDir = reflect(-light_dir, normal);

    float reflected_light_intensity_multiplier = dot(viewDir, reflectDir);
    // The dot product may go below 0, hence handle the case
    float corrected_reflected_light_intensity_multiplier = max(reflected_light_intensity_multiplier, 0.0f);
    float specular_light_intensity_multiplier = pow(corrected_reflected_light_intensity_multiplier, material.shininess);

    vec3 specular_light_intensity = light.specular * (specular_light_intensity_multiplier * material.specular);

    return specular_light_intensity;
}