#ifndef CONFIG_BOARD_WT9011DCL_BT50_POST_BUILD_CFG_H
#define CONFIG_BOARD_WT9011DCL_BT50_POST_BUILD_CFG_H

/* 鏀规枃浠跺彧娣诲姞鍜宨sd_config.ini鐩稿叧鐨勯厤缃紝鐢ㄤ互鐢熸垚isd_config.ini */
/* 鍏朵粬涓嶇浉鍏崇殑閰嶇疆璇峰嬁娣诲姞鍦ㄦ敼鏂囦欢 */

#ifdef CONFIG_BOARD_WT9011DCL_BT50

/* Following Macros Affect Periods Of Both Code Compiling And Post-build */

#define CONFIG_DOUBLE_BANK_ENABLE               0       //鍗曞弻澶囦唤閫夋嫨(鑻ユ墦寮€浜嗘敼瀹?FLASH缁撴瀯鍙樹负鍙屽浠界粨鏋勶紝閫傜敤浜庢帴鍏ョ涓夋柟鍗忚鐨凮TA锛?PS: JL-OTA鍚屾牱鏀寔鍙屽浠藉崌绾? 闇€瑕佹牴鎹疄闄匜LASH澶у皬鍚屾椂閰嶇疆CONFIG_FLASH_SIZE)
#define CONFIG_APP_OTA_ENABLE                   0       //鏄惁鏀寔RCSP鍗囩骇(JL-OTA)

#define CONFIG_UPDATE_JUMP_TO_MASK              0   	//閰嶇疆鍗囩骇鍒發oader鐨勬柟寮?涓虹洿鎺eset,1涓鸿烦杞?閫傜敤浜庤姱鐗囩數婧愮敱IO鍙EEP浣忕殑鏂规,闇€瑕佹敞鎰忔鏌ヨ烦杞墠鏄惁灏嗕娇鐢―MA鐨勭‖浠舵ā鍧楀叏閮ㄥ叧闂?

#define CONFIG_LP_TOUCH_KEY_EN					0		//閰嶇疆鏄惁浣跨敤鍐呯疆瑙︽懜
#define CONFIG_UPDATE_WITH_MD5_CHECK_EN			0 		//閰嶇疆鍗囩骇鏄惁鏀寔MD5鏍￠獙

#define CONFIG_ANC_ENABLE           			0		//閰嶇疆鏄惁鏀寔ANC

//flash size vaule definition
#define FLASH_SIZE_256K							0x40000
#define FLASH_SIZE_512K							0x80000
#define FLASH_SIZE_1M							0x100000
#define FLASH_SIZE_2M							0x200000
#define FLASH_SIZE_4M							0x400000

#define CONFIG_FLASH_SIZE                       FLASH_SIZE_1M    //閰嶇疆FLASH澶у皬


/* Above Macros Affect Periods Of Both Code Compiling And Post-build */

/* Following Macros Only For Post Bulid Configuaration */

#define CONFIG_DB_UPDATE_DATA_GENERATE_EN       0       //鏄惁鐢熸垚db_data.bin(鐢ㄤ簬绗笁鏂瑰崗璁帴鍏ヤ娇鐢?
#define CONFIG_ONLY_GRENERATE_ALIGN_4K_CODE     0    	//ufw鍙敓鎴?浠?K瀵归綈鐨勪唬鐮?

//config for supported chip version
#ifdef CONFIG_BR30_C_VERSION
#define CONFIG_SUPPORTED_CHIP_VERSION			C
#else
#define CONFIG_SUPPORTED_CHIP_VERSION			B,D,E,M,N,O,P
#endif

//DON'T MODIFY THIS CONFIG EXCEPT SDK PUBLISHER
#define CONFIG_CHIP_NAME                        AC632N              //闄や簡SDK鍙戝竷鑰?璇蜂笉瑕佷慨鏀?
//it can be modified before first programming,but keep the same as the original version
#define CONFIG_PID                              AC632N              //鐑у啓鎴栧己鍒跺崌绾т箣鍓嶅彲浠ヤ慨鏀?涔嬪悗鍗囩骇瑕佷繚鎸佷竴鑷?
//it can be modified before first programming,but keep the same as the original version
#define CONFIG_VID                              0.01				//鐑у啓鎴栧己鍒跺崌绾т箣鍓嶅彲浠ヤ慨鏀?涔嬪悗鍗囩骇瑕佷繚鎸佷竴鑷?

//Project with bluetooth,it must use OSC as PLL_SOURCE;
#define CONFIG_PLL_SOURCE_USING_LRC             0       			//PLL鏃堕挓婧愰€夋嫨 1:LRC 2:OSC

//config alignment size unit
#ifdef CONFIG_256K_FLASH
#define ALIGN_UNIT_256B             1                 				//FLASH瀵归綈鏂瑰紡閫夋嫨锛屽鏋滄槸256K鐨凢LASH锛岄€夋嫨256BYTE瀵归綈鏂瑰紡
#else
#define ALIGN_UNIT_256B             0
#endif

//partial platform check this config to select the uart IO for wired update
#define CONFIG_UART_UPDATE_PIN                  PB05

//isd_download loader/uboot/update_loader debug io config
//#define CONFIG_UBOOT_DEBUG_PIN                  PA05
//#define CONFIG_UBOOT_DEBUG_BAUD_RATE            1000000

//config long-press reset io pin,time,trigger level
#if CONFIG_LP_TOUCH_KEY_EN
#define CONFIG_RESET_PIN                        LDO  //io pin
#define CONFIG_RESET_TIME                       04   //unit:second
#define CONFIG_RESET_LEVEL                      1	 //tigger level(0/1)
#else
#define CONFIG_RESET_PIN                        PB01 //io pin
#define CONFIG_RESET_TIME                       08   //unit:second
#define CONFIG_RESET_LEVEL                      0	 //tigger level(0/1)
#endif

//reserved three custom cfg item for the future definition
//#define CONFIG_CUSTOM_CFG1_TYPE			POWER_PIN
//#define CONFIG_CUSTOM_CFG1_VALUE		 	PC01_1

//#define CONFIG_CUSTOM_CFG2_TYPE
//#define CONFIG_CUSTOM_CFG2_VALUE

//#define CONFIG_CUSTOM_CFG3_TYPE
//#define CONFIG_CUSTOM_CFG3_VALUE


//#define CONFIG_VDDIO_LVD_LEVEL                  4 ////VDDIO_LVD鎸′綅锛?: 1.55V   1: 1.70V   2: 1.85V   3: 2.00V   4: 2.15V   5: 2.30V   6: 2.45V   7: 2.60V

//with single-bank mode,actual vm size should larger this VM_LEAST_SIZE,and dual bank mode,actual vm size equals this;
#define CONFIG_VM_LEAST_SIZE                    8K
//config whether erased this area when do a update,1-No Operation,0-Erase
#define CONFIG_VM_OPT							1

//config whether erased this area when do a update,1-No Operation,0-Erase
#define CONFIG_BTIF_OPT							1

//reserved two custom cfg area for the future definition
//#define CONFIG_RESERVED_AREA1					EXIF1
#ifdef CONFIG_RESERVED_AREA1
#define CONFIG_RESERVED_AREA1_ADDR				AUTO
#define CONFIG_RESERVED_AREA1_LEN				0x1000
#define CONFIG_RESERVED_AREA1_OPT				1
//#define CONFIG_RESERVED_AREA1_FILE				anc_gains.bin
#endif

//#define CONFIG_RESERVED_AREA2					EXIF2
#ifdef CONFIG_RESERVED_AREA2
#define CONFIG_RESERVED_AREA2_ADDR				AUTO
#define CONFIG_RESERVED_AREA2_LEN				0x1000
#define CONFIG_RESERVED_AREA2_OPT				1
//#define CONFIG_RESERVED_AREA2_FILE				anc_gains.bin
#endif

/* Above Macros Only For Post Bulid Configuaration */
#endif

#endif
