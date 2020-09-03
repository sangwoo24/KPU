#include <mega128.h>
#include <delay.h>
#include "C:\Users\���\Desktop\vhd\header\lcd2.h"


#define FND_data PORTB
#define FND_dig PORTE

#define FND_Null        17
#define FND_Star        14
#define FND_Sharp       15

#define     Do      1908
#define     Re      1700
#define     Mi      1515
#define     Fa      1432
#define     Sol     1275
#define     Ra      1136
#define     Si      1012


unsigned char key_Num = 0;
unsigned char key, sec = 0, mode, min = 0, k, hour, day, month, keynum, alram_key, alram_keynum;    //Ű���� �ޱ� ���� ������� �ʱⰪ�� 
unsigned int i, j, cnt, year;
unsigned char first[] = "2014-12-05";         //�ʱ�ȭ�� ���� ���ڿ� 
unsigned char second[] = "AM 12:00:00";
unsigned char AM[] = "AM";
unsigned char PM[] = "PM";
unsigned char str[] = "Key Setting!!";
unsigned char str2[] = "---Insert Key---";
unsigned char str3[] = "alram set mode!!";
unsigned char str4[] = "--insert alram--";
unsigned char str5[] = "     CLEAR!     ";
unsigned char alram[3][10];  //10�ڸ� 2�����迭 
unsigned char str7[] = "Setting Complite!";
unsigned char time[14];      //���⿡ �ð� ������ �� ����.
bit timer_flag;              //���� ȭ����°� ����Ʈ/Ż�� �� ���� �÷��׵� 
bit dir, flag_chattering;
bit buzzer_flag;
bit escape_flag;
bit dir;
bit delete_flag;

unsigned char alram_month1, alram_month2, alram_month3, alram_day1, alram_day2, alram_day3, alram_hour1, alram_hour2;  //�˶��ð��� �����ϱ� ���� ������ 
unsigned char alram_hour3, alram_min1, alram_min2, alram_min3, alram_sec1, alram_sec2, alram_sec3;

interrupt[EXT_INT0] void ext_int0(void)      //���ͷ�Ʈ 0�� ��� -> PIND.0
{
	buzzer_flag = 0;
	LCD_Clear();
	delay_ms(100);
}

void init_reg(void)
{
	EIMSK = 0x01;           //���ͷ�Ʈ 0���
	EICRA = 0x02;           //�ϰ����� ���� 
	DDRB = 0xff;
	DDRC = 0x0f;            // ���� 4bit Col(�Է�), ���� 4bit Row(���)
	PORTC = 0x0f;
	DDRG |= (1 << 4);         //������� 
	DDRD = 0x00;
	SREG |= 0x80;           //SEI
}
void init_timer(void)
{
	TIMSK = (1 << TOIE0);
	TCCR0 = (1 << CS02) | (1 << CS01) | (1 << CS00); //1024 Ÿ�̸�ī���� 0
	TCNT0 = -5;
	SREG |= 0x80;
}

interrupt[TIM0_OVF] void timer0_overflow(void)
{
	TCNT0 = -5;
	if (timer_flag == 0) cnt++;  //timer_flag�� 1�̸� ī���� ���� ���� (�ð� ������)

	if (cnt == 2881)            //�ð輳��. 1������
	{
		sec++;
		cnt = 0;
	}
	if (sec == 60)
	{
		sec = 0;
		min++;
	}
	if (min == 60)
	{
		min = 0; hour++;
	}
	if (hour == 24)
	{
		hour = 0; day++;
	}
}

void myDelay_us(unsigned int delay)     //������ ����ϱ� ���� �������Լ� 
{
	int i;
	for (i = 0; i < delay; i++)
	{
		delay_us(1);
	}
}

void SSound(int time)                   //���� �Լ� 
{
	int i, tim;
	tim = 50000 / time;
	for (i = 0; i < tim; i++) // ���踶�� ���� �ð� ���� �︮���� time ���� ���
	{
		PORTG |= 1 << PORTG4; //����ON, PORTG�� 4�� �� ON(out 1)
		myDelay_us(time);
		PORTG &= ~(1 << PORTG4);  //����OFF, PORTG�� 4�� �� OFF(out 0)
		myDelay_us(time);
	}
}

unsigned char KeyScan(void)                     // 4X4 Ű�е� ��ĵ �Լ�, ��� ���� 10���� 1~16  {
{
	unsigned int Key_Scan_Line_Sel = 0xf7;      // Init_data ���� �Ϻ��� ����� ����  
	// ���� �Ϻ�(4bit)�� ����Ī(���������� ���ư��鼭)�ϸ鼭 ���    
	unsigned char Key_Scan_sel = 0, key_scan_num = 0;
	unsigned char Get_Key_Data = 0;            // ���� Ű ������        

	//Ű��ĵ �κ�  
	for (Key_Scan_sel = 0; Key_Scan_sel < 4; Key_Scan_sel++)
	{
		// �ʱ�ȭ 
		PORTC = Key_Scan_Line_Sel;
		delay_us(10);

		//���� �κ�
		Get_Key_Data = (PINC & 0xf0);   // 74LS14�� ������ ���      

		if (Get_Key_Data != 0x00)
		{
			switch (Get_Key_Data)        // C��Ʈ ������ ���� �Ϻ�(4bit)�� ����            
			{
			case 0x10:            // 0001�� ������ ���� count���� 4�� ������                                        
								  //  1�� ���ϰ� key_scan_num������ ����
				key_scan_num = Key_Scan_sel * 4 + 1;
				break;
			case 0x20:            // 0010�� ������ ���� count���� 4�� ������
								  //  2�� ���ϰ� key_scan_num������ ����
				key_scan_num = Key_Scan_sel * 4 + 2;
				break;
			case 0x40:            // 0100�� ������ ���� count���� 4�� ������ 
								  //  3�� ���ϰ� key_scan_num������ ����           
				key_scan_num = Key_Scan_sel * 4 + 3;
				break;
			case 0x80:            // 1000�� ������ ���� count���� 4�� ������                                                      
								  //  4�� ���ϰ� key_scan_num������ ����                                         
				key_scan_num = Key_Scan_sel * 4 + 4;
				break;
			default:
				key_scan_num = FND_Null;
				break;
			}
			return key_scan_num;
		}
		Key_Scan_Line_Sel = (Key_Scan_Line_Sel >> 1);
		//Init_data�� ����Ʈ ��, Key_Scan_Line �̵�     
	}
	return key_scan_num;
}
void time_set(void)
{

	for (k = 0; k <= 13; k++)           //�ʱ�ȭ 
	{
		time[k] = 0;
	}
	i = 0;
	LCD_Pos(0, 5);
	LCD_Char('-');

	LCD_Pos(0, 8);
	LCD_Char('-');

	LCD_Pos(1, 6);
	LCD_Char(':');

	LCD_Pos(1, 9);
	LCD_Char(':');
	LCD_Pos(0, 1);

	while (1)
	{


		key = KeyScan();
		if (key == 0)                 //ä�͸� ���� �ڵ� 
		{
			flag_chattering = 0;
		}

		else
		{
			if (!flag_chattering)      //ä�͸� ���� �ڵ� 
			{
				flag_chattering = 1;

				if (key)
				{
					if (key == 16)            //�߰� Ż���ư 
					{
						timer_flag = 0;
						break;
					}
					else if (key == 4)        //���� �Ϸ� �� Ż���ư
					{
						escape_flag = 1;
					}
					else if (key == 15)       //�ѱ��� �� �����ϴ� ��ư 
					{
						delete_flag = 1;
					}

					if (key == 14)            //0�� �ȳ��ͼ� 0�� ������ַ��� ��� 
					{
						keynum = 0;
					}
					else if (key % 4 != 0)
					{
						keynum = (key / 4) * 3 + (key % 4);
					}
					else
					{
						keynum = (key / 4) + 9;
					}
					if (delete_flag == 0)
					{
						if (dir == 0)
						{
							time[i] = (keynum + 0x30);     //Ű�� �ް� �迭�� �ְ� lcd�� ��� 
							LCD_Char(time[i]);
							delay_ms(300);
							i++;
						}
						else;

						if (escape_flag == 1)               //���� ���ϰ� Ż�� ��ư�� ���� �� 
						{
							timer_flag = 0;
							LCD_Clear();     //�Է� ���ϸ� clear��Ű�� setting complite! ���� 
							delay_ms(10);
							LCD_Pos(0, 0);
							LCD_Str(str7);   //�ϰ� �迭 14�� ���� timer_intr ���� �ð豸��. 
							delay_ms(1500);
							LCD_Clear();
							delay_ms(50);

							year = ((time[0] - 0x30) * 1000) + ((time[1] - 0x30) * 100) + ((time[2] - 0x30) * 10) + (time[3] - 0x30);
							month = ((time[4] - 0x30) * 10) + (time[5] - 0x30);        //ȭ�鿡 ���� ��¥/�簣 ���� ����ϱ����� ������ ���� 
							day = ((time[6] - 0x30) * 10) + (time[7] - 0x30);
							hour = ((time[8] - 0x30) * 10) + (time[9] - 0x30);
							min = ((time[10] - 0x30) * 10) + (time[11] - 0x30);
							sec = ((time[12] - 0x30) * 10) + (time[13] - 0x30);
							escape_flag = 0;
							dir = 0;
							break;
						}
						if (i == 4)  LCD_Pos(0, 6);     //pos�� ���� 
						if (i == 6)  LCD_Pos(0, 9);
						if (i == 8)  LCD_Pos(1, 4);
						if (i == 10) LCD_Pos(1, 7);
						if (i == 12) LCD_Pos(1, 10);
						if (i == 14) dir = 1;
					}
					else if (delete_flag == 1)         //������ ���� �ڵ� ��
					{
						if ((i >= 1) && (i <= 4))      //pos���� �ڲ� �ٲ�� ������ ���� ��ġ���� ����Ʈ�ϴ� ��ġ(pos)�� �����ϰ�
						{                             //���鹮�ڸ� �߰��Ͽ� �����Ѱ�ó�� ���� 
							dir = 0;
							LCD_Pos(0, i);
							LCD_Char(' ');
							LCD_Pos(0, i);
							delay_ms(200);
							delete_flag = 0;
							i--;
						}
						else if (i == 5 || i == 6)
						{
							dir = 0;
							LCD_Pos(0, i + 1);
							LCD_Char(' ');
							LCD_Pos(0, i + 1);
							delay_ms(200);
							delete_flag = 0;
							i--;
						}
						else if (i == 7 || i == 8)
						{
							dir = 0;
							LCD_Pos(0, i + 2);
							LCD_Char(' ');
							LCD_Pos(0, i + 2);
							delay_ms(200);
							delete_flag = 0;
							i--;
						}
						else if (i == 9 || i == 10)
						{
							dir = 0;
							LCD_Pos(1, i - 5);
							LCD_Char(' ');
							LCD_Pos(1, i - 5);
							delay_ms(200);
							delete_flag = 0;
							i--;
						}
						else if (i == 11 || i == 12)
						{
							dir = 0;
							LCD_Pos(1, i - 4);
							LCD_Char(' ');
							LCD_Pos(1, i - 4);
							delay_ms(200);
							delete_flag = 0;
							i--;
						}
						else if (i == 13 || i == 14)
						{
							dir = 0;
							LCD_Pos(1, i - 3);
							LCD_Char(' ');
							LCD_Pos(1, i - 3);
							delay_ms(200);
							delete_flag = 0;
							i--;
						}
						else delete_flag = 0;
					}
				}
			}
		}
	}
}

void alram_set(void)                        //10�ڸ� �Է�x3 �� �ٷ�Ż�� -> main
{
	for (j = 0; j < 3; j++)                        //�ʱ�ȭ 
	{
		for (k = 0; k < 10; k++)
		{
			alram[j][k] = 0;
		}
	}
	i = j = 0;

	LCD_Pos(0, 6);
	while (1)
	{
		if (j == 0)                           //�ʱ⿡ �⵵�� ǥ���ϱ� ���ؼ� ���� 
		{
			LCD_Pos(0, 1);
			LCD_Char((year / 1000) + '0');
			LCD_Char(((year % 1000) - (year % 100)) / 100 + '0');
			LCD_Char(((year % 100) - (year % 10)) / 10 + '0');
			LCD_Char((year % 10) + '0');

			LCD_Pos(0, 5);
			LCD_Char('-');

			LCD_Pos(0, 8);
			LCD_Char('-');
			LCD_Pos(1, 6);
			LCD_Char(':');
			LCD_Pos(1, 9);
			LCD_Char(':');
			LCD_Pos(0, 6);
		}

		alram_key = KeyScan();                //Ű �ް� 
		if (alram_key == 0)                    //ä�͸� ���� �ڵ� 
		{
			flag_chattering = 0;
		}
		else
		{
			if (!flag_chattering)              //ä�͸� ���� �ڵ� 
			{
				flag_chattering = 1;

				if (alram_key)
				{
					if (alram_key == 16)       //Ż�� 
					{
						break;
					}
					if (alram_key == 14)
					{
						alram_keynum = 0;
					}
					else if (alram_key % 4 != 0)
					{
						alram_keynum = (alram_key / 4) * 3 + (alram_key % 4);
					}
					else
					{
						alram_keynum = (alram_key / 4) + 9;
					}

					alram[i][j] = (alram_keynum + 0x30);   //Ű �ް� ȭ�鿡 ��� 
					LCD_Char(alram[i][j]);
					delay_ms(300);

					if (j != 10)
					{
						j++;
					}

					if (j == 10)                         //10�ڸ� �� �Է½� ȭ����� �� ���� 
					{
						i++;
						j = 0;
						LCD_Clear();
						delay_ms(10);
						LCD_Str(str5);
						delay_ms(1000);
						LCD_Clear();
						delay_ms(10);
						LCD_Pos(0, 6);
						delay_ms(50);
					}
					if (j == 2)  LCD_Pos(0, 9);
					else if (j == 4)  LCD_Pos(1, 4);
					else if (j == 6)  LCD_Pos(1, 7);
					else if (j == 8)  LCD_Pos(1, 10);

					if (i == 3)  break;
				}
				alram_month1 = ((alram[0][0] - 0x30) * 10) + (alram[0][1] - 0x30);      //���ϱ� ���� �����鿡 �迭���� ���� 
				alram_day1 = ((alram[0][2] - 0x30) * 10) + (alram[0][3] - 0x30);
				alram_hour1 = ((alram[0][4] - 0x30) * 10) + (alram[0][5] - 0x30);
				alram_min1 = ((alram[0][6] - 0x30) * 10) + (alram[0][7] - 0x30);
				alram_sec1 = ((alram[0][8] - 0x30) * 10) + (alram[0][8] - 0x30);

				alram_month2 = ((alram[1][0] - 0x30) * 10) + (alram[1][1] - 0x30);
				alram_day2 = ((alram[1][2] - 0x30) * 10) + (alram[1][3] - 0x30);
				alram_hour2 = ((alram[1][4] - 0x30) * 10) + (alram[1][5] - 0x30);
				alram_min2 = ((alram[1][6] - 0x30) * 10) + (alram[1][7] - 0x30);
				alram_sec2 = ((alram[1][8] - 0x30) * 10) + (alram[1][8] - 0x30);

				alram_month3 = ((alram[2][0] - 0x30) * 10) + (alram[2][1] - 0x30);
				alram_day3 = ((alram[2][2] - 0x30) * 10) + (alram[2][3] - 0x30);
				alram_hour3 = ((alram[2][4] - 0x30) * 10) + (alram[2][5] - 0x30);
				alram_min3 = ((alram[2][6] - 0x30) * 10) + (alram[2][7] - 0x30);
				alram_sec3 = ((alram[2][8] - 0x30) * 10) + (alram[2][8] - 0x30);
			}
		}
	}

}



void main(void)
{
	init_timer();                      //�ʱ� Ÿ�̸� ���� 
	LCD_Init();                        //lcd��� ���� 
	delay_ms(50);
	init_reg();                        //���� ��Ʈ ���� 

	mode = sec = min = 0;              //�ʱⰪ ���� 
	month = 12;
	day = 5;
	hour = 12;
	year = 2019;
	dir = 0;

	LCD_Pos(0, 1);                      //�ʱ�ȭ�� ��� 
	LCD_Str(first);
	LCD_Pos(1, 1);
	LCD_Str(second);
	delay_ms(50);

	while (1)
	{

		key_Num = KeyScan();       //Ű�� �ް� ��忡 ���� ���� 
		mode = key_Num;

		if (mode == 4) //m1 -> time_setting 
		{
			timer_flag = 1;
			LCD_Clear();
			delay_ms(10);
			LCD_Pos(0, 1);
			LCD_Str(str);
			delay_ms(2000);
			LCD_Clear();
			delay_ms(10);
			LCD_Str(str2);
			delay_ms(2000);
			LCD_Clear();
			delay_ms(10);
			mode = 0;
			time_set();
		}
		if (mode == 8) //m2 -> alram set mode
		{
			LCD_Clear();
			delay_ms(10);
			LCD_Str(str3);
			delay_ms(2000);
			LCD_Clear();
			LCD_Str(str4);
			delay_ms(2000);
			LCD_Clear();
			delay_ms(10);
			mode = 0;
			alram_set();
		}

		else if (mode == 0)
		{
			flag_chattering = 0;
		}
		else
		{
			if (!flag_chattering)
			{
				flag_chattering = 1;
				if (mode == 12)        //m3 -> am/pm���� (m3��ư �ߺ������� �����ϴ� ä�͸� �ڵ� �߰�) 
				{
					dir = ~dir;
				}
			}
		}


		LCD_Pos(0, 5);
		LCD_Char('-');
		delay_ms(50);

		LCD_Pos(0, 8);
		LCD_Char('-');
		delay_ms(50);

		LCD_Pos(1, 6);
		LCD_Char(':');
		delay_ms(50);

		LCD_Pos(1, 9);
		LCD_Char(':');
		delay_ms(50);
		if (dir == 1)                          //m3�� ������ �� am/pm��ȯ �ڵ� 
		{
			if (hour <= 12)                    //am -> pm �϶� am �� ����� 12�ð��� ���Ѵ� 
			{                                 //ex) am 11:00 -> "  "23:00
				LCD_Pos(1, 1);
				LCD_Str("  ");
				LCD_Pos(1, 4);
				LCD_Char(((hour + 12) / 10) + '0');
				LCD_Char(((hour + 12) % 10) + '0');
			}
			else if (hour > 12)
			{
				LCD_Pos(1, 1);
				LCD_Str(AM);
				LCD_Pos(1, 4);
				LCD_Char(((hour - 12) / 10) + '0');
				LCD_Char(((hour - 12) % 10) + '0');
			}
		}

		else if (dir == 0)                     //�ʱ���¿��� am/pm�����ϱ� ���� �ڵ� 
		{                                     //hour �� 23�� ������ -> pm 11�÷� �ٲٱ� ���� 
			if (hour > 12)
			{
				LCD_Pos(1, 1);
				LCD_Str(PM);
				LCD_Pos(1, 4);
				LCD_Char(((hour - 12) / 10) + '0');
				LCD_Char(((hour - 12) % 10) + '0');
			}
			else
			{
				LCD_Pos(1, 1);
				LCD_Str(AM);
				LCD_Pos(1, 4);
				LCD_Char((hour / 10) + '0');
				LCD_Char((hour % 10) + '0');
			}
		}
		else;

		LCD_Pos(0, 1);
		LCD_Char((year / 1000) + '0');                    //��� �Լ� 
		LCD_Char(((year % 1000) - (year % 100)) / 100 + '0');
		LCD_Char(((year % 100) - (year % 10)) / 10 + '0');
		LCD_Char((year % 10) + '0');

		LCD_Pos(0, 6);
		LCD_Char(month / 10 + '0');
		LCD_Char(month % 10 + '0');

		LCD_Pos(0, 9);
		LCD_Char(day / 10 + '0');
		LCD_Char(day % 10 + '0');

		LCD_Pos(1, 7);
		LCD_Char((min / 10) + '0');
		LCD_Char((min % 10) + '0');
		LCD_Pos(1, 10);
		LCD_Char((sec / 10) + '0');
		LCD_Char((sec % 10) + '0');
		delay_ms(50);

		if ((alram_month1 == month) && (alram_day1 == day) && (alram_hour1 == hour) && (alram_min1 == min))// && (alram_sec1 == sec))
		{
			alram_month1 = alram_day1 = alram_hour1 = alram_min1 = 0;    //�д������� ���ؼ� ���� �� ���� + ȭ����� 
			LCD_Clear();
			buzzer_flag = 1;
			while (buzzer_flag == 1)
			{
				LCD_Pos(0, 0);
				LCD_Str("!!!!Stand Up!!!!");
				LCD_Pos(1, 0);
				LCD_Str("press  PIND.0!!");
				delay_ms(10);

				SSound(Do);
				SSound(Mi);
			}
		}

		if ((alram_month2 == month) && (alram_day2 == day) && (alram_hour2 == hour) && (alram_min2 == min))
		{
			alram_month2 = alram_day2 = alram_hour2 = alram_min2 = 0;     //�д������� ���ؼ� ���� �� ���� + ȭ����� 
			buzzer_flag = 1;
			while (buzzer_flag == 1)
			{
				LCD_Pos(0, 0);
				LCD_Str("!!!!Stand Up!!!!");
				LCD_Pos(1, 0);
				LCD_Str("press  PIND.0!!");
				delay_ms(10);

				SSound(Do);
				SSound(Mi);
				SSound(Ra);
			}
		}
		if ((alram_month3 == month) && (alram_day3 == day) && (alram_hour3 == hour) && (alram_min3 == min))
		{
			alram_month3 = alram_day3 = alram_hour3 = alram_min3 = 0;      //�д������� ���ؼ� ���� �� ���� + ȭ����� 
			buzzer_flag = 1;
			while (buzzer_flag == 1)
			{
				LCD_Pos(0, 0);
				LCD_Str("!!!!Stand Up!!!!");
				LCD_Pos(1, 0);
				LCD_Str("press  PIND.0!!");
				delay_ms(10);

				SSound(Do);
				SSound(Mi);
				SSound(Ra);
				SSound(Si);
			}
		}
	}
}
