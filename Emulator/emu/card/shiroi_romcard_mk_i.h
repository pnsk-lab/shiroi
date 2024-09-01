/* $Id$ */

#ifndef __SHIROI_ROMCARD_MK_I_H__
#define __SHIROI_ROMCARD_MK_I_H__

#include "shiroi.h"

void shiroi_romcard_mk_i(shiroi_t* shiroi);
void shiroi_romcard_mk_i_tick(shiroi_t* shiroi);
void shiroi_romcard_mk_i_install(shiroi_t* shiroi, int slot);
void shiroi_romcard_mk_i_reset(shiroi_t* shiroi, int slot);

#endif
