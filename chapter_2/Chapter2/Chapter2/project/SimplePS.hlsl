struct PSInput
{
	//vertex shader�Ɠ������͂��w�肷��
	float4 Position : SV_POSITION;		//�ʒu���W . �V�X�e���l�Z�}���e�B�N�X�Ƃ��Đݒ肷��K�v������̂ŁA�d�l�̒ʂ��float4�^�ɂ��A�Z�}���e�B�N�X����SV_POSITION�Ƃ��Ă���
	float4 Color : COLOR;		//���_�J���[�B�V�X�e���Œ�`����Ă�����̂ɐݒ肷��K�v�͂Ȃ��̂�COLOR�Ƃ������O�ɂ��Ă���
};

//�s�N�Z���V�F�[�_�[�̏o�͐�͏o�͐�
struct PSOutput
{
	//RGBA��4�v�f�ō\�������s�N�Z��(�ł�)�l�������_�[�^�[�Q�b�g�̃t�H�[�}�b�g�Ƃ��Ďw�肵�Ă���̂ŁA�o�͂͂���ɍ��킹��
	//�����_�[�^�[�Q�b�g��1�������g�p���Ȃ��̂�SV_TARGET0���w�肷��
	float4 Color : SV_TARGET0;
	//SV_TARGET: �V�X�e���l�Z�}���e�B�N�X. �����_�[�^�[�Q�b�g�z���0�Ԗڂɏ������݂�����̂�SV_TARGET0���w��
	//1�Ԗڂ̂�񂾁[�^�[�Q�b�g�ɏ������݂�����ꍇ��SV_TARGET1
};

PSOutput main(PSInput input) : SV_TARGET
{
	//������
	PSOutput output = (PSOutput)input;
	output.Color = input.Color;
	return output;
}