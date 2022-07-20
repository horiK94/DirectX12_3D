//���͂Ƃ��đ�����f�[�^
struct VSInput
{
	//COLOR, POSITION�̓Z�}���e�B�N�X��
	//CPU���瑗��ꂽ�f�[�^���ǂ����߂��邩���`����. �u���́v�Z�}���e�B�N�X�͎��R�ɖ��O��t������
	float3 position : POSITION;		//POSITION: ���W�f�[�^�ł��邱�Ƃ��`
	float4 Color : COLOR;		//COLOR: ���_�J���[�f�[�^�ł��邱�Ƃ��`
};

//�o�̓f�[�^�ɒ��_�J���[�����̂܂ܓn�����Ƃ��l�����Ƃ��A�ǂ̈ʒu�ɕ\�����邩�̍��W�f�[�^�ƃJ���[��񂪕K�v
struct VSOutput
{
	//�o�͂̃Z�}���e�B�N�X�́u�V�X�e���l�Z�}���e�B�N�X�v���g�p������̂�����B
	//�V�X�e���l�Z�}���e�B�N�X��SV_�Ŏn�܂�A�ǂ̃V�F�[�_�[�X�e�[�W�Ŏg�p���邩���d�l�Ō��܂��Ă���
	//https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics#semantics-supported-only-for-direct3d-10-and-newer
	float4 position : SV_POSITION;		//�ʒu���W . �V�X�e���l�Z�}���e�B�N�X�Ƃ��Đݒ肷��K�v������̂ŁA�d�l�̒ʂ��float4�^�ɂ��A�Z�}���e�B�N�X����SV_POSITION�Ƃ��Ă���
	float4 color : COLOR;		//���_�J���[�B�V�X�e���Œ�`����Ă�����̂ɐݒ肷��K�v�͂Ȃ��̂�COLOR�Ƃ������O�ɂ��Ă���
};

//���_�f�[�^�����H����̂ɕK�v�Ȓ萔�o�b�t�@��ǉ�
//cbuffer: �萔�o�b�t�@��\��
//Transform: �萔�o�b�t�@��
//register(b0): �萔�o�b�t�@�p���W�X�^��0�Ԗڂ��g�p
cbuffer Transform : register(b0)
{
	//packoffset()�͒萔�o�b�t�@�̐擪����̃I�t�Z�b�g�w��
	//c0���擪�f�[�^, c1�͐擪����float��4����16byte���\��
	//float1����\�������ꍇ�͎��̂悤�ɕ\��
	//float hoge0: packoffset(c0.x):
	//float hoge1: packoffset(c0.y):
	//float hoge2: packoffset(c0.z):
	//float hoge3: packoffset(c0.w):

	float4x4 World: packoffset(c0);		//���[���h�s��
	float4x4 View: packoffset(c4);		//�r���[�s��
	float4x4 Proj: packoffset(c8);		//�ˉe�s��
}

VSOutput main(VSInput input)
{
	VSOutput output = (VSOutput)0;

	float4 localPos = float4(input.position, 1.0f);
	float4 worldPos = mul(World, localPos);
	float4 viewPos = mul(View, worldPos);
	float4 projPos = mul(Proj, viewPos);

	output.position = projPos;
	output.color = input.Color;
	return output;
}