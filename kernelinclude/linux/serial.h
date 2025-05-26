
#ifndef _LINUX_SERIAL_H
#define _LINUX_SERIAL_H

#include <uapi/linux/serial.h>
#include <uapi/linux/serial_reg.h>

#define UART_IER_ALL_INTR	(UART_IER_MSI | \
				 UART_IER_RLSI | \
				 UART_IER_THRI | \
				 UART_IER_RDI)


#define UART_LCR_WLEN(x)	((x) - 5)


#define UART_LSR_BOTH_EMPTY	(UART_LSR_TEMT | UART_LSR_THRE)

static inline bool uart_lsr_tx_empty(u16 lsr)
{
	return (lsr & UART_LSR_BOTH_EMPTY) == UART_LSR_BOTH_EMPTY;
}

#define UART_MSR_STATUS_BITS	(UART_MSR_DCD | \
				 UART_MSR_RI | \
				 UART_MSR_DSR | \
				 UART_MSR_CTS)



struct async_icount {
	__u32	cts, dsr, rng, dcd, tx, rx;
	__u32	frame, parity, overrun, brk;
	__u32	buf_overrun;
};

#include <linux/compiler.h>

#endif 
