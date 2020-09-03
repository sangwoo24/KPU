#include <delay.h>
#include <mega128.h>

#include "C:\Users\���\Desktop\vhd\header\lcd2.h"


void LCD_PORT_Init(void)
{
    DDRA = 0xFF;    //PORTA�� �������
    DDRG = 0x0F;    //PORTG�� ���� 4��Ʈ�� �������
}

void LCD_Data(unsigned char ch)
{
    LCD_CTRL |= (1 << LCD_RS);    // RS=1, R/W��=0���� ������ ���� ����Ŭ
    LCD_CTRL &= ~(1 << LCD_RW);
    LCD_CTRL |= (1 << LCD_EN);    //LCD Enable
    delay_us(50);    //�ð� ����
    LCD_WDATA = ch;    //������ ���
    delay_us(50);    //�ð� ����
    LCD_CTRL &= ~(1 << LCD_EN);    //LCD Disable
}

void LCD_Comm(unsigned char ch)
{
    LCD_CTRL &= ~(1 << LCD_RS);    //RS=0���� ��ɾ� ���� ����Ŭ
    LCD_CTRL &= ~(1 << LCD_RW);
    LCD_CTRL |= (1 << LCD_EN);    //LCD Enable
    delay_us(50);    //�ð� ����
    LCD_WINST = ch;    //��ɾ� ����
    delay_us(50);    //�ð� ����
    LCD_CTRL &= ~(1 << LCD_EN);
}

void LCD_Delay(unsigned char ms)
{
    delay_ms(ms);
}

void LCD_Char(unsigned char c)    // �� ���� ���
{
    LCD_Data(c);
    delay_ms(1);
}

void LCD_Str(unsigned char *str)    //���ڿ� ���
{
    while (*str != 0) 
    {
        LCD_Char(*str);
        str++;
    }
}

void LCD_Pos(unsigned char x, unsigned char y)    // LCD ��ġ ����, x�� y�� ��� ���� �ǹ���
{
    LCD_Comm(0x80 | (x * 0x40 + y));    // x= ������(0,1), y = ���ο�(0-15)
}

void LCD_Clear(void)    //ȭ�� Ŭ����(1)
{
    LCD_Comm(0x01);
    LCD_Delay(2);
}

void LCD_Display_Shift(unsigned char p)
{
    if(p == RIGHT)
    {
        LCD_Comm(0x1c);
        LCD_Delay(1);
    }
    else if(p == LEFT)
    {
        LCD_Comm(0x18);
        LCD_Delay(1);
    }
}

void LCD_Cursor_Shift(unsigned char p)
{
    if(p == RIGHT)
    {
        LCD_Comm(0x14);
        LCD_Delay(1);
    }
    else if(p == LEFT)
    {
        LCD_Comm(0x10);
        LCD_Delay(1);
    }
}

void LCD_Cursor_Home(void)
{
    LCD_Comm(0x02);
    LCD_Delay(2);
}



void LCD_Init(void)    //LCD �ʱ�ȭ
{
    LCD_PORT_Init();    // LCD ��� ��Ʈ ����

    LCD_Comm(0x38);    //DDRAM, ������ 8��Ʈ ���, LCD 2���� ���(6)
    LCD_Delay(4);    //4ms ����
    LCD_Comm(0x38);    //DDRAM, ������ 8��Ʈ ���, LCD 2���� ���(6)
    LCD_Delay(4);    //4ms ����
    LCD_Comm(0x38);    //DDRAM, ������ 8��Ʈ ���, LCD 2���� ���(6)
    LCD_Delay(4);    //4ms ����
    LCD_Comm(0x0e);    //ȭ��ǥ�� ON/OFF
    LCD_Delay(2);    //2ms ����
    LCD_Comm(0x06);    //�ּ�+1, Ŀ���� �������� �̵�(3)
    LCD_Delay(2);    //2ms ����
    LCD_Clear();    //LCD ȭ�� Ŭ����
}