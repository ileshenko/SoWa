#ifndef _UART_H_
#define _UART_H_

void uart_init(void);
void uart_report(char *line);

char *cat_ul(char *buf, unsigned long val);
char *cat_str(char *buf, char *str);
#define tab_ul(buf, val) cat_ul(cat_str(buf, "\t"), val)

#endif
