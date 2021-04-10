// 모든 GLSL 셰이더 파일은 먼저 사용하려는 GLSL 프로그래밍 언어의 버전을 지정해야 한다.
// version 330은 우리가 사용할 OpenGL 3.3 버전을 나타낸다.
#version 330

// 이 프로그램은 버텍스 셰이더 이므로 각 버텍스에 대한 버텍스 속성을 지정해야 한다.
// 이 속성은 버텍스 배열 개체의 속성과 일치해야 하며,
// 버텍스 배열 개체는 버텍스 셰이더의입력이 된다.
// 하지만 GLSL의 메인 함수는 어떤 파라미터도 받지 않는다.
// 대신 아래와 같이 전역 변수 비슷한 형태로 입력을 받을 수 있다. (전역 변수는 특별한 키워드로 표시한다.)
// 아래의 문장은 외부로 부터 vec3 타입을 입력받아 inPosition 변수에 저장한다는 뜻이다.
in vec3 inPosition;	// vec3은 3개의 부동 소수점 값의 벡터에 해당한다.

// 셰이더의 진입점도 C/C++ 처럼 main 함수이다.
// main 함수가 void를 리턴하므로 셰이더의 결과물을 저장하려면 GLSL이 제공하는 전역 변수를 사용해야 한다.

void main()
{
	//gl_Position은 GLSL에서 출력을 위해 제공해주는 전역 변수이다.
	// gl_Position은 vec4 타입으로 vec3 타입인 inPosition에 부동 소수 값 하나를 추가로 더 받는다.
	gl_Position = vec4(inPosition, 1.0); // vec4는 (x,y,z,w)로 구성되어 있다.
}

// 이 프로그램을 아래와 같이 입력받은 값을 그대로 다시 출력하는 프로그램이다. 순서는 다음과 같다.
// 1. C++로 부터 inPosition 변수에 정점 하나를 입력 받음
// 2. gl_Position 변수에 입력받은 inPosition 변수를 그대로 복사함
// 3. gl_Position을 GLSL이 알아서 출력함.