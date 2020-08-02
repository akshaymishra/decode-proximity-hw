#include <stdint.h>
#include <stdio.h>
#include "dp3t-config.h"
#include "dp3t.h"

extern beacons_t *beacons;
static void print_ephid(const uint8_t *x);

void print_hex(const uint8_t *x, int len)
{
    int i;
    for(i = 0; i < len; i++) {
        printf("%02x",x[i]);
    }
    printf("\n");
}

void print_sk(const uint8_t *x)
{
    print_hex(x, SK_LEN);
}


void print_dp3t_ephids(void)
{
    int i;
    for (i = 0; i < EPOCHS; i++) {
        printf("[ %03d ] ", i);
        print_ephid(beacons->ephids[i]);
    }
}

static void print_ephid(const uint8_t *x)
{
    print_hex(x, EPHID_LEN);
}

