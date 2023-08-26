#ifndef __uart_h__
#define __uart_h__


void uart_init();
void uart_tx(char ch);
char uart_rx();
void uart_write(char* str);


#endif
