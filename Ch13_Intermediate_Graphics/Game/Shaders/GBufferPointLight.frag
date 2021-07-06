#version 330

in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

// G 버퍼의 다양한 입력 텍스처
uniform sampler2D uGDiffuse;
uniform sampler2D uGNormal;
uniform sampler2D uGWorldPos;

// 점광원의 구조체
struct PointLight
{
	//광원의 위치
	vec3 mWorldPos;
	// 분산 색상
	vec3 mDiffuseColor;
	// 광원의 반지름
	float mInnerRadius;
	float mOuterRadius;
};

uniform PointLight uPointLight;
// 화면의 너비/높이 저장
uniform vec2 uScreenDimensions;

void main()
{
	// From this fragment, calculate the coordinate to sample into the G-buffer
	vec2 gbufferCoord = gl_FragCoord.xy / uScreenDimensions;
	
	// Sample from G-buffer
	vec3 gbufferDiffuse = texture(uGDiffuse, gbufferCoord).xyz;
	vec3 gbufferNorm = texture(uGNormal, gbufferCoord).xyz;
	vec3 gbufferWorldPos = texture(uGWorldPos, gbufferCoord).xyz;
	
	// Surface normal
	vec3 N = normalize(gbufferNorm);
	// Vector from surface to light
	vec3 L = normalize(uPointLight.mWorldPos - gbufferWorldPos);

	// Compute Phong diffuse component for the light
	vec3 Phong = vec3(0.0, 0.0, 0.0);
	float NdotL = dot(N, L);
	if (NdotL > 0)
	{
		// Get the distance between the light and the world pos
		float dist = distance(uPointLight.mWorldPos, gbufferWorldPos);
		// Use smoothstep to compute value in range [0,1]
		// between inner/outer radius
		float intensity = smoothstep(uPointLight.mInnerRadius,
									 uPointLight.mOuterRadius, dist);
		// The diffuse color of the light depends on intensity
		vec3 DiffuseColor = mix(uPointLight.mDiffuseColor,
								vec3(0.0, 0.0, 0.0), intensity);
		Phong = DiffuseColor * NdotL;
	}

	// Final color is texture color times phong light (alpha = 1)
	outColor = vec4(gbufferDiffuse * Phong, 1.0);
}
