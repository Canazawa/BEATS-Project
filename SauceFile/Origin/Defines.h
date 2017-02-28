#pragma once

// �}�N����`
#define SAFE_DELETE(x) if(x){delete x; x=0;}
#define SAFE_RELEASE(x) if(x){x->Release(); x=NULL;}
#define SAFE_DELETE_ARRAY(p){ if(p){ delete[] (p);(p)=NULL;}}

// �E�B���h�E�萔��`
#define WINDOW_WIDTH		1920				// �E�B���h�E��
#define WINDOW_HEIGHT		1080				// �E�B���h�E����
#define APP_NAME			L"BEATs"		// �E�B���h�E��

// �e�L�X�g��`
#define DIMENSION			48.0f
#define WDIMENSION			40.0f
#define TEX_DIMENSION		512.00f

#define BMS_RESOLUTION		9600			// 1���߂̃J�E���g�l
#define BMS_MAXBUFFER		(16 * 16)		// 00�`FF�܂ł̃o�b�t�@��

// BMS�`�����l����`
#define BMS_BACKMUSIC		0x01			// ���̈ʒu�ɂ�����A�����I�ɍĐ������WAV���w�肵�܂�
#define BMS_STRETCH			0x02			// ���̏��߂̒������`�����f�[�^�{���܂��i�P�O�i���A�����Ƃ��Ɏg�p�j
#define BMS_TEMPO			0x03			// �Đ��e���|�i�a�o�l / �P���Ԃ̎l���������j�̓r���ύX�i�P�U�i���j
#define BMS_BACKANIME		0x04			// �o�b�N�O���E���h�A�j���[�V�����@�\
#define BMS_EXTENEDOBJ		0x05			// �������Ă���I�u�W�F��ʂ̃L�����N�^�[�i���o�[�̂��̂ɂ���ւ���@�\
#define BMS_CHGPOORANI		0x06			// POOR���o�����Ƃ��ɕ\�������摜��ύX
#define BMS_LAYERANIME		0x07			// Ch.04�Ŏw�肵��BGA�̏�ɂ��Ԃ���BMP���w��ł��܂�
#define BMS_BPMINDEX		0x08			// BPM�̃C���f�b�N�X�w��(�V)

// ����L�[��`
#define KYE_DOWN			0x53			//�uS�v
#define KYE_UP				0x44			//�uD�v
#define KYE_CANCEL			0x4B			//�uK�v
#define KYE_DECIDE			0x4C			//�uL�v
