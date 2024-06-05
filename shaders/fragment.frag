#version 330 core

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light{
    vec3 position;
    vec3 direction;
    float cutoff;
    float outer_cutoff;

    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 normal;
in vec3 fragmentPos;
in vec2 texture_coordinates;

uniform vec3 object_color; 
uniform vec3 light_color;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

vec4 calculate_flashlight_lighting();
vec3 calculate_light_direction();
vec4 calculate_phong_lighting(vec3 light_dir, float intensity);
vec3 calculate_ambient_light(float attenuation);
vec3 calculate_diffuse_light(vec3 light_dir, vec3 normalised_normal, float attenuation, float intensity);
vec3 calculate_specular_light(vec3 light_dir, vec3 normalised_normal, float attenuation, float intensity);
float calculate_attenuation(vec3 light_direction);

void main()
{
    FragColor = calculate_flashlight_lighting();
}

vec4 calculate_flashlight_lighting(){
    vec4 color;
    vec3 light_direction = calculate_light_direction();
    vec3 spot_direction = normalize(-light.direction);

    float theta = dot(light_direction, spot_direction);
    float epsilon = light.cutoff - light.outer_cutoff;
    float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);

    color = calculate_phong_lighting(light_direction, intensity);
    return color;
}

vec3 calculate_light_direction(){
    return normalize(light.position - fragmentPos);
}

vec4 calculate_phong_lighting(vec3 light_dir, float intensity){
    vec3 normalised_normal = normalize(normal);
    float attenuation = calculate_attenuation(light_dir);

    vec3 ambient_light = calculate_ambient_light(attenuation);
    vec3 diffuse_light = calculate_diffuse_light(light_dir, normalised_normal, attenuation, intensity);
    vec3 specular_light = calculate_specular_light(light_dir, normalised_normal, attenuation, intensity);

    vec3 phong_lighting = (ambient_light + diffuse_light + specular_light);
    return vec4(phong_lighting, 1.0f);
}

float calculate_attenuation(vec3 light_direction){
    float distance = length(light.position - fragmentPos);
    
    float linear_factor = light.linear * distance;
    float quadratic_factor = light.quadratic * (distance * distance);

    float attenuation = 1.0f / (light.constant + linear_factor + quadratic_factor);
    return attenuation;
}


vec3 calculate_ambient_light(float attenuation){
    return attenuation * light.ambient * vec3(texture(material.diffuse, texture_coordinates));
}

vec3 calculate_diffuse_light(vec3 light_dir, vec3 normalised_normal, float attenuation, float intensity){
    float diffuse_intensity_multiplier = dot(normalised_normal, light_dir);
    // The dot product may go below 0, hence handle the case
    float corrected_diffuse_intensity_multiplier = max(diffuse_intensity_multiplier, 0.0f);

    vec3 diffuse_color = vec3(texture(material.diffuse, texture_coordinates));

    vec3 diffuse_intensity = light.diffuse * (diffuse_color * corrected_diffuse_intensity_multiplier);

    return intensity * attenuation * diffuse_intensity;
}

vec3 calculate_specular_light(vec3 light_dir, vec3 normalised_normal, float attenuation, float intensity){
    vec3 viewDir = normalize(viewPos - fragmentPos);
    vec3 reflectDir = reflect(-light_dir, normal);

    float reflected_light_intensity_multiplier = dot(viewDir, reflectDir);
    // The dot product may go below 0, hence handle the case
    float corrected_reflected_light_intensity_multiplier = max(reflected_light_intensity_multiplier, 0.0f);
    float specular_light_intensity_multiplier = pow(corrected_reflected_light_intensity_multiplier, material.shininess);

    vec3 specular_map_intensity = vec3(texture(material.specular, texture_coordinates));
    vec3 specular_light_intensity = light.specular * specular_light_intensity_multiplier * specular_map_intensity;

    return intensity * attenuation * specular_light_intensity;
}