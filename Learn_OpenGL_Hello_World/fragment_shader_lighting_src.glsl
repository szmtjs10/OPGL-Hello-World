#version 330 core
	in vec3 Normal;
	in vec3 FragPos;
	in vec2 TexCoords;

	out vec4 FragColor;

	uniform vec3 viewPos;

	struct Material {
		// Ambient not necessary when using a diffuse map
		// vec3 ambient;
		// Replace vec3 diffuse with sampler2D type
		// vec3 diffuse;
		sampler2D diffuse;
		vec3 specular;
		float shininess;
	};
	uniform Material material;

	struct Light {
		vec3 position;

		vec3 ambient;
		vec3 diffuse;
		vec3 specular;

		// Implementing attenuation: f_att = 1.0 / (constant + linear*distance + quadratic*distance^2)
		float constant;
		float linear;
		float quadratic;
	};
	uniform Light light;

	struct SpotLight {
		bool on;
		vec3 position;
		vec3 direction;

		vec3 ambient;
		vec3 diffuse;
		vec3 specular;

		float constant;
		float linear;
		float quadratic;

		// Angle of spotlight
		float cutOff;
	};
	uniform SpotLight flashlight;

void main() {

	// Point light
	//
	// Phong model implementation
	// Includes ambient, diffuse, specular lighting components
	//
	// Ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	// Diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(0.0, dot(norm, lightDir));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	// Specular
	float shininess = 128;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = (spec * material.specular) * light.specular;
	// Attenuation computation
	float lightDistance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	// Flashlight
	// 
	// Compare light angle to cutOff
	if(flashlight.on) {
		vec3 flashlightDir = normalize(flashlight.position - FragPos);
		float theta = dot(flashlightDir, normalize(-flashlight.direction)); 
		if(theta > flashlight.cutOff) // inside the flashlight range
		{
			// testing
			// ambient += vec3(0.2f,0.2f,0.2f);
			ambient *= 3.2f;
		} 
		else // outside the flashlight range
		{
			// TODO
			// color = vec4(light.ambient * vec3(texture(material.diffuse, TexCoords)), 1.0);
		}
	}


	// Combine
	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);

}