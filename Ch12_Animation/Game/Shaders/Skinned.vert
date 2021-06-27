#version 330

// uniform은 in, out 변수와 다르게 셰이더 프로그램의 
// 수많은 호출 사이에서도 동일하게 유지되는 전역 변수이다.
// mat4 는 3차원 동차좌표계 변환 행렬인 4x4 행렬이다.
uniform mat4 uWorldTransform;	
uniform mat4 uViewProj;
// matrixPalette를 위한 uniform
uniform mat4 uMatrixPalette[96];

// layout 명령어는 속성 슬롯이 어떤 변수에 해당하는지 지정할 수 있다.
// 여기서 속성 슬롯이란 glVertexAttribPointer 함수를 호출했을 때의 슬롯 번호이다.
// 각 속성의 크기는 C++ 코드에서 지정한다.
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in uvec4 inSkinBones;	// C++ 코드에서 1바이트 uint 4개로 선언 하여 최적화
layout(location = 3) in vec4 inSkinWeights;	// C++ 코드에서 1바이트 부동소수 4개로 선언 하여 최적화
layout(location = 4) in vec2 inTexCoord;

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

	// 위치를 스키닝
	vec4 skinnedPos = (pos * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;
	// 위치를 세계 공간으로 변환
	skinnedPos = skinnedPos * uWorldTransform;
	// 세계 위치를 저장
	fragWorldPos = skinnedPos.xyz;
	// 클립 공간으로 변환
	gl_Position = skinnedPos * uViewProj;

	// 버텍스의 법선 벡터 스키닝
	vec4 skinnedNormal = vec4(inNormal, 0.0f);
	skinnedNormal = 
	  (skinnedNormal * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x
	+ (skinnedNormal * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y
	+ (skinnedNormal * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z
	+ (skinnedNormal * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;

	// 법선을 세계 공간으로 변환 (w = 0)
	fragNormal = (skinnedNormal * uWorldTransform).xyz;
	// 텍스처 좌표를 프래그먼트 셰이더로 전달
	fragTexCoord = inTexCoord;
}
