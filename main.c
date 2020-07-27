/*
 * Copyright (C) 2020 Dyne.org foundation
 *
 * This file is subject to the terms and conditions of the Affero GNU
 * General Public License (AGPL) version 3. See the file LICENSE for
 * more details.
 *
 */
#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <time.h>

#include "shell.h"
#include "msg.h"

#include "dp3t-config.h"
#include "dp3t.h"
#include "keystore.h"

#include "ble_scan.h"
#include "periph/rtc.h"

#include "embUnit.h"

#include "at25xxx.h"
#include "at25xxx_params.h"


#include "led.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int dtls_client(int argc, char **argv);
extern int dtls_server(int argc, char **argv);

#ifdef MODULE_WOLFCRYPT_TEST
extern int wolfcrypt_test(void* args);


static int wolftest(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    wolfcrypt_test(NULL);
    return 0;
}
#endif

#define TM_YEAR_OFFSET      (1900)

struct tm RTC_time = {
        .tm_year = 2020 - TM_YEAR_OFFSET,   /* years are counted from 1900 */
        .tm_mon  =  6,                      /* 0 = January, 11 = December */
        .tm_mday = 28,
        .tm_hour = 13,
        .tm_min  = 59,
        .tm_sec  = 57
    };

extern int gatt_server(void);

static const shell_command_t shell_commands[] = {
    { "testvec", "print test vectors", dp3t_shellcmd_testvec },
    { "rekey", "regenerate DP3T secure key", dp3t_shellcmd_rekey },
#ifdef MODULE_WOLFCRYPT_TEST
    { "wolftest", "Perform wolfcrypt porting test", wolftest },
#endif
    { NULL, NULL, NULL }
};

char line_buf[SHELL_DEFAULT_BUFSIZE];

int main(void)
{
    uint8_t *ephid, *sk_t0;

	/* The time parameter has to come from app*/
    rtc_set_time(&RTC_time);
	/* our eeprom init */
    at25xxx_init(&dev, &at25xxx_params[0]);
/*
    API to read and write. to be tested 
    at25xxx_write(&dev, AT25XXX_PARAM_PAGE_SIZE - 5, data_in_a, sizeof(data_in_a));
    at25xxx_read(&dev, AT25XXX_PARAM_PAGE_SIZE - 5, data_out, sizeof(data_out));

*/

    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);


    /* init board LED */
    LED_OFF(0);

    /* Global wolfSSL initialization */
    wolfSSL_Init();
    wolfSSL_Debugging_ON();

    /* dp3t */
    dp3t_start();

    /* Start dp3t gatt advertisements */
    gatt_server();

    /* Start dp3t scan service  */
    dp3t_blescan_init();

    /* start shell */
    printf( "All up, running the shell now\r\n");
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
