#version 330

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
// ������ �ؽ�ó ��ǥ�� ������ ��� ���� �ؽ�ó ���÷� uniform
// sampler2D Ÿ���� 2D �ؽ�ó�� ���ø��� �� �ִ� Ư���� Ÿ���̴�.
// sampler uniform�� C++ �ڵ忡�� ���ε��� �ʿ����.
// �ֳ��ϸ� ���� ���������� �� ���� ���� �ϳ��� �ؽ�ó�� ���ε��ϱ� �����̴�.
// �׷��� OpenGL�� �ڵ��� ���̴��� �ؽ�ó ���÷��� Ȱ��ȭ�� �ؽ�ó�� �����ϰ� �������� �ȴ�.
uniform sampler2D uTexture;


//****** ���ؽ� ���̴��� ���� �Է� �޴� �κ�********
// ��Ģ������ ���ؽ� ���̴��� ��� out �������� 
// �����׸�Ʈ ���̴����� �̿� �ش��ϴ� in ������ ���� �־�� �Ѵ�.
// �����׸�Ʈ ���̴����� in ������ �̸��� Ÿ���� ���ؽ� ���̴������� 
// out ������ ��ġ�ϴ� ������ �̸��� Ÿ���� ���� �־�� �Ѵ�.

// �ؽ�ó�� ���εǴ� �ؽ�ó ��ǥ
in vec2 fragTexCoord;
// ���� ���������� ����
in vec3 fragNormal;
// ���� ���������� ��ġ
in vec3 fragWorldPos;


// ******* �����׸�Ʈ ���̴� ��� �κ�****************
// ��� ������ �����ϱ� ���� out ���� �����ڸ� ����ؼ� ���� ������ ����
out vec4 outColor;

void main()
{
	// ǥ�� ����. �� ���ؽ������� ����ȭ�� �������͸� ������ �ִ���
	// �ﰢ�� ǥ���� �������ʹ� ���ؽ��� ���� ���͸� �����Ͽ��� ����Ѵ�.
	// �̶� ������ ���� ����ȭ�� ���͸� �������� �ʱ� ������ ����ϱ� ���� ����ȭ �ܰ踦 ���ľ� �Ѵ�. 
	vec3 N = normalize(fragNormal);

	// ǥ�鿡�� �����Ƿ��� ����
	// ���Ɽ�� �� �������� �߻��ϴ� Ư¡�� �ִ�.
	// ������ ǥ�鿡�� �������� ���ϴ� ���ʹ� ������ ���� ���͸� ������Ű�� �ȴ�.
	vec3 L = normalize(-uDirLight.mDirection);

	// ǥ�鿡�� ī�޶�� ���ϴ� ����
	vec3 V = normalize(uCameraPos - fragWorldPos);

	// N�� ���� -L�� �ݻ�
	// GLSL������ �ݻ纤�͸� ���� �� �ִ� �Լ� reflect()�� �������ش�.
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

	// ���� ���� �ؼ�ó ���� ���ϱ� �� ���� (���İ� = 1)
	outColor = texture(uTexture, fragTexCoord) * vec4(Phong,1.0f);
}
