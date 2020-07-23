// Request GLSL 3.3
#version 330

// Inputs from vertex shader
// Tex coord
in vec2 fragTexCoord;
// Normal (in world space)
in vec3 fragNormal;
// Position (in world space)
in vec3 fragWorldPos;

// This corresponds to the output color to the color buffer
out vec4 outColor;

// This is used for the texture sampling
uniform sampler2D uTexture;

// Create a struct for directional light
struct DirectionalLight
{
	// Direction of light
	vec3 mDirection;
	// Diffuse color
	vec3 mDiffuseColor;
	// Specular color
	vec3 mSpecColor;
};

// Uniforms for lighting
// Camera position (in world space)
uniform vec3 uCameraPos;
// Specular power for this surface
uniform float uSpecPower;
// Ambient light level
uniform vec3 uAmbientLight;

// Directional Light
uniform DirectionalLight uDirLight;

// create struct for point light
struct PointLight
{
	// position of light
	vec3 mPos;
	vec3 mDiffuseColor;
	vec3 mSpecColor;
	float mSpecPower;
	float mRadiusInfluence;
};

// Point Lights
uniform PointLight uPointLight0;
uniform PointLight uPointLight1;

void main()
{
	// Surface normal
	vec3 N = normalize(fragNormal);
	// Vector from surface to light
	vec3 L = normalize(-uDirLight.mDirection);
	// Vector from surface to camera
	vec3 V = normalize(uCameraPos - fragWorldPos);
	// Reflection of -L about N
	vec3 R = normalize(reflect(-L, N));

	// Compute phong reflection
	vec3 Phong = uAmbientLight;
	float NdotL = dot(N, L);
	if (NdotL > 0)
	{
		vec3 Diffuse = uDirLight.mDiffuseColor * NdotL;
		vec3 Specular = uDirLight.mSpecColor * pow(max(0.0, dot(R, V)), uSpecPower);
		Phong += Diffuse + Specular;
	}

	// set up point light array
	PointLight pointLights[2];
	pointLights[0] = uPointLight0;
	pointLights[1] = uPointLight1;

	// compute for all point lights
	for(int i = 0; i < 2; ++i)
	{
		// vector from surface to light
		vec3 PL = normalize(pointLights[i].mPos - fragWorldPos);
		// reflection of -L about N
		vec3 PR = normalize(reflect(-PL, N));
		// compute phong reflection from point light
		float PNdotPL = dot(N, PL);
		if (PNdotPL > 0)
		{
			// use sphere of influence
			float distanceFromLight = length(fragWorldPos - pointLights[i].mPos);
			if (distanceFromLight < pointLights[i].mRadiusInfluence)
			{
				vec3 Diffuse = pointLights[i].mDiffuseColor * PNdotPL;
				vec3 Specular = pointLights[i].mSpecColor * pow(max(0.0, dot(PR, V)), pointLights[i].mSpecPower);
				Phong += Diffuse + Specular;
			}
		}
	}

	// Final color is texture color times phong light (alpha = 1)
    outColor = texture(uTexture, fragTexCoord) * vec4(Phong, 1.0f);
}
