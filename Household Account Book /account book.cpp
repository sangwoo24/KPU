#include<iostream>
#include<string>
#define SIZE 4
using namespace std;

class date {  //�� �� Ŭ����
protected:
	int month;
	int day;
public:
	date() :month(0), day(0) {};
	date(int m, int d) :month(m), day(d) {};
	void setdate(int m, int d);
	void showdate();

};
void date::setdate(int m, int d) { //�� �� ����
	month = m;
	day = d;
}

void date::showdate() {  //�� �� ���
	cout << month << "��" << day << "��,    ";
}



class firstmoney :public date {                // ����ݾ�
protected:
	int fmoney;
	int cmoney;
public:
	firstmoney() :cmoney(0) {};
	firstmoney(int m, int d, int c) :date(m, d), cmoney(c) {};
	void setmoney(int m, int d, int c);
	void show();
	void showfmoney();
	void inputfmoney();
	friend void output(firstmoney *first); //��¿��� ������� ����
	int getday();
	int getmonth();
	friend void input(firstmoney *first);
};
int firstmoney::getmonth() { //�˻��� �� ����
	return month;
}

int firstmoney::getday() { //�˻��� ��¥ ����

	return day;
}

void firstmoney::inputfmoney() {           //�ʱ�ݾ� ����
	cout.width(44); cout << "�ʱ�ݾ��� �����ּ��� :[   ]��" << endl << endl;
	cin >> fmoney;
}

void firstmoney::showfmoney() {           //�ʱ�ݾ� ���
	cout << "�ʱ�ݾ�" << fmoney << "��" << endl;
}

void firstmoney::setmoney(int m, int d, int c) {  //�� �� ����ݾ� ����
	date::setdate(m, d);
	cmoney = c;

}

void firstmoney::show() {                     // �� �� ����ݾ� ���
	showdate();
	cout << "����ݾ�" << cmoney << "�� " << endl;

}

//�Է�
void input(firstmoney *first) {
	int month, day, cmoney;
	first->inputfmoney();
	cout.width(50); cout << "   [�� ���α׷��� �ϴ� �Է��� 4���� �����Ͽ����ϴ�.]   " << endl << endl << endl;
	cout.width(56); cout << "---------------------------------------------------------" << endl;
	cout.width(55); cout << "1.���Ժθ� �����ֽñ�ٶ��ϴ�. [��] [��] [�ݾ�]��" << endl << endl;
	cout.width(52); cout << "2.�ð��� ��������� ������ ������ �� �� ������" << endl << endl;
	cout.width(54); cout << "�ݵ�� ��¥/�� �� ���������� �����ֽñ�ٶ��ϴ�." << endl << endl;
	cout.width(45); cout << "ex] 5  3  12000  =  5�� 3�� 12000��" << endl;
	for (int i = 0; i < SIZE; i++)
	{
		cout.width(42); cout << endl << "[ ]�� , [ ]��, ����ݾ�[    ]�� " << endl;

		cin >> month >> day >> cmoney;
		if (month > 12, day > 31)
		{
			cout << "�߸� �Է��ϼ̽��ϴ�." << endl << endl;
			cout << "�ʱ�ȭ������ ���ư��ϴ�." << endl << endl; break;


		}

		first[i].setmoney(month, day, cmoney);

	}


}
void Sort(firstmoney *first)// ������ ����
{
	firstmoney fm;

	for (int i = 0; i < SIZE - 1; i++)
	{
		for (int j = 0; j < (SIZE - i - 1); j++)
		{
			if (first[j].getmonth() > first[j + 1].getmonth())
			{
				fm = first[j + 1];
				first[j + 1] = first[j];
				first[j] = fm;
			}
		}

	}
	for (int i = 0; i < SIZE; i++) {
		first[i].show();
	}
}
//���
void output(firstmoney *first) {
	int sum, i;
	first->showfmoney();
	for (i = 0, sum = 0; i < SIZE; i++)
	{
		sum += first[i].cmoney;
		cout << "[" << i + 1 << "]" << endl;
		first[i].show();
		cout << endl << "�ܾ� :" << first->fmoney - sum << endl << endl;

	}

}

void searchday(firstmoney *first)//�˻�[��¥]
{
	int day;


	cout << "�˻��� ��¥ : [ ]�� ";
	cin >> day;

	for (int i = 0; i < SIZE; i++)
	{
		if (first[i].getday() == day)
		{
			first[i].show();

		}
	}
}
void searchmonth(firstmoney *first)//�˻�[��]
{
	int month;

	
	cout << "�˻��� �� : [ ]�� ";
	cin >> month;

	for (int i = 0; i < SIZE; i++)
	{
		if (first[i].getmonth() == month)
		{
			first[i].show();

		}
		else
			cout << "     " << endl;
	}
	
}
void search(firstmoney *first)//�˻�
{
	int a;
	cout.width(54); cout << "��������������������������" << endl;
	cout.width(39); cout << "���⳻�� �˻�����Դϴ�." << endl << endl;
	cout.width(52); cout << "�� �˻������ ��¥�� ���⳻���� ǥ�õ˴ϴ�." << endl << endl;
	cout.width(42); cout << "�ܾ��� �����Ʈ���� Ȯ�ιٶ��ϴ�." << endl << endl;
	cout.width(33); cout << "1. �޺��� �˻��ϱ�" << endl << endl;
	cout.width(33); cout << "2. ���ڷ� �˻��ϱ�" << endl << endl;
	cout.width(31); cout << "3. �κ�� ������" << endl << endl;
	cout.width(54); cout << "��������������������������" << endl;
	cin >> a;
	switch (a)
	{
	case 1:searchmonth(first); break;
	case 2: searchday(first); break;
	case 3: break;
	default:
		cout << endl << "�߸� �Է��ϼ̽��ϴ�." << endl; break;
	}

}

//����
void mainmenu() {

	cout.width(50); cout << "�ڡڡڡڡڡڰ��Ժ� ���α׷��ڡڡڡڡڡ�" << endl << endl;
	cout.width(37); cout << " 1. ���Ժ� ���� " << endl << endl;
	cout.width(37); cout << " 2. ���Ժ� ���� " << endl << endl;
	cout.width(38); cout << " 3. ���⳻�� �˻� " << endl << endl;
	cout.width(37); cout << " 4. ��¥�� ���� " << endl << endl;
	cout.width(34); cout << " 0. �����ϱ�" << endl << endl;
	cout.width(50); cout << "�ڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡ�" << endl << endl;

}


void main() {
	firstmoney *first = new firstmoney[SIZE]; //�迭�� ����ֱ����� new
	int num;
	do {
		mainmenu();
		cin >> num;
		switch (num)
		{
		case 1: input(first); break;
		case 2: output(first); break;
		case 3: search(first); break;
		case 4: Sort(first); break;
		case 0: break;
		default:
			cout << "�߸� �Է��ϼ̽��ϴ�." << endl;
			continue; //�߸��Է��ϸ� ó������ ���ư�.
		}
	} while (num != 0);
	cout << "�����մϴ�" << endl;
}