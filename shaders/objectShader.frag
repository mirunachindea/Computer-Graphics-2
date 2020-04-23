#version 410 core

in vec3 normal;
in vec4 fragPosEye;
in vec4 fragPosLightSpace;
in vec2 fragTexCoords;

out vec4 fColor;

//lighting
uniform	mat3 normalMatrix;
uniform mat3 lightDirMatrix;
uniform	vec3 lightColor;
uniform	vec3 lightColor2;
uniform	vec3 lightColor3;
uniform	vec3 lightDir;
uniform vec3 lightDir2;
uniform vec3 lightDir3;
uniform vec3 lightPos3;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;
uniform float cutOff;
uniform float outerCutOff;
uniform float intensityPoint;
uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform int polygonal;
vec3 ambient;
vec3 diffuse;
vec3 specular;

float ambientStrength = 0.2f;
float specularStrength = 0.9f;
float shininess = 64.0f;

float ambientStrengthSpot = 0.0f;
float specularStrengthSpot = 1.0f;

float constant = 1.0f;
float linear = 0.045;
float quadratic = 0.0075f;

float distance;
float attenuation;

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	vec3 normalEye = normalize(normalMatrix * normal);	
	vec3 lightDirN = normalize(lightDirMatrix * lightDir);	
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	vec3 halfVector = normalize(lightDirN + viewDirN);

	ambient = ambientStrength * lightColor;
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	float specCoeff = pow(max(dot(halfVector, normalEye), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;

	ambient *= vec3(texture(diffuseTexture, fragTexCoords));
	diffuse *= vec3(texture(diffuseTexture, fragTexCoords));
	specular *= vec3(texture(specularTexture, fragTexCoords));
}

void computeLightComponentsPoint(vec4 lightPos4)
{
	vec3 lightPos;
	lightPos.x = lightPos4.x; lightPos.y = lightPos4.y; lightPos.z = lightPos4.z;
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	vec3 normalEye = normalize(normalMatrix * normal);	
	vec3 lightDirN = normalize(lightPos - fragPosEye.xyz);	
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	vec3 refl = reflect(-lightDirN, normalEye);

	ambient = ambientStrength * lightColor2;
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor2;
	float specCoeff = pow(max(dot(viewDirN, refl), 0.0), shininess);
	specular = specularStrength * specCoeff * lightColor2;

	ambient *= vec3(texture(diffuseTexture, fragTexCoords));
	diffuse *= vec3(texture(diffuseTexture, fragTexCoords));
	specular *= vec3(texture(specularTexture, fragTexCoords));

	distance = length(lightPos - fragPosEye.xyz);
	attenuation = 1.0/(constant + linear*distance + quadratic*(distance * distance));
	ambient *= attenuation * intensityPoint;
	diffuse *= attenuation * intensityPoint;
	specular *= attenuation * intensityPoint;
}

void computeSpotLight(vec3 lightPos, vec3 lightDir)
{
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	vec3 normalEye = normalize(normalMatrix * normal);	
	vec3 lightDirN = normalize(lightPos - fragPosEye.xyz);	
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	vec3 refl = reflect(-lightDirN, normalEye);

	ambient = ambientStrengthSpot * lightColor3;
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor3;
	float specCoeff = pow(max(dot(viewDirN, refl), 0.0), shininess);
	specular = specularStrengthSpot * specCoeff * lightColor3;

	ambient *= vec3(texture(diffuseTexture, fragTexCoords));
	diffuse *= vec3(texture(diffuseTexture, fragTexCoords));
	specular *= vec3(texture(specularTexture, fragTexCoords));

	distance = length(lightPos - fragPosEye.xyz);
	attenuation = 1.0/(constant + linear*distance + quadratic*(distance * distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	float theta = dot(lightDirN, normalize(-lightDir));
	float epsilon = cutOff - outerCutOff;
	float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

	ambient *= intensity;
    diffuse *= intensity;
    specular *= intensity;

}

void computeLightPolygonal(){
	vec3 cameraPosEye = vec3(0.0f);
	vec3 normalEye = normalize(normalMatrix * normal);
	vec3 lightDirN = normalize(lightDirMatrix * lightDir);
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	
	ambient = ambientStrength * lightColor;
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	vec3 reflectDir = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflectDir), 0.0f), 32);
	specular = specularStrength * specCoeff * lightColor;
}



float computeShadow(vec3 lightDirection)
{	
	// perform perspective divide
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if(normalizedCoords.z > 1.0f)
        return 0.0f;
    // Transform to [0,1] range
    normalizedCoords = normalizedCoords * 0.5f + 0.5f;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;    
    // Get depth of current fragment from light's perspective
    float currentDepth = normalizedCoords.z;
    // Check whether current frag pos is in shadow
    //float bias =max(0.05f * (1.0f - dot(normal, lightDirection)), 0.005f);
	float bias = 0.005f;
    float shadow = currentDepth - bias> closestDepth  ? 1.0f : 0.0f;

    return shadow;	
}

float computeFog()
{
	float fogDensity = 0.015f;
	float fragmentDistance = length(fragPosEye);
	float fogFactor = exp(-pow(fragmentDistance*fogDensity, 2));

	return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
	if (polygonal == 0) {
	float shadow = computeShadow(lightDir);
	computeLightComponents();	
	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);

	vec4 lightDir2v = view * vec4(lightDir2, 1.0f);
	computeLightComponentsPoint(lightDir2v);
	color += ambient + diffuse + specular;
	
	computeSpotLight(lightPos3, lightDir3);
	color += ambient + diffuse + specular;

	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.3f, 0.3f, 0.5f, 1.0f);
	fColor = fogColor*(1 - fogFactor) + vec4(color, 1.0f)*fogFactor;
	}
	else {
	float shadow = computeShadow(lightDir);
	computeLightPolygonal();
	vec3 baseColor = vec3(1.0f, 1.0f, 0.0f);
	vec3 color = min((ambient + diffuse) * baseColor + specular, 1.0f);
	fColor=vec4 (color, 1.0f);
	}
}
