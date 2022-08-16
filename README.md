# M480BSP_LZ4
 M480BSP_LZ4

update @ 2022/08/16

1. porting LZ4 library from below ,   sample code : examples/simple_buffer.c

https://github.com/lz4

2. KEIL setting , pre define : LZ4_DISABLE_DEPRECATE_WARNINGS , LZ4_HEAPMODE = 1,

![image](https://github.com/released/M480BSP_LZ4/blob/main/keil_option_preprocessor_define.jpg)	

3. modify heap size for the library

![image](https://github.com/released/M480BSP_LZ4/blob/main/LZ4_HEAPMODE.jpg)	

4. 4 demos , 1 with ram data , 3 with const data in flash , to compress and decompress data

5. below is enable print buffer log message , 

![image](https://github.com/released/M480BSP_LZ4/blob/main/demo1.jpg)	

![image](https://github.com/released/M480BSP_LZ4/blob/main/demo2.jpg)	

6. below is log message about the demo , 

![image](https://github.com/released/M480BSP_LZ4/blob/main/LOG1.jpg)	

![image](https://github.com/released/M480BSP_LZ4/blob/main/LOG2.jpg)	

