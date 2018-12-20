#ifndef __SREG_H__
#define __SREG_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

struct sreg;

#define SREG_CMD_RR  (0x01)
#define SREG_CMD_RD  (0x02)
#define SREG_CMD_WR  (0x03)

struct sreg *
sreg_alloc(void *ud, 
		int (*report)(void *ud, int cmd, uint16_t start_reg, uint16_t *var, int count),
		int (*output)(void *ud, const void *data, int count));

void sreg_free(struct sreg *);

void sreg_write(struct sreg *, uint16_t start_reg, uint16_t *val, int count);
void sreg_read(struct sreg *, uint16_t start_reg, int count);

void sreg_push(struct sreg *, const void *data, int count);
int  sreg_flush(struct sreg *);

#ifdef __cplusplus
}
#endif
#endif

