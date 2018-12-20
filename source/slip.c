#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SLIP_BUFSIZE (128)

struct slip {
    unsigned long    flags;  /* Flag values/ mode etc */
#define SLF_ESCAPE   1       /* ESC received          */
#define SLF_ERROR    2       /* Parity, etc. error    */

	/* buffer */
    unsigned char	 rbuff[SLIP_BUFSIZE];  /* receiver buffer		 */
	int              rcount;               /* received chars counter */

	unsigned char    xbuff[SLIP_BUFSIZE * 2];
	int              xleft;

	/* callback */
	void (*bump)(void *, unsigned char *buf, int count);
	int  (*write)(void *, const void *buf, int count);
	void *user;

	/* statics */
	unsigned long rx_over_errors;
	unsigned long rx_packets;
	unsigned long rx_bytes;
	unsigned long tx_dropped;
	unsigned long tx_packets;
	unsigned long tx_bytes;
};

/* SLIP protocol characters. */
#define END             0300		/* indicates end of frame	*/
#define ESC             0333		/* indicates byte stuffing	*/
#define ESC_END         0334		/* ESC ESC_END means END 'data'	*/
#define ESC_ESC         0335		/* ESC ESC_ESC means ESC 'data'	*/

static inline void clear_bit(int bit, unsigned long *v)
{
	*v &= ~(1 << bit);
}

static inline void set_bit(int bit, unsigned long *v)
{
	*v |= (1 << bit);
}

static inline int test_bit(int bit, unsigned long *v)
{
	return ((*v) & (1 << bit));
}

static inline int test_and_clear_bit(int bit, unsigned long *v)
{
	int r = test_bit(bit, v);
	clear_bit(bit, v);
	return (r);
}

/*
 * STANDARD SLIP ENCAPSULATION
 */

static int slip_esc(const unsigned char *s, unsigned char *d, int len)
{
	unsigned char *ptr = d;
	unsigned char c;

	/*
	 * Send an initial END character to flush out any
	 * data that may have accumulated in the receiver
	 * due to line noise.
	 */

	*ptr++ = END;

	/*
	 * For each byte in the packet, send the appropriate
	 * character sequence, according to the SLIP protocol.
	 */

	while (len-- > 0) {
		switch (c = *s++) {
		case END:
			*ptr++ = ESC;
			*ptr++ = ESC_END;
			break;
		case ESC:
			*ptr++ = ESC;
			*ptr++ = ESC_ESC;
			break;
		default:
			*ptr++ = c;
			break;
		}
	}
	*ptr++ = END;
	return ptr - d;
}

/* Write out any remaining transmit buffer. Scheduled when tty is writable */
int slip_flush(struct slip *sl)
{
	int actual;

	if (sl->xleft > 0) {
		actual = sl->write(sl->user, sl->xbuff, sl->xleft);
		if (actual > 0) {
			sl->xleft -= actual;
			if (sl->xleft > 0) {
				memmove(sl->xbuff, sl->xbuff + actual, sl->xleft);
			}
		}
	}
	return sl->xleft > 0;
}

/* Encapsulate one IP datagram and stuff into a TTY queue. */
int slip_transmit(struct slip *sl, const void *icp, int len)
{
	const unsigned char *p;
	int count;

	if ((len + sl->xleft) > SLIP_BUFSIZE) {
		/* Sigh, shouldn't occur BUT ... */
		printf("%p: truncating oversized transmit packet!\n", sl);
		sl->tx_dropped++;
		return -1;
	}

	p = icp;
	count = slip_esc(p, sl->xbuff + sl->xleft, len);
	sl->xleft += count;
	sl->tx_packets++;
	sl->tx_bytes += len;

	slip_flush(sl);

	return sl->xleft;
}


/* Send one completely decapsulated IP datagram to the IP layer. */
static void sl_bump(struct slip *sl)
{
	int count;

	count = sl->rcount;
	sl->rx_bytes += count;

	if (sl->bump)
		sl->bump(sl->user, sl->rbuff, count);

	sl->rx_packets++;
}

static void slip_unesc(struct slip *sl, unsigned char s)
{
	switch (s) {
	case END:
		if (!test_and_clear_bit(SLF_ERROR, &sl->flags) &&
		    (sl->rcount > 2))
			sl_bump(sl);
		clear_bit(SLF_ESCAPE, &sl->flags);
		sl->rcount = 0;
		return;

	case ESC:
		set_bit(SLF_ESCAPE, &sl->flags);
		return;
	case ESC_ESC:
		if (test_and_clear_bit(SLF_ESCAPE, &sl->flags))
			s = ESC;
		break;
	case ESC_END:
		if (test_and_clear_bit(SLF_ESCAPE, &sl->flags))
			s = END;
		break;
	}
	if (!test_bit(SLF_ERROR, &sl->flags))  {
		if (sl->rcount < SLIP_BUFSIZE)  {
			sl->rbuff[sl->rcount++] = s;
			return;
		}
		sl->rx_over_errors++;
		set_bit(SLF_ERROR, &sl->flags);
	}
}

void slip_receive(struct slip *sl, const void *_cp, int count)
{
	const unsigned char *cp = _cp;
	while (count--) {
		slip_unesc(sl, *cp++);
	}
}

struct slip *slip_alloc(void *ud, void (*bump)(void *ud, unsigned char *buf, int count), int  (*write)(void *ud, const void *buf, int count))
{
	struct slip *sl;

	sl = calloc(1, sizeof *sl);
	if (!sl) {
		fprintf(stderr, "%s: out of memory.\n", __func__);
		return NULL;
	}
	sl->user  = ud;
	sl->bump  = bump;
	sl->write = write;
	return sl;
}

void slip_free(struct slip *sl)
{
	if (sl->xleft) {
		fprintf(stderr, "%s: drop %d bytes.\n", __func__, sl->xleft);
	}
	free(sl);
}

void slip_debug(struct slip *sl, int rst)
{
	printf("rx_over_errors: %lu\n", sl->rx_over_errors);
	printf("rx_packets:     %lu\n", sl->rx_packets);
	printf("rx_bytes:       %lu\n", sl->rx_bytes);
	printf("tx_dropped:     %lu\n", sl->tx_dropped);
	printf("tx_packets:     %lu\n", sl->tx_packets);
	printf("tx_bytes:       %lu\n", sl->tx_bytes);

	if (rst) {
		sl->rx_over_errors = 0;
		sl->rx_packets     = 0;
		sl->rx_bytes       = 0;
		sl->tx_dropped     = 0;
		sl->tx_packets     = 0;
		sl->tx_bytes       = 0;
	}
}

