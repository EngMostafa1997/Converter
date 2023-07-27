#include "StdTypes.h"
#include "LCD_Interface.h"
#include "Utils.h"
#include "KeyPad_Interface.h"
#include "CONVERTER_Interface.h"
#include "CONVERTER_Private.h"

u8 mode_f=NO_KEY,mode_t=NO_KEY;

void mode_select (void)
{
	c8 choices[3]={'D','B','H'};
	LCD_WriteString("Convert From ");
	LCD_WriteStringGoto(1,0,"1-D   2-B   3-H");
	checkpoint1:
	mode_f=KEYPAD_GetKey();
	if (mode_f=='1' || mode_f=='2' || mode_f=='3')
	{
		LCD_Clear();
		LCD_WriteString("From ");
		LCD_WriteChar(choices[(mode_f-'0')-1]);
		LCD_WriteString(" To ");
		LCD_WriteStringGoto(1,0,"1-D   2-B   3-H");
		checkpoint2:
		mode_t=KEYPAD_GetKey();
		if ((mode_t=='1' || mode_t=='2' || mode_t=='3') && mode_t!=mode_f)
		{
			LCD_WriteStringGoto(1,0,"                ");
			LCD_GoTo(0,10);
			LCD_WriteChar(choices[(mode_t-'0')-1]);
		}
		else if (mode_t =='C')
		{
			LCD_Clear();
			LCD_WriteString("Convert From ");
			LCD_WriteStringGoto(1,0,"1-D   2-B   3-H");
			goto checkpoint1;
		}
		else
		{
			goto checkpoint2;
		}
	}
	else
	{
		goto checkpoint1;
	}
}

static void convert_from_decimal (void)
{
	u8 k=NO_KEY;
	u32 num=0;
	LCD_GoTo(1,0);
	k=KEYPAD_GetKey();
	while(k!='=')
	{
		if (k>='0' && k<='9')
		{
			num=num*10+(k-'0');
			LCD_WriteChar(k);
		}
		k=KEYPAD_GetKey();
	}
	if (k=='=')
	{
		if (mode_t=='2')
		{
			LCD_Clear();
			LCD_WriteNumber(num);
			LCD_GoTo(1,0);
			LCD_WriteBinary(num);
		}
		else if (mode_t=='3')
		{
			LCD_Clear();
			LCD_WriteNumber(num);
			LCD_GoTo(1,0);
			LCD_WriteHex(num);
		}
	}
}

static void convert_from_binary (void)
{
	u8 k;
	s8 bits;
	u16 num=0;
	LCD_Clear();
	LCD_WriteString("1-u8      2-u16");
	checkpoint1:
	k=KEYPAD_GetKey();
	if (k=='1')
	{
		bits=7;
	}
	else if (k=='2')
	{
		bits=15;
	}
	else
	{
		goto checkpoint1;
	}
	LCD_Clear();
	while(bits>=0 && k!='=')
	{
		k=KEYPAD_GetKey();
		if (k=='0' || k=='1')
		{
			LCD_WriteChar(k);
			if (k=='1')
			{
				SET_BIT(num,bits);
			}
			bits--;
		}
	}
	if (mode_t=='1')
	{
		LCD_GoTo(1,0);
		LCD_WriteNumber(num);
	}
	else if (mode_t=='3')
	{
		LCD_GoTo(1,0);
		LCD_WriteHex(num);
	}
}

static void convert_from_hex (void)
{
	LCD_Clear();
	LCD_WriteString("Use + For A To F");
	LCD_GoTo(1,0);
	LCD_WriteString("Use (-) Confirm");
	u8 k=NO_KEY,n_digits=0,temp=0,index=0;
	u8 digits[]={'A','B','C','D','E','F'};
	u32 num=0;
	
	while(k!='='  && n_digits<8)
	{
		k=KEYPAD_GetKey();
		if (k!=NO_KEY)
		{
			if (n_digits==0)
			{
				LCD_Clear();
			}
			if (k>='0' && k<='9')
			{
				LCD_WriteChar(k);
				temp=k-'0';
				num<<=4;
				num=num|temp;
				n_digits++;
			}
			else if (k=='+')
			{
				LCD_GoTo(0,n_digits);
				LCD_WriteChar(digits[index]);
				temp=index+10;
				index++;
				while (k!='-')
				{
					k=KEYPAD_GetKey();
					if (k=='+')
					{
						LCD_GoTo(0,n_digits);
						LCD_WriteChar(digits[index]);
						temp=index+10;
						index++;
						if (index>5)
						{
							index=0;
						}
					}
				}
				num<<=4;
				num=num|temp;
				n_digits++;
				index=0;
			}
		}
	}
	if (mode_t=='1')
	{
		LCD_GoTo(1,0);
		LCD_WriteNumber(num);
	}
	else if (mode_t=='2')
	{
		LCD_GoTo(1,0);
		LCD_WriteBinary(num);
	}
}

void convert (void)
{
	
	if (mode_f=='1')
	{
		convert_from_decimal();
	}
	else if (mode_f=='2')
	{
		convert_from_binary();
	}
	else if (mode_f=='3')
	{
		convert_from_hex();
	}
}