#version 330
// 버텍스 셰이더로부터의 입력
in vec2 fragTexCoord;	// 텍스처 좌표

layout(location = 0) out vec4 outColor;

// G 버퍼의 다양한 입력 텍스처
uniform sampler2D uGDiffuse;
uniform sampler2D uGNormal;
uniform sampler2D uGWorldPos;

// 방향광을 위한 구조체 정의
struct DirectionalLight
{
	// 빛의 방향
	vec3 mDirection;
	// 난반사 색상
	vec3 mDiffuseColor;
	// 정반사 색상
	vec3 mSpecColor;
}; 

//********* C++로 부터 입력 받는 uniform 부분**************
// 조명을 위한 uniform
// 세계 공간에서의 카메라 위치
uniform vec3 uCameraPos;
// 주변광
uniform vec3 uAmbientLight;
// 표면에 대한 정반사 지수
uniform float uSpecPower;
// 방향광(지금은 오직 하나)
uniform DirectionalLight uDirLight;

void main()
{
	// 분산 색상과 법선, 세계 위치를 G 버퍼로부터 샘플링
	vec3 gbufferDiffuse = texture(uGDiffuse, fragTexCoord).xyz;
	vec3 gbufferNorm = texture(uGNormal, fragTexCoord).xyz;
	vec3 gbufferWorldPos = texture(uGWorldPos, fragTexCoord).xyz;
	// Surface normal
	vec3 N = normalize(gbufferNorm);
	// Vector from surface to light
	vec3 L = normalize(-uDirLight.mDirection);
	// Vector from surface to camera
	vec3 V = normalize(uCameraPos - gbufferWorldPos);
	// Reflection of -L about N
	vec3 R = normalize(reflect(-L, N));

	// 퐁 반사 계산
	vec3 Phong = uAmbientLight;
	float NdotL = dot(N, L);
	if(NdotL > 0)
	{
		vec3 Diffuse = uDirLight.mDiffuseColor * NdotL;
		vec3 Specular = uDirLight.mSpecColor * 
						pow(max(0.0, dot(R, V)), uSpecPower);
		Phong += Diffuse + Specular;
	}

	// 최종 색상은 분산 색상 곱하기 퐁 광원
	outColor = vec4(gbufferDiffuse * Phong, 1.0f);

}