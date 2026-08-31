/*********************************************************************************************
    *   Filename        : app_main.c

    *   Description     :

    *   Copyright:(c)JIELI  2011-2019  @ , All Rights Reserved.
*********************************************************************************************/
#include "system/includes.h"
#include "app_config.h"
#include "app_action.h"
#include "app_main.h"
#include "update.h"
#include "update_loader_download.h"
#include "app_charge.h"
#include "app_power_manage.h"
#include "asm/charge.h"
#include "asm/adc_api.h"
#include "asm/iic_soft.h"
#include "asm/spi.h"


#if TCFG_KWS_VOICE_RECOGNITION_ENABLE
#include "jl_kws/jl_kws_api.h"
#endif /* #if TCFG_KWS_VOICE_RECOGNITION_ENABLE */

#define LOG_TAG_CONST       APP
#define LOG_TAG             "[APP]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

/*任务列表   */
const struct task_info task_info_table[] = {
#if CONFIG_APP_FINDMY
    {"app_core",            1,     0,   640 * 2,   128  },
#else
    {"app_core",            1,     0,   640,   128  },
#endif

    {"sys_event",           7,     0,   256,   0    },
    {"btctrler",            4,     0,   512,   256  },
    {"btencry",             1,     0,   512,   128  },
    {"btstack",             3,     0,   768,   256   },
    {"systimer",		    7,	   0,   128,   0	},
    {"update",				1,	   0,   512,   0    },
    {"dw_update",		 	2,	   0,   256,   128  },
#if (RCSP_BTMATE_EN)
    {"rcsp_task",		    2,	   0,   640,	0},
#endif
#if(USER_UART_UPDATE_ENABLE)
    {"uart_update",	        1,	   0,   256,   128	},
#endif
#if (XM_MMA_EN)
    {"xm_mma",   		    2,	   0,   640,   256	},
#endif
    {"usb_msd",           	1,     0,   512,   128  },
#if TCFG_AUDIO_ENABLE
    {"audio_dec",           3,     0,   768,   128  },
    {"audio_enc",           4,     0,   512,   128  },
#endif/*TCFG_AUDIO_ENABLE*/
#if TCFG_KWS_VOICE_RECOGNITION_ENABLE
    {"kws",                 2,     0,   256,   64   },
#endif /* #if TCFG_KWS_VOICE_RECOGNITION_ENABLE */
#if (TUYA_DEMO_EN)
    {"user_deal",           2,     0,   512,   512  },//定义线程 tuya任务调度
#endif
#if (CONFIG_APP_HILINK)
    {"hilink_task",         2,     0,   1024,   0},//定义线程 hilink任务调度
#endif
    {0, 0},
};

APP_VAR app_var;

void app_var_init(void)
{
    app_var.play_poweron_tone = 1;

    app_var.auto_off_time =  TCFG_AUTO_SHUT_DOWN_TIME;
    app_var.warning_tone_v = 340;
    app_var.poweroff_tone_v = 330;
}

__attribute__((weak))
u8 get_charge_online_flag(void)
{
    return 0;
}

void clr_wdt(void);
void check_power_on_key(void)
{
#if TCFG_POWER_ON_NEED_KEY

    u32 delay_10ms_cnt = 0;
    while (1) {
        clr_wdt();
        os_time_dly(1);

        extern u8 get_power_on_status(void);
        if (get_power_on_status()) {
            log_info("+");
            delay_10ms_cnt++;
            if (delay_10ms_cnt > 70) {
                /* extern void set_key_poweron_flag(u8 flag); */
                /* set_key_poweron_flag(1); */
                return;
            }
        } else {
            log_info("-");
            delay_10ms_cnt = 0;
            log_info("enter softpoweroff\n");
            power_set_soft_poweroff();
        }
    }
#endif
}

static void wtyi_uart_heartbeat(void *priv)
{
    static u32 cnt;

    printf("[WTYI] UART heartbeat %lu, bt_name=WTYI_BT_TEST, baud=1000000\r\n", ++cnt);
}

#define WTYI_HW_TEST_ENABLE  1

#if WTYI_HW_TEST_ENABLE
static u8 wtyi_hw_test_inited;

static void wtyi_hw_test_adc(void)
{
    u32 vbat_mv = adc_get_voltage(AD_CH_VBAT) * 4;
    u32 vbg_mv = adc_get_voltage(AD_CH_LDOREF);
    u32 pb1_raw = adc_get_value(AD_CH_PB1);
    u32 pb1_mv = adc_get_voltage(AD_CH_PB1);

    printf("[WTYI_HW][ADC] VBAT=%lu mV, VBG=%lu mV, PB1_RAW=%lu, PB1=%lu mV\r\n",
           vbat_mv, vbg_mv, pb1_raw, pb1_mv);
}

static void wtyi_hw_test_iic_scan(void)
{
    u8 addr;
    u8 ack;
    u8 found = 0;

    soft_iic_init(0);
    printf("[WTYI_HW][IIC] scan PA07=SCL PA08=SDA start\r\n");

    for (addr = 0x08; addr <= 0x77; addr++) {
        soft_iic_start(0);
        ack = soft_iic_tx_byte(0, addr << 1);
        soft_iic_stop(0);

        if (ack) {
            found++;
            printf("[WTYI_HW][IIC] found addr=0x%x\r\n", addr);
        }
    }

    if (!found) {
        printf("[WTYI_HW][IIC] no ACK found; no device connected or wiring/pullup missing\r\n");
    }

    soft_iic_uninit(0);
}

static void wtyi_hw_test_spi_loopback(void)
{
    static const u8 tx_buf[] = {0x55, 0xaa, 0x5a, 0xa5};
    u8 rx;
    u8 i;
    int err;
    u8 pass = 1;

    spi_open(SPI1);
    printf("[WTYI_HW][SPI1] loopback test: PB1(DO)->PB2(DI), PB0=CLK\r\n");

    for (i = 0; i < sizeof(tx_buf); i++) {
        rx = spi_send_recv_byte(SPI1, tx_buf[i], &err);
        printf("[WTYI_HW][SPI1] tx=0x%x rx=0x%x err=%d\r\n", tx_buf[i], rx, err);
        if (err || rx != tx_buf[i]) {
            pass = 0;
        }
    }

    printf("[WTYI_HW][SPI1] loopback %s\r\n", pass ? "PASS" : "FAIL");
    spi_close(SPI1);
}

static void wtyi_hw_test_timer(void *priv)
{
    printf("[WTYI_HW] ===== hardware test tick =====\r\n");
    wtyi_hw_test_adc();
    wtyi_hw_test_iic_scan();
    wtyi_hw_test_spi_loopback();
}

static void wtyi_hw_test_init(void)
{
    if (wtyi_hw_test_inited) {
        return;
    }

    wtyi_hw_test_inited = 1;
    adc_add_sample_ch(AD_CH_PB1);

    printf("[WTYI_HW] hardware test enabled\r\n");
    printf("[WTYI_HW] UART: PA00 TX, 1000000 baud\r\n");
    printf("[WTYI_HW] IIC: PA07 SCL, PA08 SDA\r\n");
    printf("[WTYI_HW] SPI1: PB0 CLK, PB1 DO, PB2 DI\r\n");
    printf("[WTYI_HW] ADC: internal VBAT/VBG + PB1 external/ADKEY\r\n");

    sys_timer_add(NULL, wtyi_hw_test_timer, 5000);
}
#endif


void app_main()
{
    struct intent it;

    if (!UPDATE_SUPPORT_DEV_IS_NULL()) {
        int update = 0;
        update = update_result_deal();
    }

    printf(">>>>>>>>>>>>>>>>>app_main...\n");
    printf(">>> v220,2022-11-23 >>>\n");
    printf("[WTYI] AC63 SPP+LE firmware start\r\n");
    printf("[WTYI] BT name target: WTYI_BT_TEST\r\n");
    printf("[WTYI] UART log: TX=PA00, baud=1000000\r\n");
    sys_timer_add(NULL, wtyi_uart_heartbeat, 5000);
#if WTYI_HW_TEST_ENABLE
    wtyi_hw_test_init();
#endif

    if (get_charge_online_flag()) {
#if(TCFG_SYS_LVD_EN == 1)
        vbat_check_init();
#endif
    } else {
        check_power_on_voltage();
    }

#if TCFG_POWER_ON_NEED_KEY
    check_power_on_key();
#endif

#if TCFG_AUDIO_ENABLE
    extern int audio_dec_init();
    extern int audio_enc_init();
    audio_dec_init();
    audio_enc_init();
#endif/*TCFG_AUDIO_ENABLE*/

#if TCFG_KWS_VOICE_RECOGNITION_ENABLE
    jl_kws_main_user_demo();
#endif /* #if TCFG_KWS_VOICE_RECOGNITION_ENABLE */

    init_intent(&it);

#if CONFIG_APP_SPP_LE
    it.name = "spp_le";
    it.action = ACTION_SPPLE_MAIN;

#elif CONFIG_APP_AT_COM || CONFIG_APP_AT_CHAR_COM
    it.name = "at_com";
    it.action = ACTION_AT_COM;

#elif CONFIG_APP_DONGLE
    it.name = "dongle";
    it.action = ACTION_DONGLE_MAIN;

#elif CONFIG_APP_MULTI
    it.name = "multi_conn";
    it.action = ACTION_MULTI_MAIN;

#elif CONFIG_APP_NONCONN_24G
    it.name = "nonconn_24g";
    it.action = ACTION_NOCONN_24G_MAIN;

#elif CONFIG_APP_HILINK
    it.name = "hilink";
    it.action = ACTION_HILINK_MAIN;

#elif CONFIG_APP_LL_SYNC
    it.name = "ll_sync";
    it.action = ACTION_LL_SYNC;

#elif CONFIG_APP_TUYA
    it.name = "tuya";
    it.action = ACTION_TUYA;

#elif CONFIG_APP_CENTRAL
    it.name = "central";
    it.action = ACTION_CENTRAL_MAIN;

#elif CONFIG_APP_DONGLE
    it.name = "dongle";
    it.action = ACTION_DONGLE_MAIN;

#elif CONFIG_APP_BEACON
    it.name = "beacon";
    it.action = ACTION_BEACON_MAIN;

#elif CONFIG_APP_IDLE
    it.name = "idle";
    it.action = ACTION_IDLE_MAIN;

#elif CONFIG_APP_CONN_24G
    it.name = "conn_24g";
    it.action = ACTION_CONN_24G_MAIN;

#elif CONFIG_APP_FINDMY
    it.name = "findmy";
    it.action = ACTION_FINDMY;

#elif CONFIG_APP_FTMS
    it.name = "ftms";
    it.action = ACTION_FTMS;

#else
    while (1) {
        printf("no app!!!");
    }
#endif


    log_info("run app>>> %s", it.name);
    log_info("%s,%s", __DATE__, __TIME__);

    start_app(&it);

#if TCFG_CHARGE_ENABLE
    set_charge_event_flag(1);
#endif
}

/*
 * app模式切换
 */
void app_switch(const char *name, int action)
{
    struct intent it;
    struct application *app;

    log_info("app_exit\n");

    init_intent(&it);
    app = get_current_app();
    if (app) {
        /*
         * 退出当前app, 会执行state_machine()函数中APP_STA_STOP 和 APP_STA_DESTORY
         */
        it.name = app->name;
        it.action = ACTION_BACK;
        start_app(&it);
    }

    /*
     * 切换到app (name)并执行action分支
     */
    it.name = name;
    it.action = action;
    start_app(&it);
}

int eSystemConfirmStopStatus(void)
{
    /* 系统进入在未来时间里，无任务超时唤醒，可根据用户选择系统停止，或者系统定时唤醒(100ms) */
    //1:Endless Sleep
    //0:100 ms wakeup
    /* log_info("100ms wakeup"); */
    return 1;
}

__attribute__((used)) int *__errno()
{
    static int err;
    return &err;
}


