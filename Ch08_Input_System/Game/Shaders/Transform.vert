#version 330

// uniform은 in, out 변수와 다르게 셰이더 프로그램의 
// 수많은 호출 사이에서도 동일하게 유지되는 전역 변수이다.
// mat4 는 3차원 동차좌표계 변환 행렬인 4x4 행렬이다.
uniform mat4 uWorldTransform;	
uniform mat4 uViewProj;

in vec3 inPosition;

void main()
{
	vec4 pos = vec4(inPosition, 1.0);
	gl_Position = pos * uWorldTransform * uViewProj;
}
