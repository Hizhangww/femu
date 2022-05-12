## save to .dat file
import struct
import os
import sys

# sudo nvme write /dev/nvme0n1 -s 1738079 -c 0 -z 4096 -d write.txt
def sys_call(line):

    val = os.system(line)
    print(line)


def read_file(file_addr):
    fopen = open(file_addr, 'r')
    lines = fopen.readlines()
    for line in lines:
        sys_call(line)


if __name__ == '__main__':
    for i in range(0,57):
        read_file('Data{}.sh'.format(i))























