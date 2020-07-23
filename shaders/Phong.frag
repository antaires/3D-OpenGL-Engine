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

// Point Light todo: make array
uniform PointLight uPointLight;

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

	// compute for point light
	// vector from surface to light
	vec3 PL = normalize(uPointLight.mPos - fragWorldPos);
	// reflection of -L about N
	vec3 PR = normalize(reflect(-PL, N));
	// compute phong reflection from point light
	float PNdotPL = dot(N, PL);
	if (PNdotPL > 0)
	{
		// use sphere of influence
		float distanceFromLight = length(fragWorldPos - uPointLight.mPos);
		if (distanceFromLight < uPointLight.mRadiusInfluence)
		{
			vec3 Diffuse = uPointLight.mDiffuseColor * PNdotPL;
			vec3 Specular = uPointLight.mSpecColor * pow(max(0.0, dot(PR, V)), uPointLight.mSpecPower);
			Phong += Diffuse + Specular;
		}
	}

	// Final color is texture color times phong light (alpha = 1)
    outColor = texture(uTexture, fragTexCoord) * vec4(Phong, 1.0f);
}
