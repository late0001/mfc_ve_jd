#ifndef __FF_COMMON_H__
#define __FF_COMMON_H__

#define PRId64 "I64d"

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/mem.h>
#include <libavutil/fifo.h>
#include "libavutil/mathematics.h"
#include <libavutil/time.h>
#include <libavutil/timestamp.h>
#include <libswscale/swscale.h>
#include <sdl/sdl.h>
#ifdef __cplusplus
}
#endif

#endif 

