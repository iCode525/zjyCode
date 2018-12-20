#ifndef __SLIP_H___
#define __SLIP_H___
#ifdef __cplusplus
extern "C" {
#endif

struct slip;

/*
 * push received raw data to slip decoder.
 *
 * may be lead to `bump' to be called.
 */
void slip_receive(struct slip *sl, const void *cp, int count);

/*
 * send data to line discpline.
 */
int  slip_transmit(struct slip *sl, const void *icp, int len);

/*
 * flush out data in slip.
 *
 * may be call `write' to send data.
 */
int  slip_flush(struct slip *sl);

/*
 * allocate slip object.
 */
struct slip *slip_alloc(void *ud, void (*bump)(void *ud, const void *buf, int count),
		int (*write)(void *ud, const void *buf, int count));

/*
 * free slip object.
 */
void slip_free(struct slip *sl);

/*
 * for debugging.
 */
void slip_debug(struct slip *sl, int rst);

#ifdef __cplusplus
}
#endif
#endif

