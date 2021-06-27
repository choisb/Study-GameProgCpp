#version 330

// uniform�� in, out ������ �ٸ��� ���̴� ���α׷��� 
// ������ ȣ�� ���̿����� �����ϰ� �����Ǵ� ���� �����̴�.
// mat4 �� 3���� ������ǥ�� ��ȯ ����� 4x4 ����̴�.
uniform mat4 uWorldTransform;	
uniform mat4 uViewProj;
// matrixPalette�� ���� uniform
uniform mat4 uMatrixPalette[96];

// layout ��ɾ�� �Ӽ� ������ � ������ �ش��ϴ��� ������ �� �ִ�.
// ���⼭ �Ӽ� �����̶� glVertexAttribPointer �Լ��� ȣ������ ���� ���� ��ȣ�̴�.
// �� �Ӽ��� ũ��� C++ �ڵ忡�� �����Ѵ�.
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in uvec4 inSkinBones;	// C++ �ڵ忡�� 1����Ʈ uint 4���� ���� �Ͽ� ����ȭ
layout(location = 3) in vec4 inSkinWeights;	// C++ �ڵ忡�� 1����Ʈ �ε��Ҽ� 4���� ���� �Ͽ� ����ȭ
layout(location = 4) in vec2 inTexCoord;

// ���ؽ� ���̴��� �Էµ� �ؽ��� ��ǥ�� �����׸�Ʈ ���̴��� �����ϱ� ���� ����

// out ������ �����ϸ� ���ؽ� ���̴����� �����׸�Ʈ ���̴��� ������ ������ �����ϴ�.
out vec2 fragTexCoord;
// ���� ���������� ����
out vec3 fragNormal;
// ���� ���������� ��ġ
out vec3 fragWorldPos;

void main()
{
	// ��ġ�� ���� ��ǥ�� ��ȯ
	vec4 pos = vec4(inPosition, 1.0);

	// ��ġ�� ��Ű��
	vec4 skinnedPos = (pos * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z;
	skinnedPos += (pos * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;
	// ��ġ�� ���� �������� ��ȯ
	skinnedPos = skinnedPos * uWorldTransform;
	// ���� ��ġ�� ����
	fragWorldPos = skinnedPos.xyz;
	// Ŭ�� �������� ��ȯ
	gl_Position = skinnedPos * uViewProj;

	// ���ؽ��� ���� ���� ��Ű��
	vec4 skinnedNormal = vec4(inNormal, 0.0f);
	skinnedNormal = 
	  (skinnedNormal * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x
	+ (skinnedNormal * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y
	+ (skinnedNormal * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z
	+ (skinnedNormal * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;

	// ������ ���� �������� ��ȯ (w = 0)
	fragNormal = (skinnedNormal * uWorldTransform).xyz;
	// �ؽ�ó ��ǥ�� �����׸�Ʈ ���̴��� ����
	fragTexCoord = inTexCoord;
}
