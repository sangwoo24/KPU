#include <mega128.h>
#include <delay.h>
#include "C:\Users\���\Desktop\vhd\header\lcd2.h"

#define FND_Null        17
#define FND_Star        14
#define FND_Sharp       15

#define     Do      1908
#define     Re      1700
#define     Mi       1515
#define     Pa       1432
#define     Sol      1275
#define     La      1136
#define     Si      1012

unsigned char mode, i, key_set, key_set_num, key_input, key_input_num, wrong_count, key_admin, admin_num, key_lock, key_lock_num, count;
unsigned char password_set[4] = { 0,0,0,0 };    //�ʱ� ���� �迭 ���� 
unsigned char password_in[4];    //��й�ȣ �Է� �迭
unsigned char admin[10];    //������ ��й�ȣ ���� �迭
unsigned char lock[10];     //������ ��й�ȣ �Է� �迭 
bit open_flag = 0;          //�ʱ�ȭ�鿡�� open/close �� ǥ���ϱ� ���� flag
bit setting_flag = 0;       //  { ������ �ϰ� ��й�ȣ�� �Է��ϴ� ������ �̷������ �ݰ� �ý����� �����ϰ� �ϱ�����
bit input_flag = 0;         //    flag ����. �Ѵ� 1�϶��� �´���/Ʋ���� Ȯ�ΰ��� } 
bit lock_flag = 0;          //������� �� ������ �ڵ� �Է��ϴ� ������ ���������� ���ϰ� �ϱ����� flag
bit delete_flag = 0;        //�ѱ��� �� �����ϱ� ���� flag
bit dir;                    //4/10�ڸ��� �ʰ��ϸ� ���̻� �Է� ���� ���ϰ� �ϱ����� flag
bit escape_flag;            //��й�ȣ�� �� ����/�Է� �� �� ���� ������ ��ư�� ���� main���� �������� flag 
bit flag_chattering;        //ä�͸��� �����ϱ� ���� �÷��� ���� 


void init_reg(void)
{
	DDRB = 0xff;
	DDRC = 0x0f;            // ���� 4bit Col(�Է�), ���� 4bit Row(���)
	PORTC = 0x0f;           // ����4��Ʈ�� ���� 
	DDRG |= (1 << 4);         // ���� ��밡��        
	SREG |= 0x80;
}

void myDelay_us(unsigned int delay)     //������ �������� �������Լ� 
{
	int i;
	for (i = 0; i < delay; i++)
	{
		delay_us(1);
	}
}

void SSound(int time, int lng)          //���� ����Լ� 
{
	int i;
	for (i = 0; i < lng; i++) // ���踶�� ���� �ð� ���� �︮���� time ���� ���
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
void password_setting(void)                //��й�ȣ ���� �Լ� 
{
	LCD_Clear();                           //���� �Լ��� ������ �ʱ⿡ LCD�� ����ϴ� ȭ�鼳�� 
	delay_ms(30);
	LCD_Pos(0, 0);
	LCD_Str("Password setting");
	delay_ms(1500);
	LCD_Pos(0, 0);
	LCD_Str("Initial Password");
	LCD_Pos(1, 7);
	LCD_Char(':');
	i = 0;

	while (1)
	{
		key_set = KeyScan();                //Ű�� �ް� 

		if (key_set == 0)                 //ä�͸� ���� �ڵ� 
		{
			flag_chattering = 0;
		}

		else
		{
			if (!flag_chattering)      //ä�͸� ���� �ڵ� 
			{
				flag_chattering = 1;

				if (key_set)                        //Ű�� ������ 
				{
					SSound(Si, 20);                 //Ű ���� �� ���� �︮�� ���� 
					if (key_set == 12)              //m3��ư�� ������ �ʱ���·� ���ư���[main] 
					{
						LCD_Clear();
						delay_ms(30);
						break;
					}
					else if (key_set == 15)         //#�� ������ �ѱ��ھ� �����ȴ� 
					{
						delete_flag = 1;          //�����ϱ� ���� �÷��� ���� 
					}
					else if (key_set == 4)         //4�ڸ� �Է��� main �����ϴ� ��ư     
					{
						escape_flag = 1;
					}


					if (key_set == 14)
					{
						key_set_num = 0;
					}
					else if (key_set % 4 != 0)       // 3x4 Ű�е� ����� ���� ���� 
					{
						key_set_num = (key_set / 4) * 3 + (key_set % 4);
					}
					else
					{
						key_set_num = (key_set / 4) + 9;
					}

					if (delete_flag == 0)
					{
						if (dir == 0)     //4�ڸ� �Ѱ� �Է��������ϰ� �ϴ� flag
						{
							password_set[i] = (key_set_num + 0x30);    //passwore_set�̶�� �迭�� ���� �Է��� ���ڵ��� �ְ� LCD�� ��� 
							LCD_Char(password_set[i]);
							delay_ms(300);
							i++;
						}
						else;

						if (escape_flag == 1)      //4�ڸ� ���� �Ϸ� �� Ż���ڵ� 
						{
							LCD_Clear();
							delay_ms(30);
							LCD_Pos(0, 0);
							LCD_Str("Setting Complite");           //��й�ȣ 4�ڸ��� �� �����ÿ� LCD�� ��µǴ� ȭ��
							delay_ms(1000);

							for (i = 0; i < 3; i++)
							{
								SSound(Si, 30);                     //���� �� �ϸ� ���� �︮�� ���� 
								SSound(Sol, 30);
							}
							LCD_Clear();
							delay_ms(30);

							setting_flag = 1;                      //�ٸ� �������� �������� flag���� 
							open_flag = 0;
							wrong_count = 3;
							escape_flag = 0;
							dir = 0;
							break;
						}


						if (i == 4)                                //4�ڸ� �Է� �� 
						{
							dir = 1;
						}
					}
					else if (delete_flag == 1)                      //#�� ������ �� flag�� 1�̵ȴ� -> �����ϱ� ���� �ڵ� 
					{
						if (i >= 1 && i <= 4)                       //4�ڸ��� �Է��� �� �ٷ� ���ι����� ������ ������ 3�ڸ����� �Է��� �޾����� ������ �� �ִ�. 
						{                                          //3�ڸ� ������ ���� �ڵ� 
							dir = 0;
							LCD_Pos(1, i + 7);
							LCD_Char(' ');
							LCD_Pos(1, i + 7);
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
void password_input(void)                            //��й�ȣ �Է� �Լ� 
{
	LCD_Clear();
	delay_ms(30);
	LCD_Pos(0, 0);
	LCD_Str("Press Password");                      //��й�ȣ �Է� �Լ��� ���� �� LCD�� ��µǴ� ȭ�� 
	LCD_Pos(1, 6);
	LCD_Char(':');
	i = 0;

	LCD_Pos(1, 7);
	while (1)
	{
		key_input = KeyScan();           //Ű�� �ް� 

		if (key_input == 0)                 //ä�͸� ���� �ڵ� 
		{
			flag_chattering = 0;
		}
		else
		{
			if (!flag_chattering)         //ä�͸� ���� �ڵ� 
			{
				flag_chattering = 1;

				if (key_input)
				{
					SSound(Si, 20);       //Ű ������ ���� �︮�� 

					if (key_input == 12)
					{
						LCD_Clear();
						delay_ms(30);
						break;
					}
					else if (key_input == 15)               //#Ű�� ������ ���� ��Ű�� ���� flag ���� 
					{
						delete_flag = 1;
					}
					else if (key_input == 8)
					{
						escape_flag = 1;
					}


					if (key_input == 14)
					{
						key_input_num = 0;
					}
					else if (key_input % 4 != 0)
					{
						key_input_num = (key_input / 4) * 3 + (key_input % 4);
					}
					else
					{
						key_input_num = (key_input / 4) + 9;
					}

					if (delete_flag == 0)
					{
						if (dir == 0)
						{
							password_in[i] = (key_input_num + 0x30);           //�Է��� ���ڸ� �迭�� �ְ� ����Ѵ�
							LCD_Char('*');                                     //��й�ȣ�� ���̸� �ȵǴϱ� *�� ����Ѵ�
							delay_ms(300);
							i++;
						}
						else;

						if (escape_flag == 1)                               //4�ڸ� �� �Է½ÿ� main������ �������´� 
						{
							LCD_Clear();
							delay_ms(30);

							input_flag = 1;
							escape_flag = 0;
							dir = 0;
							break;
						}
						if (i == 4) dir = 1;                                //4�ڸ� �̻� �Է� x
					}
					else if (delete_flag == 1)                              //4�ڸ��� �Է��� �� �ٷ� ���ι����� ������ ������ 3�ڸ����� �Է��� �޾����� ������ �� �ִ�. 
					{                                                      //3�ڸ� ������ ���� �ڵ�
						if (i >= 1 && i <= 4)
						{
							LCD_Pos(1, i + 6);
							LCD_Char(' ');
							LCD_Pos(1, i + 6);
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
void admin_code(void)                           //������ �ڵ� ���� �Լ� 
{
	LCD_Clear();
	delay_ms(30);
	LCD_Pos(0, 0);
	LCD_Str("Admin Setting..!");                //�Լ��� ������ �ʱ�ȭ�鿡 ��µǴ� LCD ����  
	LCD_Pos(1, 0);
	LCD_Char(':');
	delay_ms(300);
	i = 0;

	while (1)
	{
		key_admin = KeyScan();

		if (key_admin == 0)                 //ä�͸� ���� �ڵ� 
		{
			flag_chattering = 0;
		}
		else
		{
			if (!flag_chattering)      //ä�͸� ���� �ڵ� 
			{
				flag_chattering = 1;

				if (key_admin)
				{
					SSound(Si, 20);         //Ű ������ ���� �︮�� 
					if (key_admin == 12)
					{
						LCD_Clear();
						delay_ms(30);
						break;
					}
					else if (key_admin == 16)
					{
						escape_flag = 1;
					}
					else if (key_admin == 15)           //#Ű�� ���� �� �����ϱ����� flag���� 
					{
						delete_flag = 1;
					}
					else if (key_admin == 16)          //���� �Ϸ� �� M4������ Ż�� 
					{
						escape_flag = 1;
					}


					if (key_admin == 14)
					{
						admin_num = 0;
					}
					else if (key_admin % 4 != 0)
					{
						admin_num = (key_admin / 4) * 3 + (key_admin % 4);
					}
					else
					{
						admin_num = (key_admin / 4) + 9;
					}

					if (delete_flag == 0)
					{
						if (dir == 0)
						{
							admin[i] = (admin_num + 0x30);               //�Է��� ���ڸ� �迭�� �ְ� ����ϴ� ���� 
							LCD_Char(admin[i]);
							delay_ms(300);
							i++;
						}
						else;

						if (escape_flag == 1)                             //10�ڸ� ���� �Է½� main������ 
						{
							LCD_Clear();
							delay_ms(30);
							LCD_Pos(0, 0);
							LCD_Str("Setting Complite");           //��й�ȣ 4�ڸ��� �� �����ÿ� LCD�� ��µǴ� ȭ��
							delay_ms(1000);

							for (i = 0; i < 3; i++)                       //���� �Ϸ� �� ���� �︮�� 
							{
								SSound(Si, 30);
								SSound(Sol, 30);
							}
							LCD_Clear();
							delay_ms(30);
							escape_flag = 0;
							dir = 0;
							break;
						}                                                //10�ڸ� �̻� �Է� X
						if (i == 10)
						{
							dir = 1;
						}
					}
					else if (delete_flag == 1)                      //#������ �� �����ϱ� ���� �ڵ� (10�ڸ�) 
					{
						if (i >= 1)                                 //pos���� ��� �ٲ��ֱ� ���� ���� ���Ǵ� ���� i�� ��� 
						{
							dir = 0;
							LCD_Pos(1, i);
							LCD_Char(' ');
							LCD_Pos(1, i);
							delay_ms(200);
							delete_flag = 0;
							i--;
						}
						else  delete_flag = 0;   //�����ؼ� ó������ ���ư� �� �ٽ� �Է��� �ޱ�����       
					}
				}
			}

		}
	}
}
void lock_release(void)                               //������ �ڵ� �Է��Լ� 
{
	LCD_Clear();
	delay_ms(30);
	LCD_Pos(0, 0);
	LCD_Str("Press Code!!!");                         //�ڵ� �Է� �Լ��� ������ �� lcd��� ���� 
	LCD_Pos(1, 0);
	LCD_Char(':');
	i = 0;

	while (1)
	{
		key_lock = KeyScan();

		if (key_lock == 0)                 //ä�͸� ���� �ڵ� 
		{
			flag_chattering = 0;
		}
		else
		{
			if (!flag_chattering)      //ä�͸� ���� �ڵ� 
			{
				flag_chattering = 1;

				if (key_lock)
				{
					SSound(Si, 20);                     //Ű ������ �� ���� �︮�� ���� 
					if (key_lock == 15)                 //#�� ������ �� ���� ���� 
					{
						delete_flag = 1;
					}
					else if (key_lock == 16)            //10�ڸ� �Է� �Ϸ� �� M4���� �� Ż�� 
					{
						escape_flag = 1;
					}

					if (key_lock == 14)
					{
						key_lock_num = 0;
					}
					else if (key_lock % 4 != 0)
					{
						key_lock_num = (key_lock / 4) * 3 + (key_lock % 4);
					}
					else
					{
						key_lock_num = (key_lock / 4) + 9;
					}

					if (delete_flag == 0)                   //10�ڸ� �Է��ϱ� ���� �迭�� Ű ������ �ְ� ��� 
					{
						if (dir == 0)                       //10�ڸ� ������ �Է� �ȹޱ� ���� FLAG
						{
							lock[i] = (key_lock_num + 0x30);
							LCD_Char(lock[i]);
							delay_ms(300);
							i++;
						}
						else;

						if (escape_flag == 1)               //Ż���ư ������ 
						{
							LCD_Clear();
							delay_ms(30);
							escape_flag = 0;
							dir = 0;
							break;
						}                                 //10�ڸ� �̻� �Է� X
						if (i == 10)
						{
							dir = 1;
						}
					}
					else if (delete_flag == 1)             //�ѱ��� ��(10����) �����ϱ� ���� �ڵ� 
					{
						if (i >= 1)
						{
							dir = 0;
							LCD_Pos(1, i);
							LCD_Char(' ');
							LCD_Pos(1, i);
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

void main()
{
	LCD_Init();                              //LCD �ʱ�ȭ
	delay_ms(30);
	init_reg();                             //���� ��Ʈ�� ���� 
	setting_flag = input_flag = count = 0;  //������, flag �ʱ�ȭ 
	wrong_count = 3;                        //Ƚ������ ���� 


	while (1)
	{
		if (lock_flag == 0)                    //������� ������ �ʱ�ȭ������ ���ƿ��� ���ϰ� flag���� 
		{
			LCD_Pos(0, 0);
			LCD_Str("   Kpu Safe...  ");      //�ʱ�ȭ�� 
			delay_ms(30);
			LCD_Pos(0, 14);
			LCD_Pos(1, 13);
			LCD_Char('[');
			LCD_Char(wrong_count + 0x30);     //Ʋ��Ƚ�� lcd�� ǥ���ϱ����� ���� 
			LCD_Char(']');
			delay_ms(50);


			if (open_flag == 1)                   //ȭ�鿡 ������� (����,����)�� ǥ���ϱ� ���ؼ� flag���� 
			{
				LCD_Pos(1, 6);
				LCD_Str("[Close]");
				delay_ms(30);
			}
			else if (open_flag == 0)
			{
				LCD_Pos(1, 7);
				LCD_Str("[Open]");
				delay_ms(30);
			}
		}
		mode = KeyScan();

		if (mode == 4)                     //m1 : ��й�ȣ ����
		{
			password_setting();
		}
		else if (mode == 16)               //m4 : ������ �ڵ� ���� 
		{
			admin_code();

			delay_ms(30);
			LCD_Pos(0, 0);
			LCD_Str("Setting Complite");
			delay_ms(1500);
			LCD_Clear();
			delay_ms(30);
		}
		else if (mode == 8)                //m2 : ��й�ȣ �Է� 
		{
			password_input();
		}

		//�������� ��й�ȣ �����ϰ� ���� �÷��� ��ȭ 
		if ((setting_flag == 1) && (input_flag == 1))     //���ʿ� ��й�ȣ�� [���� / �Է�] �ؾ߸� ��й�ȣ�� ���Ҽ��ְ� flag����  
		{
			if ((password_set[0] == password_in[0]) && (password_set[1] == password_in[1]) && (password_set[2] == password_in[2]) && (password_set[3] == password_in[3]))
			{
				input_flag = 0;                        //��й�ȣ �������� ��й�ȣ �Է°��� ������ ���� 
				open_flag = 0;
				wrong_count = 3;

				LCD_Clear();
				delay_ms(50);
				LCD_Pos(0, 3);
				LCD_Str("Safe Open!");

				SSound(Do, 100);                      //���� on
				SSound(Mi, 100);
				SSound(Sol, 100);
				delay_ms(2000);
			}
			else
			{
				input_flag = 0;                     //��й�ȣ�� Ʋ���� 
				open_flag = 1;
				wrong_count--;                      //��й�ȣ Ƚ���� ǥ���ϴ� wrong count�� ���δ�

				LCD_Clear();
				delay_ms(50);
				LCD_Pos(0, 0);
				LCD_Str("Wrong Password!!");

				for (i = 0; i < 3; i++)
				{                                  //���� on
					SSound(Do, 70);
					SSound(Si, 70);
				}
				delay_ms(2000);
			}
		}
		if (wrong_count == 0)                      //3���� �� Ʋ���ÿ� 
		{
			LCD_Pos(0, 0);
			LCD_Str("  Safe Lock!!!");
			delay_ms(2000);

			lock_release();    //10�ڸ� �ް�        //�������ڵ带 �Է��Ҽ��ִ� �Լ��� �ҷ��´� 

			for (i = 0; i < 10; i++)
			{
				if (admin[i] == lock[i])          //10�ڸ��� ���ؼ� count���� ������Ų�� 
				{
					count++;
				}
			}

			if (count == 10)    //10�ڸ� �� ������
			{
				lock_flag = 0;                  //safe open ���� main���� ���ư��� ���� flag���� 
				wrong_count = 3;                //�ʱ��� open���¿� wrong count�� 3���θ���� �ʱ�� ���� �����
				open_flag = 0;                  //��й�ȣ ���� �� main������ open�� ǥ�� �ϱ����� flag���� 

				count = 0;
				LCD_Clear();
				delay_ms(30);
				LCD_Pos(0, 3);
				LCD_Str("Safe Open!");

				SSound(Do, 100);
				SSound(Mi, 100);
				SSound(Sol, 100);
				delay_ms(2000);

			}
			else if (count != 10)   //���ڸ��� Ʋ����� �ٽ� �������ڵ带 ĥ���ִ� �Լ��� ���ư������ؼ� flag�� ��������� 
			{
				wrong_count = 0;   //Ƚ���� 0���� ���� �ٽ� 10�ڸ� �Է¹ް� �� 
				count = 0;         //10�ڸ� �� ī��Ʈ �ٽü��� 
				lock_flag = 1;     //lock count �� 1�� ������ �ÿ� �ٽ� 429����[������ ��й�ȣ �Է��Լ�] �� ���´� 

				LCD_Clear();
				delay_ms(50);
				LCD_Pos(0, 0);
				LCD_Str("Wrong Admin-Code");

				for (i = 0; i < 3; i++)
				{
					SSound(Do, 70);
					SSound(Si, 70);
				}
				delay_ms(2000);
				LCD_Clear();
			}
		}
	}
}