#version 330

// 방향광을 위한 구조체 정의
struct DirectionalLight
{
	// 빛의 방향
	vec3 mDirection;
	// 난반사 색상
	vec3 mDiffuseColor;
	// 정반사 색상
	vec3 mSpecColor;
};

//********* C++로 부터 입력 받는 uniform 부분**************
// 조명을 위한 uniform
// 세계 공간에서의 카메라 위치
uniform vec3 uCameraPos;
// 주변광
uniform vec3 uAmbientLight;
// 표면에 대한 정반사 지수
uniform float uSpecPower;
// 방향광(지금은 오직 하나)
uniform DirectionalLight uDirLight;
// 제공된 텍스처 좌표로 색상을 얻기 위해 텍스처 샘플러 uniform
// sampler2D 타입은 2D 텍스처를 샘플링할 수 있는 특별한 타입이다.
// sampler uniform은 C++ 코드에서 바인딩이 필요없다.
// 왜냐하면 지금 구조에서는 한 번에 오직 하나의 텍스처만 바인딩하기 때문이다.
// 그래서 OpenGL은 자동을 셰이더의 텍스처 샘플러가 활성화된 텍스처에 유일하게 대응함을 안다.
uniform sampler2D uTexture;


//****** 버텍스 셰이더로 부터 입력 받는 부분********
// 원칙적으로 버텍스 셰이더의 모든 out 변수들은 
// 프래그먼트 셰이더에서 이에 해당하는 in 변수를 갖고 있어야 한다.
// 프래그먼트 셰이더에서 in 변수의 이름과 타입은 버텍스 셰이더에서의 
// out 변수와 일치하는 동일한 이름과 타입을 갖고 있어야 한다.

// 텍스처와 매핑되는 텍스처 좌표
in vec2 fragTexCoord;
// 세계 공간에서의 법선
in vec3 fragNormal;
// 세계 공간에서의 위치
in vec3 fragWorldPos;


// ******* 프래그먼트 셰이더 출력 부분****************
// 출력 색상을 저장하기 위해 out 변수 지정자를 사용해서 전역 변수를 선언
out vec4 outColor;

void main()
{
	// 표면 법선. 각 버텍스에서는 정규화된 법선벡터를 가지고 있더라도
	// 삼각형 표면의 법선벡터는 버텍스의 법선 벡터를 보간하여서 사용한다.
	// 이때 보간된 값은 정규화된 벡터를 보장하지 않기 때문에 사용하기 전에 정규화 단계를 거쳐야 한다. 
	vec3 N = normalize(fragNormal);

	// 표면에서 광원의로의 벡터
	// 방향광은 한 방향으로 발산하는 특징이 있다.
	// 때문에 표면에서 광원으로 향하는 벡터는 광원의 방향 벡터를 반전시키면 된다.
	vec3 L = normalize(-uDirLight.mDirection);

	// 표면에서 카메라로 향하는 벡터
	vec3 V = normalize(uCameraPos - fragWorldPos);

	// N에 대한 -L의 반사
	// GLSL에서는 반사벡터를 구할 수 있는 함수 reflect()를 제공해준다.
	vec3 R = normalize(reflect(-L, N));

	// 퐁 반사 계산
	vec3 Phong = uAmbientLight;
	float NdotL = dot(N, L);
	if(NdotL > 0)
	{
		vec3 Diffuse = uDirLight.mDiffuseColor * NdotL;
		vec3 Specular = uDirLight.mSpecColor * 
						pow(max(0.0, dot(R, V)), uSpecPower);
		Phong += Diffuse + Specular;
	}

	// 최종 색은 텍서처 색상 곱하기 퐁 광원 (알파값 = 1)
	outColor = texture(uTexture, fragTexCoord) * vec4(Phong,1.0f);
}
