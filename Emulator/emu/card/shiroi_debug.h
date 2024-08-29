/* $Id$ */

#ifndef __SHIROI_DEBUG_H__
#define __SHIROI_DEBUG_H__

#include "shiroi.h"

void shiroi_debug(shiroi_t* shiroi);
void shiroi_debug_tick(shiroi_t* shiroi);
void shiroi_debug_install(shiroi_t* shiroi, int slot);
void shiroi_debug_reset(shiroi_t* shiroi, int slot);

#endif
