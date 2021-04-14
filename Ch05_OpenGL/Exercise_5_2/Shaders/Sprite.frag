#version 330

in vec2 fragTexCoord;
in vec3 vertexColor;	// 버텍스 셰이더로부터 입력받을 버텍스 색상

uniform sampler2D uTexture;

// 출력 색상을 저장하기 위해 out 변수 지정자를 사용해서 전역 변수를 선언
out vec4 outColor;

void main()
{
	vec4 textureColor = texture(uTexture, fragTexCoord);

	// 텍스처로부터 얻은 색상과 버텍스 색상의 평균을 outColor로 지정
	outColor.xyz = (vertexColor.xyz + textureColor.xyz) / 2;
	outColor.w = textureColor.w;	
}
