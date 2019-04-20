#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "h264encoder.h"


int WIDTH = 640;
int HEIGHT = 480;


void compress_begin(Encoder *en, int width, int height) {
	en->param = (x264_param_t *) malloc(sizeof(x264_param_t));
	en->picture = (x264_picture_t *) malloc(sizeof(x264_picture_t));
	x264_param_default(en->param); //set default param
	//en->param->rc.i_rc_method = X264_RC_CQP;//设置为恒定码率
	// en->param->i_log_level = X264_LOG_NONE;

	en->param->i_threads  = X264_SYNC_LOOKAHEAD_AUTO;//取空缓存区使用不死锁保证
	en->param->i_width = width; //set frame width
	en->param->i_height = height; //set frame height
	WIDTH = width;
	HEIGHT = height;


	//en->param->i_frame_total = 0;

	//  en->param->i_keyint_max = 10;
	en->param->rc.i_lookahead = 0; //表示i帧向前缓冲区
	//   en->param->i_bframe = 5; //两个参考帧之间b帧的数目

	//  en->param->b_open_gop = 0;
	//  en->param->i_bframe_pyramid = 0;
	//   en->param->i_bframe_adaptive = X264_B_ADAPT_TRELLIS;

	//en->param->rc.i_bitrate = 1024 * 10;//rate 为10 kbps
	en->param->i_fps_num = 24; //帧率分子
	en->param->i_fps_den = 1; //帧率分母
	
	x264_param_apply_profile(en->param, x264_profile_names[0]); //使用baseline

	if ((en->handle = x264_encoder_open(en->param)) == 0) {
		return;
	}
	/* Create a new pic */
	x264_picture_alloc(en->picture, X264_CSP_I422, en->param->i_width,
			en->param->i_height);
	en->picture->img.i_csp = X264_CSP_I420;
	en->picture->img.i_plane = 3;
}

static long int pts = 0;
int compress_frame(Encoder *en, int type, uint8_t *in, uint8_t *out) {
	x264_picture_t pic_out;
	int index_y, index_u, index_v;
	int num;
	int nNal = -1;
	int result = 0;
	int i = 0;
	uint8_t *p_out = out;
	unsigned char *y = en->picture->img.plane[0];   
	unsigned char *u = en->picture->img.plane[1];   
	unsigned char *v = en->picture->img.plane[2];   
	char * ptr;

	index_y = 0;
    index_u = 0;
    index_v = 0;

    num = WIDTH * HEIGHT * 2 - 4;

    for(i=0; i<num; i=i+4){
        *(y + (index_y++)) = *(in + i);
        *(u + (index_u++)) = *(in + i + 1);
        *(y + (index_y++)) = *(in + i + 2);
        *(v + (index_v++)) = *(in + i + 3);
    }

	switch (type) {
	case 0:
		en->picture->i_type = X264_TYPE_P;
		break;
	case 1:
		en->picture->i_type = X264_TYPE_IDR;
		break;
	case 2:
		en->picture->i_type = X264_TYPE_I;
		break;
	default:
		en->picture->i_type = X264_TYPE_AUTO;
		break;
	}
	en->picture->i_pts = pts++;
	//printf("lex---pts:%d\n",pts);
	if ( x264_encoder_encode(en->handle, &(en->nal), &nNal, en->picture,&pic_out) < 0) {
		return -1;
	}

	for (i = 0; i < nNal; i++) {
		memcpy(p_out, en->nal[i].p_payload, en->nal[i].i_payload);
		p_out += en->nal[i].i_payload;
		result += en->nal[i].i_payload;
	}

	return result;
}

void compress_end(Encoder *en) {
	if (en->handle) {
		x264_encoder_close(en->handle);
	}
	if (en->picture) {
		x264_picture_clean(en->picture);
		free(en->picture);
		en->picture = 0;
	}
	if (en->param) {
		free(en->param);
		en->param = 0;
	}
	
	
}
