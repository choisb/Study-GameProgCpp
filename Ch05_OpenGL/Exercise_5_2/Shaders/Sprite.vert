#version 330

uniform mat4 uWorldTransform;	
uniform mat4 uViewProj;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
// 버텍스 색상을 2번 속성으로 받는다.
layout(location = 2) in vec3 inVertexColor;

out vec2 fragTexCoord;
out vec3 vertexColor;

void main()
{
	vec4 pos = vec4(inPosition, 1.0);
	gl_Position = pos * uWorldTransform * uViewProj;
	fragTexCoord = inTexCoord;
	// 입력받은 버텍스 색상을 그대로 프래그먼트 셰이더로 넘겨준다.
	vertexColor = inVertexColor;  
}
