#ifndef __HI_IR_REMOTE_H__
#define __HI_IR_REMOTE_H__

#include "hi_unf_ecs.h"
#include "hi_type.h"


/** virtual key */
typedef enum hiADP_KEYCODE_E
{
    HI_ADP_KEY_1 = 2,
    HI_ADP_KEY_2,
    HI_ADP_KEY_3,
    HI_ADP_KEY_4,
    HI_ADP_KEY_5,
    HI_ADP_KEY_6,
    HI_ADP_KEY_7,
    HI_ADP_KEY_8,
    HI_ADP_KEY_9,
    HI_ADP_KEY_0,
    HI_ADP_KEY_DEL  = 14,
    HI_ADP_KEY_IME  = 38,
    HI_ADP_KEY_FAV  = 189,
    HI_ADP_KEY_MORE = 190,
    HI_ADP_KEY_PREV = 191,
    HI_ADP_KEY_REC   = 192,
    HI_ADP_KEY_NEXT  = 193,
    HI_ADP_KEY_SLOW  = 194,
    HI_ADP_KEY_RADIO = 195,
    HI_ADP_KEY_LIVE  = 196,

    HI_ADP_KEY_SEARCH = 180,

    HI_ADP_KEY_UP    = 103,
    HI_ADP_KEY_DOWN  = 108,
    HI_ADP_KEY_LEFT  = 105,
    HI_ADP_KEY_RIGHT = 106,
    HI_ADP_KEY_ENTER = 28,
    HI_ADP_KEY_EXIT  = 29,
    HI_ADP_KEY_MENU  = 30,

    HI_ADP_KEY_PLAY = 146,
    HI_ADP_KEY_STOP = 147,
    HI_ADP_KEY_REW = 150,
    HI_ADP_KEY_FF = 151,

    HI_ADP_KEY_EPG    = 134,
    HI_ADP_KEY_PGUP   = 104,
    HI_ADP_KEY_PGDOWN = 109,
    HI_ADP_KEY_BACK   = 135,

    HI_ADP_KEY_NVOD = 183,
    HI_ADP_KEY_BTV,
    HI_ADP_KEY_VOD,
    HI_ADP_KEY_NPVR,
    HI_ADP_KEY_HELP,
    HI_ADP_KEY_STOCK,

    HI_ADP_KEY_GUIDE = 197,
    HI_ADP_KEY_MAIL,
    HI_ADP_KEY_NEWS,
    HI_ADP_KEY_IPANEL,

    HI_ADP_KEY_P1 = 210,
    HI_ADP_KEY_P2,

    HI_ADP_KEY_F1 = 59,
    HI_ADP_KEY_F2 = 15,
    HI_ADP_KEY_F3 = 61,
    HI_ADP_KEY_F4 = 62,

    HI_ADP_KEY_SET     = 143,
    HI_ADP_KEY_SUB     = 145,
    HI_ADP_KEY_SEEK    = 152,
    HI_ADP_KEY_INFO    = 156,
    HI_ADP_KEY_POWER   = 129,
    HI_ADP_KEY_AUDIO   = 155,
    HI_ADP_KEY_MUTE    = 130,
    HI_ADP_KEY_CHNDOWN = 138,
    HI_ADP_KEY_CHNUP   = 137,
    HI_ADP_KEY_VOLDOWN = 148,
    HI_ADP_KEY_VOLUP   = 149,

    HI_ADP_KEY_BUTT
} HI_ADP_KEYCODE_E;

#define HI_USE_KEYS_FOR_STB_THEME    /**
                                      Enable keys for HiSilicon STB theme.
                                      Check theme source to confirm which
                                      keycode should be used
                                   **/


 
HI_S32 HI_RemoteDev_Initialize(HI_VOID);
HI_S32 HI_RemoteDev_Shutdown(HI_VOID);
HI_S32 HI_RemoteDev_GetInput(HI_VOID);



#endif /* __HI_IR_REMOTE_H__*/
