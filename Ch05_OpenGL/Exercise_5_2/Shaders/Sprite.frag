#version 330

in vec2 fragTexCoord;
in vec3 vertexColor;	// ���ؽ� ���̴��κ��� �Է¹��� ���ؽ� ����

uniform sampler2D uTexture;

// ��� ������ �����ϱ� ���� out ���� �����ڸ� ����ؼ� ���� ������ ����
out vec4 outColor;

void main()
{
	vec4 textureColor = texture(uTexture, fragTexCoord);

	// �ؽ�ó�κ��� ���� ����� ���ؽ� ������ ����� outColor�� ����
	outColor.xyz = (vertexColor.xyz + textureColor.xyz) / 2;
	outColor.w = textureColor.w;	
}
