// ns16550a compatible
// current stage: somewhat primitive (no interrupts)
// check out: https://www.lammertbies.nl/comm/info/serial-uart

#include "dtb.h"
#include "lock.h"

#define BASE DTB_SERIAL
#define REG(reg) ((unsigned char *)(BASE + reg))
#define R_REG(reg) (*REG(reg))
#define W_REG(reg, val) (*REG(reg) = val)

// receive buffer register
#define RBR 0

// transmitter holding register
#define THR 0

// divisor latch LSB
#define DLL 0
#define DLL_38400 0x03

// interrupt enable register
#define IER 1
#define IER_RECEIVED_DATA_AVAILABLE (1 << 0)
#define IER_THR_EMPTY (1 << 1)
#define IER_RECEIVER_LSR_CHANGE (1 << 2)
#define IER_MSR_CHANGE (1 << 3)

// divisor latch MSB
#define DLM 1
#define DLM_38400 0x00

// interrupt identification register
#define IIR 2
// value	description				reset by
// --------------------------------------------------------------------------
// xxxxxxx0	Interrupt pending			–
// xxxxxxx1	No interrupt pending			–
// xxxx000x	Modem status change			MSR read
// xxxx001x	Transmitter holding register empty	IIR read or THR write
// xxxx010x	Received data available			RBR read
// xxxx011x	Line status change			LSR read
// xxxx110x	Character timeout			RBR read
// xx00xxxx	Reserved				-
// 00xxxxxx	No FIFO					–
// 01xxxxxx	Unusable FIFO				–
// 11xxxxxx	FIFO enabled				–

// FIFO control register
#define FCR 2
#define FCR_ENABLE_FIFO (1 << 0)
#define FCR_CLEAR_RECEIVE_FIFO (1 << 1)
#define FCR_CLEAR_TRANSMIT_FIFO (1 << 2)
// value	description
// ------------------------------------------
// xxxx0xxx	Select DMA mode 0
// xxxx1xxx	Select DMA mode 1
// xx00xxxx	Reserved
// 00xxxxxx	1 byte
// 01xxxxxx	4 bytes
// 10xxxxxx	8 bytes
// 11xxxxxx	14 bytes

// line control register
#define LCR 3
#define LCR_DLAB (1 << 7)		// enable baud mode frequency setting
#define LCR_EIGHT_BITS (0b11 << 0)	// 8 bits, no parity

// modem control register
#define MCR 4
#define MCR_DATA_TERMINAL_READY (1 << 0)
#define MCR_RTS (1 << 1)	// request to send
#define MCR_AUX1 (1 << 2)	// auxilary output 1
#define MCR_AUX2 (1 << 3)	// auxilary output 2
#define MCR_LOOPBACK_MODE (1 << 4)

// line status register
#define LSR 5
#define LSR_DATA_AVAILABLE (1 << 0)
#define LSR_OVERRUN_ERROR (1 << 1)
#define LSR_PARITY_ERROR (1 << 2)
#define LSR_FRAMING_ERROR (1 << 3)
#define LSR_BREAK_SIGNAL_RECVD (1 << 4)
#define LSR_THR_EMPTY (1 << 5)
#define LSR_THR_EMPTY_AND_LINE_IDLE (1 << 6)
#define LSR_ERRORNOUS_DATA_IN_FIFO (1 << 7)

// modem status register
#define MSR 6
#define MSR_CHANGE_IN_CTS (1 << 0)
#define MSR_CHANGE_IN_DATA_SET_READY (1 << 1)
#define MSR_TRAILING_EDGE_RING_INDICATOR (1 << 2)
#define MSR_CHANGE_IN_CARRIER_DETECT (1 << 3)
#define MSR_CTS (1 << 4)	// clear to send
#define MSR_DATA_SET_READY (1 << 5)
#define MSR_RING_INDICATOR (1 << 6)
#define MSR_CARRIER_DETECT (1 << 7)

// scratch register
#define SCR 7

void uartputc(char c);
void init_uart();

static struct lock uart_lk;

void
uartputc(char c)
{
	acquire(&uart_lk);
	while ((R_REG(LSR) & LSR_THR_EMPTY) == 0);
	W_REG(THR, c);
	release(&uart_lk);
}

// TODO: test this function (it was written in the heat of the moment / no real need for it at the time)
// type int so it can contain character and an error code situation (-1)
//int
//uartgetc()
//{
//	if (R_REG(LSR) & LSR_DATA_AVAILABLE)
//		return R_REG(RBR);
//	return -1;
//}

void
init_uart()
{
	// disable all interrupts
	W_REG(IER, 0);

	// set baud rate to 38400
	W_REG(LCR, LCR_DLAB);		// enter set-baud-rate mode
	W_REG(DLL, DLL_38400);
	W_REG(DLM, DLM_38400);
	W_REG(LCR, LCR_EIGHT_BITS);	// exit set-baud-rate mode

	// enable FIFO
	W_REG(FCR, FCR_ENABLE_FIFO | FCR_CLEAR_RECEIVE_FIFO | FCR_CLEAR_TRANSMIT_FIFO);

	// no control flow
	W_REG(MCR, 0);

	init_lock(&uart_lk, "uart");
}
