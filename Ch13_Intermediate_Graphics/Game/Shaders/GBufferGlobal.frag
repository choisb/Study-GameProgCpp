#version 330
// ���ؽ� ���̴��κ����� �Է�
in vec2 fragTexCoord;	// �ؽ�ó ��ǥ

layout(location = 0) out vec4 outColor;

// G ������ �پ��� �Է� �ؽ�ó
uniform sampler2D uGDiffuse;
uniform sampler2D uGNormal;
uniform sampler2D uGWorldPos;

// ���Ɽ�� ���� ����ü ����
struct DirectionalLight
{
	// ���� ����
	vec3 mDirection;
	// ���ݻ� ����
	vec3 mDiffuseColor;
	// ���ݻ� ����
	vec3 mSpecColor;
}; 

//********* C++�� ���� �Է� �޴� uniform �κ�**************
// ������ ���� uniform
// ���� ���������� ī�޶� ��ġ
uniform vec3 uCameraPos;
// �ֺ���
uniform vec3 uAmbientLight;
// ǥ�鿡 ���� ���ݻ� ����
uniform float uSpecPower;
// ���Ɽ(������ ���� �ϳ�)
uniform DirectionalLight uDirLight;

void main()
{
	// �л� ����� ����, ���� ��ġ�� G ���۷κ��� ���ø�
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

	// �� �ݻ� ���
	vec3 Phong = uAmbientLight;
	float NdotL = dot(N, L);
	if(NdotL > 0)
	{
		vec3 Diffuse = uDirLight.mDiffuseColor * NdotL;
		vec3 Specular = uDirLight.mSpecColor * 
						pow(max(0.0, dot(R, V)), uSpecPower);
		Phong += Diffuse + Specular;
	}

	// ���� ������ �л� ���� ���ϱ� �� ����
	outColor = vec4(gbufferDiffuse * Phong, 1.0f);

}