#include<stdarg.h>
 
void print_int(int argint);
void print_float(double argflt);
void print_small_radix(int argint,int n);
void print_str(char *argstr);
void print_capital_radix(int argint,int n);

extern int shell_fputc(int ch);

int putchar(int ch)
{
    return shell_fputc(ch);
}

int printf(const char *format,...)
{
    char ch;
    int argint;
    float argflt;
    char *argstr;
    va_list va;                 //声明一个va_list类型的指向形参的指针va
    va_start(va,format);        //初始化va，使其指向第一个可变参数

    while((ch = *format++) != '\0') //将format指向的值赋给ch，然后format指向下一个字符
    {
        if(ch != '%')           //未遇到%之前都直接输出
        {
            putchar(ch);
            continue;
        }

        //format指向的字符是\0就返回\0，不是的话返回这个字符并让format指向下一个字符
        switch(*format != '\0'?*format++:'\0')
        {
            case '\0':
            {
                putchar('%');
                break;
            }
            case 'd':
            {
                argint = va_arg(va,int);
                print_int(argint);
                break;
            }
            case 'l':
            {
                if(*format == 'f')  //如果%l后是f就执行打印浮点型
                {
                    argflt = va_arg(va,double);
                    print_float(argflt);
                    format ++;
                }
                else                //%l后加不同字母有很多种输出，这里是简易版printf不详细写了
                {
                    putchar('%');
                }
                break;
            }
            case 'f':
            {
                argflt = va_arg(va,double);
                print_float(argflt);
                break;
            }
            case 'c':
            {
                putchar(va_arg(va,int));
                break;
            }
            case 's':
            {
                argstr = va_arg(va,char *);
                print_str(argstr);
                break;
            }
            case 'o':       //整型数字以八进制形式输出
            {
                argint = va_arg(va,int);
                print_small_radix(argint,8);
                break;
            }
            case 'x':       //整型数字以十六进制形式输出
            {
                argint = va_arg(va,int);
                print_small_radix(argint,16);
                break;
            }
            case 'X':       //整型数字以十六进制形式输出
            {
                argint = va_arg(va,int);
                print_capital_radix(argint,16);
                break;
            }
            case '%':       //发现连续%%时先打印第一个%
            {

            }
            default :
            {
                putchar('%');
                format --;  //将指针移回第一个%后面，进行下一轮判断
                break;
            }
        }
    }
    va_end(va);             //结束可变参数的获取
    return 0;
}

/*此函数功能为输出十进制整数。用递归和取余的方式依次打印最高到最低*/
void print_int(int argint)  //以入参整数123举例，执行顺序为1,2,3,4...
{
    int value = argint / 10;    //1.value为12 3.value为1 5.value为0，argint为1
    if(value != 0)
    {
        print_int(value);   //2.value为12做参数调用自身函数 4.value为1做参数调用自身函数
    }
    value = argint % 10;    //6.对argint取余赋给value
    int temp = value + '0'; //7.将整型数字转成字符型数字
    putchar(temp);
}

void print_float(double argflt)
{
    int i;
    int num;
    char ptr[7];
    ptr[6] = '\0';
    print_int(argflt);
    putchar('.');

    long int n = argflt * 1000000;
    n = n % 1000000;
    for(i = 5;i > -1;i--)
    {
        num = n % 10;
        n = n / 10;
        ptr[i] = num + '0';
    }
    print_str(ptr);
}

void print_str(char *argstr)
{
    while(*argstr != '\0')
    {
        putchar(*argstr);
        argstr ++;
    }
}

void print_small_radix(int argint,int n)
{
    int temp[100];
    int i = 0;
    while(argint > 0)
    {
        temp[i] = (argint % n) > 9?argint % n - 10 + 'a':argint % n + '0';
        argint = argint / n;
        i ++;
    }
    for(i = i - 1;i > -1;i --)
    {
        putchar(temp[i]);
    }
}

void print_capital_radix(int argint,int n)
{
    int temp[100];
    int i = 0;
    while(argint > 0)
    {
        temp[i] = (argint % n) > 9?argint % n - 10 + 'A':argint % n + '0';
        argint = argint / n;
        i ++;
    }
    for(i = i - 1;i > -1;i --)
    {
        putchar(temp[i]);
    }
}
