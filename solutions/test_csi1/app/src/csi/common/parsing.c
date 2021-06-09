#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


void cmd_parsing(void *cml, void *mc, void *args)
{
	char *p;
	char *at_cml = NULL;

	p = strtok((char *)cml, "=");
	if (p != NULL)
	{
		at_cml = p;
		p = strtok(NULL, "=");
		if (p != NULL)
		{
			strcpy((char *)args, p);
		}
		else
		{
			*(char *)args = '\0';
		}
	}

	p = strtok(at_cml, "+");
	if (p != NULL)
	{
		p = strtok(NULL, "+");
		if (p != NULL)
		{
			strcpy((char *)mc, p);
		}
		else
		{
			*(char *)mc = '\0';
		}
	}
}

static uint32_t str_to_ui(char *str)
{
	char *inputString = NULL;

	uint8_t base = 10, index = 0;
	uint32_t number = 0;

	inputString = str;
	if ((inputString[0] == '0') && (inputString[1] == 'x'))
	{
		base = 16;
		index = 2;
	}

	while (inputString[index] != '\0')
	{
		if (base == 10)
		{
			if (inputString[index] >= '0' && inputString[index] <= '9')
			{
				number = number * 10 + inputString[index] - '0';
			}
			else
			{
				break;
			}
		}
		if (base == 16)
		{
			if (inputString[index] >= '0' && inputString[index] <= '9')
			{
				number = number * 16 + inputString[index] - '0';
			}
			else if (inputString[index] >= 'A' && inputString[index] <= 'F')
			{
				number = number * 16 + inputString[index] - 'A' + 10;
			}
			else if ((inputString[index] >= 'a' && inputString[index] <= 'f'))
			{
				number = number * 16 + inputString[index] - 'a' + 10;
			}
			else
			{
				break;
			}
		}
		index++;
	}

	return number;
}

int args_parsing(void *args, uint32_t *value, uint8_t num)
{
	char *p;
	int ret_num;
	uint8_t i;

	if ((args == NULL) || (value == NULL))
	{
		return -1;
	}

	if (num == 0)
	{
		return 0;
	}

	ret_num = 0;
	i = 0;
	p = strtok((char *)args, ",");
	do
	{
		if (p != NULL)
		{
			*(value + i) = str_to_ui(p);
		}
		else
		{
			ret_num = -1;
			break;
		}
		p = strtok(NULL, ",");
		i++;
	} while (i < num);

	return ret_num;
}

void transfer_data(char *addr, uint32_t size)
{
    uint32_t i;

    for (i=0; i<size; i++) {
        *(addr+i) = i%256;
    }
}

void dataset(uint8_t *addr, uint32_t size, uint8_t pattern){
    uint32_t i;
    for (i=0; i<size; i++) {
        *(addr+i) = pattern;
    }
}
