// GLSL 3.3 요청
#version 330

// 출력 색상을 저장하기 위해 out 변수 지정자를 사용해서 전역 변수를 선언
out vec4 outColor;

void main()
{
	// 출력 색상을 파랑색으로 설정. 
	outColor = vec4(0.0, 0.0, 1.0, 1.0); //  (R, G, B, A)
}