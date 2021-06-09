#version 330

// 원칙적으로 버텍스 셰이더의 모든 out 변수들은 
// 프래그먼트 셰이더에서 이에 해당하는 in 변수를 갖고 있어야 한다.
// 프래그먼트 셰이더에서 in 변수의 이름과 타입은 버텍스 셰이더에서의 
// out 변수와 일치하는 동일한 이름과 타입을 갖고 있어야 한다.
in vec2 fragTexCoord;

// 제공된 텍스처 좌표로 색상을 얻기 위해 텍스처 샘플러 uniform
// sampler2D 타입은 2D 텍스처를 샘플링할 수 있는 특별한 타입이다.
// sampler uniform은 C++ 코드에서 바인딩이 필요없다.
// 왜냐하면 지금 구조에서는 한 번에 오직 하나의 텍스처만 바인딩하기 때문이다.
// 그래서 OpenGL은 자동을 셰이더의 텍스처 샘플러가 활성화된 텍스처에 유일하게 대응함을 안다.
uniform sampler2D uTexture;

// 출력 색상을 저장하기 위해 out 변수 지정자를 사용해서 전역 변수를 선언
out vec4 outColor;

void main()
{
	// 텍스처로부터 색상을 샘플링
	outColor = texture(uTexture, fragTexCoord);
}
