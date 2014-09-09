#include "hi_ir_remote.h"

typedef struct
{
    unsigned int CommonValue;
    unsigned int IrkeyValue;
} HI_adp_KEY_MAP_S;


static HI_adp_KEY_MAP_S s_PanIRMap[] =
{
    /*统一定义键值*/           /*遥控器实际键值*/
    {HI_ADP_KEY_POWER,       0x639cff00, /*电源*/      },
    {HI_ADP_KEY_AUDIO,       0x2ed1ff00, /*声道*/      },
    {HI_ADP_KEY_MUTE,       0x22ddff00, /*静音*/      },
    {HI_ADP_KEY_CHNDOWN,   0x7a85ff00, /*频道减*/    },
    {HI_ADP_KEY_CHNUP,       0x7986ff00, /*频道加*/    },
    {HI_ADP_KEY_VOLDOWN,   0x7e81ff00, /*频道减*/    },
    {HI_ADP_KEY_VOLUP,       0x7f80ff00, /*频道加*/    },
    {HI_ADP_KEY_1,           0x6d92ff00, /*1*/         },
    {HI_ADP_KEY_2,           0x6c93ff00, /*2*/         },
    {HI_ADP_KEY_3,           0x33ccff00, /*3*/         },
    {HI_ADP_KEY_4,           0x718eff00, /*4*/         },
    {HI_ADP_KEY_5,           0x708fff00, /*5*/         },
    {HI_ADP_KEY_6,           0x37c8ff00, /*6*/         },
    {HI_ADP_KEY_7,           0x758aff00, /*7*/         },
    {HI_ADP_KEY_8,           0x748bff00, /*8*/         },
    {HI_ADP_KEY_9,           0x3bc4ff00, /*9*/         },
    {HI_ADP_KEY_0,         0x7887ff00, /*0*/         },
    {HI_ADP_KEY_IME,       0x609fff00, /*输入法*/    },
    {HI_ADP_KEY_FAV,       0xfa057748, /*FAV*/       },
    {HI_ADP_KEY_EXIT,      0xe21d7748 },

    {HI_ADP_KEY_SEARCH,      0x6897ff00, /*Search*/    },
    {HI_ADP_KEY_UP,          0x35caff00, /*上*/        },
    {HI_ADP_KEY_LEFT,        0x6699ff00, /*左*/        },
    {HI_ADP_KEY_RIGHT,       0x3ec1ff00, /*右*/        },
    {HI_ADP_KEY_DOWN,        0x2dd2ff00, /*下*/        },
    {HI_ADP_KEY_ENTER,       0x31ceff00, /*OK*/        },
    {HI_ADP_KEY_MENU,        0x629dff00, /*Menu*/      },
    {HI_ADP_KEY_BACK,        0x6f90ff00, /*Back*/      },
    {HI_ADP_KEY_PGUP,        0x30cfff00, /*Page Up*/   },
    {HI_ADP_KEY_PGDOWN,      0x6798ff00, /*Page Down*/ },
    
    {HI_ADP_KEY_PREV,        0x9b, /*PREV*/   },
    {HI_ADP_KEY_PLAY,        0x3cc3ff00, /*Play/Pause*/},
    {HI_ADP_KEY_REC,         0xc5, /*REC*/ },
    {HI_ADP_KEY_NEXT,        0xcd, /*Next*/ },

    {HI_ADP_KEY_REW,         0x25daff00, /*Rew*/ },
    {HI_ADP_KEY_SLOW,        0xda, /*Slow*/ },
    {HI_ADP_KEY_STOP,        0x2fd0ff00, /*Stop*/ },
    {HI_ADP_KEY_FF,          0x29d6ff00, /*FFW*/ },

    //{HI_ADP_KEY_RADIO,       0xe41b7749, /*Radio*/ },
    {HI_ADP_KEY_LIVE,        0xce317748, /*Live*/ },
    {HI_ADP_KEY_INFO,        0x38c7ff00 /*Info*/},
    {HI_ADP_KEY_SUB,         0x738cff00, /*Sub*/ },
    {HI_ADP_KEY_NVOD,        0x3fc0ff00,/*轮播*/ },
  

    {HI_ADP_KEY_F1,          0x7b84ff00, /*F1*/     },
    {HI_ADP_KEY_F2,          0x7689ff00, /*F2*/     },
    {HI_ADP_KEY_F3,          0x26d9ff00, /*F3*/     },
    {HI_ADP_KEY_F4,          0x6996ff00, /*F4*/     },
    
    {HI_ADP_KEY_GUIDE,         0xf40b7748, /*指南*/ },
    {HI_ADP_KEY_VOD,           0xe41b7748, /*点播*/     },
    {HI_ADP_KEY_MAIL,          0xcc337748, /*邮件*/     },
    {HI_ADP_KEY_STOCK,         188, /*股票*/ },
    {HI_ADP_KEY_EPG,           0xf40b7748, /* EPG/列表*/ },
    {HI_ADP_KEY_SET,           0x6a95ff00, /* SET*/ },

  };

HI_S32 HI_RemoteDev_Initialize(HI_VOID)
{
    HI_S32 s32RetVal = HI_FAILURE;

    s32RetVal = HI_UNF_IR_Open();
    if (HI_SUCCESS != s32RetVal)
    {
        printf("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, s32RetVal);
        return s32RetVal;
    }

    /** enable key up **/
    s32RetVal = HI_UNF_IR_EnableKeyUp(HI_TRUE);
    if (HI_SUCCESS != s32RetVal)
    {
        printf("EnableKeyUp failed!\n");
        HI_UNF_IR_Close();
        return s32RetVal;
    }

    /** set repeat timeout **/
    s32RetVal = HI_UNF_IR_SetRepKeyTimeoutAttr(108);
    if (HI_SUCCESS != s32RetVal)
    {
        printf("SetRepKeyTimeoutAttr failed!\n");
        HI_UNF_IR_Close();
        return s32RetVal;
    }

    s32RetVal = HI_UNF_IR_EnableRepKey(HI_TRUE);
    if (HI_SUCCESS != s32RetVal)
    {
        printf("enable RepKey failed!\n");
        HI_UNF_IR_Close();
        return s32RetVal;
    }

    /** enable the remote device **/
    s32RetVal = HI_UNF_IR_Enable(HI_TRUE);
    if (HI_SUCCESS != s32RetVal)
    {
        printf("enable ir failed!\n");
        HI_UNF_IR_Close();
        return s32RetVal;
    }

    return HI_SUCCESS;
}


HI_S32 HI_RemoteDev_Shutdown(HI_VOID)
{
    HI_S32 s32RetVal = HI_FAILURE;

    s32RetVal = HI_UNF_IR_Close();
    if (HI_SUCCESS != s32RetVal)
    {
        printf("device file closed fail!\n");
        return s32RetVal;
    }

    return HI_SUCCESS;
}

HI_S32 HI_RemoteDev_GetInput(HI_VOID)
{
    HI_U64 u64KeyId;
    HI_UNF_KEY_STATUS_E PressStatus;
    HI_U32 s_IMKeyCount = sizeof(s_PanIRMap) / sizeof(HI_adp_KEY_MAP_S);
    HI_U8 i;

    /** 只接受按下键 **/
    if (HI_SUCCESS != HI_UNF_IR_GetValue(&PressStatus, &u64KeyId, 0))
    {
        return HI_FAILURE;
    }

    if (HI_UNF_KEY_STATUS_DOWN != PressStatus)
    {
        return HI_FAILURE;
    }

    for (i = 0; i < s_IMKeyCount; i++)
    {
        if (s_PanIRMap[i].IrkeyValue == u64KeyId)
        {
            return (int)(s_PanIRMap[i].CommonValue);
        }
    }

    return HI_FAILURE;
}
