#ifndef __MENU_H__
#define __MENU_H__

extern int KeyNum;
typedef struct
{
	unsigned char current;
	unsigned char next;
	unsigned char enter;
	unsigned char back;
	void(*current_operation)(void)
}Menu_table;

void ParaA();
void ParaB();
void ParaC();
void ParaD();
void Menu_key_set();

#endif
