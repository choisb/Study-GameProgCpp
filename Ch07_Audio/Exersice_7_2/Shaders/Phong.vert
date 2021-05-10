#version 330

// uniform은 in, out 변수와 다르게 셰이더 프로그램의 
// 수많은 호출 사이에서도 동일하게 유지되는 전역 변수이다.
// mat4 는 3차원 동차좌표계 변환 행렬인 4x4 행렬이다.
uniform mat4 uWorldTransform;	
uniform mat4 uViewProj;

// layout 명령어는 속성 슬롯이 어떤 변수에 해당하는지 지정할 수 있다.
// 여기서 속성 슬롯이란 glVertexAttribPointer 함수를 호출했을 때의 슬롯 번호이다.
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// 버텍스 셰이더로 입력된 텍스쳐 좌표를 프래그먼트 셰이더로 전달하기 위한 변수
// out 변수를 선언하면 버텍스 셰이더에서 프래그먼트 셰이더로 데이터 전달이 가능하다.
out vec2 fragTexCoord;
// 세계 공간에서의 법선
out vec3 fragNormal;
// 세계 공간에서의 위치
out vec3 fragWorldPos;

void main()
{
	// 위치를 동차 좌표로 변환
	vec4 pos = vec4(inPosition, 1.0);
	// 위치를 세계 공간으로 변환한 뒤 클립 공간으로 변환
	pos = pos * uWorldTransform;
	// 위치를 클립 공간 좌표로 변환
	gl_Position = pos * uViewProj;

	// 프레그먼트 셰이더로 데이터 전달 부분
	// 세계 공간상의 위치를 저장
	fragWorldPos = pos.xyz;
	// 법선벡터를 세계 공간상의 법선으로 변환
	// 법선벡터는 방향뿐이기에 이동변환이 의미 없다. 때문에 동차 좌표계에서 w=0 이다
	fragNormal = (vec4(inNormal, 0.0f) * uWorldTransform).xyz;

	// 텍스처 좌표를 프래그먼트 셰이더에 전달
	fragTexCoord = inTexCoord;
}
