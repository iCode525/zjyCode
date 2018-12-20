#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slip.h"
#include "sreg.h"

#define SREG_READ   (0x81)
#define SREG_WRITE  (0x82)
#define SREG_REPORT (0x83)

struct sreg {
	struct slip *sl;

	int (*report)(void *ud, int cmd, uint16_t reg, uint16_t *var, int count);
	int (*output)(void *ud, const void *data, int count);

	void *data;
};

static uint16_t __do_crc(uint8_t data, uint16_t crc)   
{   
	uint16_t check = 0;   
	crc ^= data;

	for(int i = 0; i < 8; i++) {   
		check = crc & 1;   
		crc = crc >> 1;   
		crc = crc & 0x7fff;   

		if(check == 1)   {   
			crc = crc ^ 0xa001;   
		}
		crc = crc & 0xffff;   
	}   
	return crc;   
}

uint16_t crc16(const void * d, int len)
{   
	const uint8_t *cp = d;
	uint16_t crc = 0xffff;

	while (len > 0) {
		crc = __do_crc(*cp++, crc);   
		--len;
	}
	return crc;
}   

static int __write(void *ud, const void *buf, int count)
{
	struct sreg *sr = ud;
	int r;

	r =  sr->output(sr->data, buf, count);
	return (r);
}

static int __do_read(struct sreg *sr, uint16_t start, int n)
{
	uint16_t var[3 + n];

	memset(var, 0, sizeof var);
	sr->report(sr->data, SREG_CMD_RD, start, var + 2, n);
	var[0] = ((n & 0xff) << 8) | SREG_REPORT;
	var[1] = start;
	var[n+2] = crc16(var, sizeof var - sizeof var[0]);
	return slip_transmit(sr->sl, var, sizeof var);
}

static void __bump(void *ud, const void *buf, int count)
{
	struct sreg *sr = ud;
	const uint16_t *dp = buf;
	int cmd, n;

	if ((count % 2)) {
		fprintf(stderr, "%s: data not aligned (%d).\n", __func__, count);
	}
	count /= 2;
	if (count < 3) {
		fprintf(stderr, "%s: data too short.\n", __func__);
	}
	n = (dp[0] >> 8) & 0xff;
	cmd = dp[0] & 0xff;
	switch (cmd) {
		case SREG_READ:
			__do_read(sr, dp[1], n);
			break;
		case SREG_WRITE:
			if (n > (count - 3)) {
				fprintf(stderr, "%s: reg value is not enough?\n", __func__);
				break;
			}
			sr->report(sr->data, SREG_CMD_WR, dp[1], (void *)(dp + 2), n);
			break;
		case SREG_REPORT:
			if (n > (count -3)) {
				fprintf(stderr, "%s (%d:%d): report reg value is not enough? \n", __func__, n, count);
				break;
			}
			sr->report(sr->data, SREG_CMD_RR, dp[1], (void *)(dp + 2), n);
			break;
	}
}

struct sreg * sreg_alloc(void *ud, 
		int (*report)(void *ud, int cmd, uint16_t reg, uint16_t *var, int count),
		int (*output)(void *ud, const void *data, int count))
{
	struct sreg *sr;

	sr = calloc(1, sizeof *sr);
	if (!sr) {
		fprintf(stderr, "%s: out of memory.\n", __func__);
		return NULL;
	}
	sr->data = ud;
	sr->report = report;
	sr->output = output;
	sr->sl = slip_alloc(sr, __bump, __write);
	if (!sr->sl) {
		fprintf(stderr, "%s: slip object is null.\n", __func__);
		free(sr);
		return NULL;
	}
	return (sr);
}

void sreg_read(struct sreg *sr, uint16_t reg, int count)
{
	uint16_t data[3];

	data[0] = (count & 0xff) << 8 | SREG_READ; /* hi byte: cmd, low byte: length */
	data[1] = reg; /* start register */
	data[2] = crc16(data, sizeof data - sizeof data[0]); /* ignore last crc field */
	slip_transmit(sr->sl, data, sizeof data);
}

void sreg_write(struct sreg *sr, uint16_t reg, uint16_t *var, int n)
{
	uint16_t data[n + 3];
	data[0] = ((n & 0xff) << 8) | SREG_WRITE;
	data[1] = reg;
	memcpy(data + 2, var, n * sizeof var[0]);
	data[n + 2] = crc16(data, sizeof data - sizeof data[0]);
	slip_transmit(sr->sl, data, sizeof data);
}

void sreg_free(struct sreg *sr)
{
	slip_free(sr->sl);
	free(sr);
}

void sreg_push(struct sreg *sr, const void *data, int count)
{
	slip_receive(sr->sl, data, count);
}

int  sreg_flush(struct sreg *sr)
{
	return slip_flush(sr->sl);
}

