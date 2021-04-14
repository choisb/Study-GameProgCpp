#version 330

uniform mat4 uWorldTransform;	
uniform mat4 uViewProj;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
// ���ؽ� ������ 2�� �Ӽ����� �޴´�.
layout(location = 2) in vec3 inVertexColor;

out vec2 fragTexCoord;
out vec3 vertexColor;

void main()
{
	vec4 pos = vec4(inPosition, 1.0);
	gl_Position = pos * uWorldTransform * uViewProj;
	fragTexCoord = inTexCoord;
	// �Է¹��� ���ؽ� ������ �״�� �����׸�Ʈ ���̴��� �Ѱ��ش�.
	vertexColor = inVertexColor;  
}
