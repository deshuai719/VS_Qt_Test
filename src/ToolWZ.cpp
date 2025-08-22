//进制转换以及累加校验和计算
//

#include "ToolWZ.hpp"
#include <vcruntime_string.h>

/*
	十进制数存储于dec地址中，高位存于低地址，低位存于高地址，商存于quotient中，存储方式同dec, 余数存于hex中，低位存低地址，高位存高地址

    使用求余法将十进制转为16进制，每次除法的余数即为16进制的一位，余数存储于hex中，返回值为hex中存储的16进制位数
*/

unsigned long long TOOLWZ::DecToHexBigInteger(unsigned char* dec, unsigned long long len, unsigned char* hex)
{
	unsigned char quotient[800] = { 0 };

	memcpy(quotient, dec, len);
	size_t dividend(0), nonzero_index(0), remainder_index(0);
	while (nonzero_index < len)
	{
		for (int index = nonzero_index; index < len; index++)//除法过程模拟
		{
			dividend = dividend * 10 + quotient[index];
			quotient[index] = dividend / 16;
			dividend = dividend % 16;
		}
		hex[remainder_index] = dividend;//记录本次除法的余数
		remainder_index++;
		dividend = 0;
		while (nonzero_index < len && quotient[nonzero_index] == 0)nonzero_index++;
	}
	return remainder_index;
}

unsigned char TOOLWZ::CharToHex(unsigned char c)
{
	if (c >= '0' && c <= '9')return c - '0';
	if (c >= 'A' && c <= 'F')return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')return c - 'a' + 10;
	return 0;
}

uint8_t TOOLWZ::AccVerify(char* p, int n)
{
	uint32_t sum(0);
	for (int index = 0; index < n; index++)
	{
		sum += (uint8_t)(p[index]);
	}
	sum = RIGHT_SHIFT_U8(sum, 0) + RIGHT_SHIFT_U8(sum, 8) + RIGHT_SHIFT_U8(sum, 16) + RIGHT_SHIFT_U8(sum, 24);
	sum = RIGHT_SHIFT_U8(sum, 0) + RIGHT_SHIFT_U8(sum, 8) + RIGHT_SHIFT_U8(sum, 16) + RIGHT_SHIFT_U8(sum, 24);
	sum = NEGATION_BITS(sum);
	p = 0;
	return RIGHT_SHIFT_U8(sum, 0);
}