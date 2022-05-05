/*
 * @Author: xieyinghui
 * @Date: 2022-04-30 13:41:54
 * @Last Modified by: xieyinghui
 * @Last Modified time: 2022-04-30 14:55:02
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

//#define DEBUG
#define ARRAY_LEN 3413067
unsigned int addr_array[ARRAY_LEN];
unsigned int sector_len_array[ARRAY_LEN];

void input_file_parse(char* filename)
{
    char*        line = NULL;
    size_t       len  = 0;
    ssize_t      read;
    int          count      = 0;
    unsigned int address    = 0;
    static int   flag       = 0;
    unsigned int sector_len = 0;
    FILE*        fp         = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("open file error");
        exit(-1);
    }

    while ((read = getline(&line, &len, fp)) != -1)
    {
        char buf[11];
        memcpy(buf, line, sizeof(buf));
        buf[10] = '\0';
        if (buf[0] == 'W')
        {
            flag = 1;
            for (int i = 0; i < 8; i++)
            {
                if ('0' <= buf[i + 2] && buf[i + 2] <= '9')
                {
                    address = (address << 4) + 0 + buf[i + 2] - '0';
                }
                else if ('A' <= buf[i + 2] && buf[i + 2] <= 'F')
                {
                    address = (address << 4) + 10 + buf[i + 2] - 'A';
                }
                else if ('a' <= buf[i + 2] && buf[i + 2] <= 'f')
                {
                    address = (address << 4) + 10 + buf[i + 2] - 'a';
                }
            }
#ifdef DEBUG
            printf("%s\n", buf);
            printf("%08x\n", address);
#endif
            addr_array[count] = address;
        }
        else if ((buf[0] == 'L') && (flag == 1))
        {
            flag = 0;
            for (int i = 0; i < 8; i++)
            {
                if ('0' <= buf[i + 2] && buf[i + 2] <= '9')
                {
                    sector_len = (sector_len << 4) + 0 + buf[i + 2] - '0';
                }
                else if ('A' <= buf[i + 2] && buf[i + 2] <= 'F')
                {
                    sector_len = (sector_len << 4) + 10 + buf[i + 2] - 'A';
                }
                else if ('a' <= buf[i + 2] && buf[i + 2] <= 'f')
                {
                    sector_len = (sector_len << 4) + 10 + buf[i + 2] - 'a';
                }
            }
#ifdef DEBUG
            printf("%s\n", buf);
            printf("%08x\n", sector_len);
#endif
            sector_len_array[count] = sector_len;
            count++;
        }
    }
    // printf("%d\n", count);
}

void myitoa(int num, char* str, int radix)
{
    int          i = 0;
    int          sum;
    unsigned int num1 = num;  //如果是负数求补码，必须将他的绝对值放在无符号位中在进行求反码
    char         str1[33] = {0};
    if (num < 0)
    {  //求出负数的补码
        num  = -num;
        num1 = ~num;
        num1 += 1;
    }
    if (num == 0)
    {
        str1[i] = '0';

        i++;
    }
    while (num1 != 0)
    {  //进行进制运算
        sum     = num1 % radix;
        str1[i] = (sum > 9) ? (sum - 10) + 'a' : sum + '0';
        num1    = num1 / radix;
        i++;
    }
    i--;
    int j = 0;
    for (i; i >= 0; i--)
    {  //逆序输出
        str[i] = str1[j];
        j++;
    }
}

void femu_write()
{
    for (int i = 0; i < ARRAY_LEN; i++)
    {
        char cmd[200];
        char num_str[20];
        memset(cmd, 0, sizeof(cmd));
        memset(num_str, 0, sizeof(cmd));
        strcat(cmd, "sudo nvme write /dev/nvme0n1 -s ");
        myitoa(addr_array[i] / 8, num_str, 10);
        strcat(cmd, num_str);
        strcat(cmd, " -c ");
        myitoa(sector_len_array[i] / 8, num_str, 10);
        strcat(cmd, num_str);
        strcat(cmd, " -z 40960 -d write.txt");
        printf("%s\n", cmd);
        system(cmd);
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("input arguement error\n");
        return -1;
    }
    input_file_parse(argv[1]);
    femu_write();
    return 0;
}