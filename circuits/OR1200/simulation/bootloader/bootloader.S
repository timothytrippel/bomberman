#include "or1k-asm.h"

/*
 * Define symbols to be used during startup - file is linked at compile time
 *
 */
.weak _or1k_board_mem_base
.weak _or1k_board_mem_size
.weak _or1k_board_clk_freq

_or1k_board_mem_base:	.long	0x0
_or1k_board_mem_size:	.long	0x100000

_or1k_board_clk_freq:	.long	50000000
	
/* Peripheral information - Set base to 0 if not present*/
.weak _or1k_board_uart_base
.weak _or1k_board_uart_baud
.weak _or1k_board_uart_IRQ
	
_or1k_board_uart_base:	.long	0x90000000
_or1k_board_uart_baud:	.long	115200
_or1k_board_uart_IRQ:	.long	2

.weak _or1k_board_exit
_or1k_board_exit:
        l.j _or1k_board_exit
        l.nop 0x1

.global _or1k_board_init_early
_or1k_board_init_early:
    OR1K_DELAYED_NOP(l.jr r9)

.weak _or1k_board_init
_or1k_board_init:
    OR1K_DELAYED_NOP(l.jr r9)