

#ifndef _TS_NBUS_H
#define _TS_NBUS_H

struct ts_nbus;

extern int ts_nbus_read(struct ts_nbus *ts_nbus, u8 adr, u16 *val);
extern int ts_nbus_write(struct ts_nbus *ts_nbus, u8 adr, u16 val);

#endif 
