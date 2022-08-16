/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "NuMicro.h"

#include "project_config.h"

#include "lz4.h"
/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/
volatile uint32_t BitFlag = 0;
volatile uint32_t counter_tick = 0;

// put under KEIL preprocessor define
// #define LZ4_DISABLE_DEPRECATE_WARNINGS
// #define LZ4_HEAPMODE                        (1)

#define BUFF_SIZE                           (512)

uint8_t srcBuffer1[BUFF_SIZE] = {0};
const char* const srcBuffer2 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Lorem ipsum dolor site amat.";
const char* const srcBuffer3 = 
"I have a really cool job that involves reading books and eating potato chips in massive quantities,\
not unlike a blue whale swimming through a school of krill with its mouth open. IÅfm kidding.\
That first part is totally accurate; the second part is just something I do on my own time.";

const char* const srcBuffer4 = 
"I have a really cool job that involves reading books and eating potato chips in massive quantities,\
not unlike a blue whale swimming through a school of krill with its mouth open. IÅfm kidding.\
That first part is totally accurate; the second part is just something I do on my own time.\
So I get to read a lot of classic novels. And you know what? TheyÅfre not all winners.\
Sure theyÅfre GOOD, as books go, but are they ÅgletÅfs put it on the syllabus for the next two hundred years and never look backÅh good?\
Absolutely not! Very few books are!\
These are, to my mind, the most overrated classic novels.";

uint8_t compressBuffer[BUFF_SIZE*2] = {0};
uint8_t decompressBuffer[BUFF_SIZE*2] = {0};

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

void tick_counter(void)
{
	counter_tick++;
}

uint32_t get_tick(void)
{
	return (counter_tick);
}

void set_tick(uint32_t t)
{
	counter_tick = t;
}

void compare_buffer_REDUCE_LOG(uint8_t *src, uint8_t *des, int nBytes)
{
    uint16_t i = 0;	
	
    for (i = 0; i < nBytes; i++)
    {
        if (src[i] != des[i])
        {
            printf("error idx : %4d : 0x%2X , 0x%2X\r\n", i , src[i],des[i]);
			set_flag(flag_error , ENABLE);
        }
    }

	if (!is_flag_set(flag_error))
	{
		set_flag(flag_error , DISABLE);
	}
}

void compare_buffer(uint8_t *src, uint8_t *des, int nBytes)
{
    uint16_t i = 0;	
	
    for (i = 0; i < nBytes; i++)
    {
        if (src[i] != des[i])
        {
            printf("error idx : %4d : 0x%2X , 0x%2X\r\n", i , src[i],des[i]);
			set_flag(flag_error , ENABLE);
        }
    }

	if (!is_flag_set(flag_error))
	{
    	printf("%s finish \r\n" , __FUNCTION__);	
		set_flag(flag_error , DISABLE);
	}

}

void reset_buffer(void *dest, unsigned int val, unsigned int size)
{
    uint8_t *pu8Dest;
//    unsigned int i;
    
    pu8Dest = (uint8_t *)dest;

	#if 1
	while (size-- > 0)
		*pu8Dest++ = val;
	#else
	memset(pu8Dest, val, size * (sizeof(pu8Dest[0]) ));
	#endif
	
}

void copy_buffer(void *dest, void *src, unsigned int size)
{
    uint8_t *pu8Src, *pu8Dest;
    unsigned int i;
    
    pu8Dest = (uint8_t *)dest;
    pu8Src  = (uint8_t *)src;


	#if 0
	  while (size--)
	    *pu8Dest++ = *pu8Src++;
	#else
    for (i = 0; i < size; i++)
        pu8Dest[i] = pu8Src[i];
	#endif
}

void dump_buffer(uint8_t *pucBuff, int nBytes)
{
    uint16_t i = 0;
    
    printf("dump_buffer : %2d\r\n" , nBytes);    
    for (i = 0 ; i < nBytes ; i++)
    {
        printf("0x%2X," , pucBuff[i]);
        if ((i+1)%8 ==0)
        {
            printf("\r\n");
        }            
    }
    printf("\r\n\r\n");
}

void dump_buffer_hex(uint8_t *pucBuff, int nBytes)
{
    int     nIdx, i;

    nIdx = 0;
    while (nBytes > 0)
    {
        printf("0x%04X  ", nIdx);
        for (i = 0; i < 16; i++)
            printf("%02X ", pucBuff[nIdx + i]);
        printf("  ");
        for (i = 0; i < 16; i++)
        {
            if ((pucBuff[nIdx + i] >= 0x20) && (pucBuff[nIdx + i] < 127))
                printf("%c", pucBuff[nIdx + i]);
            else
                printf(".");
            nBytes--;
        }
        nIdx += 16;
        printf("\n");
    }
    printf("\n");
}

int LZ4_compress_private(const char* source, char* dest, int inputSize, int maxOutputSize, int acceleration)
{
    int result;
    int len;

    LZ4_stream_t* ctxPtr ;   /* malloc-calloc always properly aligned */
    len = sizeof(LZ4_stream_t);    
    ctxPtr = (LZ4_stream_t*)malloc(len);
    printf("malloc1 (0x%2X,%2d)\r\n",len,len);    
    if (ctxPtr == NULL) return 0;
    printf("malloc2 (0x%2X,%2d)\r\n",len,len);
    // while(1);
    result = LZ4_compress_fast_extState(ctxPtr, source, dest, inputSize, maxOutputSize, acceleration);
    printf("compress finish\r\n");

    free(ctxPtr);

    return result;
}

void LZ4_Demo4(void)
{
    float rate = 0.0f;
    uint16_t count = 100;    
    uint16_t i = 0;
    uint32_t timing = 0;   
    int src_size = 0;      
    int compressed_data_size = 0;
    int max_dst_size = 0;  
    int decompressed_size = 0; 

    reset_buffer(compressBuffer,0x00,BUFF_SIZE*2);
    reset_buffer(decompressBuffer,0x00,BUFF_SIZE*2);

    // DEMO4
    printf("\r\ndemo 4 , const buffer , perform 1024 times , calulate timing\r\n");

    // compress buffer
    src_size = (int)(strlen(srcBuffer4) + 1);
    printf("srcSize = 0x%2X(%3d)\r\n" , src_size,src_size); 

    max_dst_size = LZ4_compressBound(src_size);
    printf("max_dst_size = 0x%2X(%3d)\r\n" , max_dst_size,max_dst_size); 

    compressed_data_size = LZ4_compress_default( (const char*) srcBuffer4 , (char*) compressBuffer , src_size, max_dst_size);   
    // compressed_data_size = LZ4_compress_private( (const char*) srcBuffer4 , (char*) compressBuffer , src_size, max_dst_size,1);       
    printf("compressed_data_size = 0x%2X(%3d)\r\n" , compressed_data_size,compressed_data_size);

    // decompressed buffer
    decompressed_size = LZ4_decompress_safe((const char*) compressBuffer , (char*)decompressBuffer, compressed_data_size, BUFF_SIZE*2);
    printf("decompressed_size = 0x%2X(%3d)\r\n" , decompressed_size,decompressed_size);

    // compare src and decompressed buffer
    rate = (float) compressed_data_size * 100 / decompressed_size ;
    printf("src size = 0x%2X(%3d), decompressed size = 0x%2X(%3d) , rate:%4.4f\r\n",src_size,src_size,decompressed_size,decompressed_size,rate);
    compare_buffer((uint8_t *)srcBuffer4,decompressBuffer,decompressed_size);

    reset_buffer(compressBuffer,0x00,BUFF_SIZE*2);
    reset_buffer(decompressBuffer,0x00,BUFF_SIZE*2);
    printf("perform compress and decompress , %d times\r\n" , count);

    timing = get_tick();
    for (i = 0 ; i < count ; i++)
    {
        compressed_data_size = LZ4_compress_default( (const char*) srcBuffer4 , (char*) compressBuffer , src_size, max_dst_size);  
        decompressed_size = LZ4_decompress_safe((const char*) compressBuffer , (char*)decompressBuffer, compressed_data_size, BUFF_SIZE*2);
        compare_buffer_REDUCE_LOG((uint8_t *)srcBuffer4,decompressBuffer,decompressed_size);
    }
    printf("timing : %d ms\r\n" , get_tick() - timing);

}

void LZ4_Demo3(void)
{
    float rate = 0.0f;
    int src_size = 0;      
    int compressed_data_size = 0;
    int max_dst_size = 0;  
    int decompressed_size = 0; 

    reset_buffer(compressBuffer,0x00,BUFF_SIZE*2);
    reset_buffer(decompressBuffer,0x00,BUFF_SIZE*2);

    // DEMO3
    printf("\r\ndemo 3 , const buffer\r\n");

    // compress buffer
    src_size = (int)(strlen(srcBuffer3) + 1);
    printf("srcSize = 0x%2X(%3d)\r\n" , src_size,src_size); 

    #if 0  // print buffer
    printf("srcBuffer3\r\n");
    dump_buffer_hex( (uint8_t *) srcBuffer3,src_size);
    #endif

    max_dst_size = LZ4_compressBound(src_size);
    printf("max_dst_size = 0x%2X(%3d)\r\n" , max_dst_size,max_dst_size); 
    // compressBuffer = (char*)malloc((size_t)max_dst_size);

    compressed_data_size = LZ4_compress_default( (const char*) srcBuffer3 , (char*) compressBuffer , src_size, max_dst_size);   
    // compressed_data_size = LZ4_compress_private( (const char*) srcBuffer3 , (char*) compressBuffer , src_size, max_dst_size,1);       
    printf("compressed_data_size = 0x%2X(%3d)\r\n" , compressed_data_size,compressed_data_size);

    #if 0  // print buffer
    printf("compressBuffer\r\n");
    // dump_buffer_hex(compressBuffer,BUFF_SIZE*2);
    dump_buffer_hex(compressBuffer,compressed_data_size);
    #endif

    // decompressed buffer
    decompressed_size = LZ4_decompress_safe((const char*) compressBuffer , (char*)decompressBuffer, compressed_data_size, BUFF_SIZE*2);
    printf("decompressed_size = 0x%2X(%3d)\r\n" , decompressed_size,decompressed_size);

    #if 0  // print buffer
    printf("decompressBuffer\r\n");
    // dump_buffer_hex(decompressBuffer,BUFF_SIZE*2);
    dump_buffer_hex(decompressBuffer,decompressed_size);
    #endif

    // compare src and decompressed buffer
    rate = (float) compressed_data_size * 100 / decompressed_size ;
    printf("src size = 0x%2X(%3d), decompressed size = 0x%2X(%3d) , rate:%4.4f\r\n",src_size,src_size,decompressed_size,decompressed_size,rate);
    compare_buffer((uint8_t *)srcBuffer3,decompressBuffer,decompressed_size);
}


void LZ4_Demo2(void)
{
    float rate = 0.0f;    
    int src_size = 0;      
    int compressed_data_size = 0;
    int max_dst_size = 0;  
    int decompressed_size = 0; 

    reset_buffer(compressBuffer,0x00,BUFF_SIZE*2);
    reset_buffer(decompressBuffer,0x00,BUFF_SIZE*2);

    // DEMO2
    printf("\r\ndemo 2 , const buffer\r\n");

    // compress buffer
    src_size = (int)(strlen(srcBuffer2) + 1);
    printf("srcSize = 0x%2X(%3d)\r\n" , src_size,src_size); 

    #if 0  // print buffer
    printf("srcBuffer2\r\n");
    dump_buffer_hex( (uint8_t *) srcBuffer2,src_size);
    #endif

    max_dst_size = LZ4_compressBound(src_size);
    printf("max_dst_size = 0x%2X(%3d)\r\n" , max_dst_size,max_dst_size); 
    // compressBuffer = (char*)malloc((size_t)max_dst_size);

    compressed_data_size = LZ4_compress_default( (const char*) srcBuffer2 , (char*) compressBuffer , src_size, max_dst_size);   
    // compressed_data_size = LZ4_compress_private( (const char*) srcBuffer2 , (char*) compressBuffer , src_size, max_dst_size,1);       
    printf("compressed_data_size = 0x%2X(%3d)\r\n" , compressed_data_size,compressed_data_size);

    #if 0  // print buffer
    printf("compressBuffer\r\n");
    // dump_buffer_hex(compressBuffer,BUFF_SIZE*2);
    dump_buffer_hex(compressBuffer,compressed_data_size);
    #endif

    // decompressed buffer
    decompressed_size = LZ4_decompress_safe((const char*) compressBuffer , (char*)decompressBuffer, compressed_data_size, BUFF_SIZE*2);
    printf("decompressed_size = 0x%2X(%3d)\r\n" , decompressed_size,decompressed_size);

    #if 0  // print buffer
    printf("decompressBuffer\r\n");
    // dump_buffer_hex(decompressBuffer,BUFF_SIZE*2);
    dump_buffer_hex(decompressBuffer,decompressed_size);    
    #endif

    // compare src and decompressed buffer
    rate = (float) compressed_data_size * 100 / decompressed_size ;
    printf("src size = 0x%2X(%3d), decompressed size = 0x%2X(%3d) , rate:%4.4f\r\n",src_size,src_size,decompressed_size,decompressed_size,rate);
    compare_buffer((uint8_t *)srcBuffer2,decompressBuffer,decompressed_size);
}

void LZ4_Demo1(void)
{
    float rate = 0.0f;    
    uint8_t i = 0;
    int src_size = 128;      
    int compressed_data_size = 0;
    int max_dst_size = 0;  
    int decompressed_size = 0; 

    reset_buffer(srcBuffer1,0x00,BUFF_SIZE);
    reset_buffer(compressBuffer,0x00,BUFF_SIZE*2);
    reset_buffer(decompressBuffer,0x00,BUFF_SIZE*2);

    // DEMO1
    printf("\r\ndemo 1 , ram buffer\r\n");

    // prepare data buffer 
    for ( i = 0 ; i < src_size ; i++ )
    {
        srcBuffer1[i] = i ;
    }

    // compress buffer
    // src_size = BUFF_SIZE;
    printf("srcSize = 0x%2X(%3d)\r\n" , src_size,src_size); 

    #if 0  // print buffer
    printf("srcBuffer1\r\n");
    dump_buffer_hex(srcBuffer1,src_size);
    #endif

    max_dst_size = LZ4_compressBound(src_size);
    printf("max_dst_size = 0x%2X(%3d)\r\n" , max_dst_size,max_dst_size); 
    // compressBuffer = (char*)malloc((size_t)max_dst_size);

    compressed_data_size = LZ4_compress_default( (const char*) srcBuffer1 , (char*) compressBuffer , src_size, max_dst_size);   
    // compressed_data_size = LZ4_compress_private( (const char*) srcBuffer1 , compressBuffer , src_size, max_dst_size,1);       
    printf("compressed_data_size = 0x%2X(%3d)\r\n" , compressed_data_size,compressed_data_size);

    #if 0  // print buffer
    printf("compressBuffer\r\n");
    // dump_buffer_hex(compressBuffer,BUFF_SIZE*2);
    dump_buffer_hex(compressBuffer,compressed_data_size);
    #endif

    // decompressed buffer
    decompressed_size = LZ4_decompress_safe((const char*) compressBuffer , (char*)decompressBuffer, compressed_data_size, BUFF_SIZE*2);
    printf("decompressed_size = 0x%2X(%3d)\r\n" , decompressed_size,decompressed_size);

    #if 0  // print buffer
    printf("decompressBuffer\r\n");
    // dump_buffer_hex(decompressBuffer,BUFF_SIZE*2);
    dump_buffer_hex(decompressBuffer,decompressed_size);
    #endif

    // compare src and decompressed buffer
    rate = (float) compressed_data_size * 100 / decompressed_size ;
    printf("src size = 0x%2X(%3d), decompressed size = 0x%2X(%3d) , rate:%4.4f\r\n",src_size,src_size,decompressed_size,decompressed_size,rate);
    compare_buffer((uint8_t *)srcBuffer1,decompressBuffer,decompressed_size);

}

void TMR1_IRQHandler(void)
{
	// static uint32_t LOG = 0;

	
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        TIMER_ClearIntFlag(TIMER1);
		tick_counter();

		if ((get_tick() % 1000) == 0)
		{
			// printf("%s : %4d\r\n",__FUNCTION__,LOG++);
			PH0 ^= 1;
		}

		if ((get_tick() % 50) == 0)
		{

		}	
    }
}


void TIMER1_Init(void)
{
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);	
    TIMER_Start(TIMER1);
}

void UARTx_Process(void)
{
	uint8_t res = 0;
	res = UART_READ(UART0);

	if (res > 0x7F)
	{
		printf("invalid command\r\n");
	}
	else
	{
		switch(res)
		{
			case '1':
				break;

			case 'X':
			case 'x':
			case 'Z':
			case 'z':
				NVIC_SystemReset();		
				break;
		}
	}
}


void UART0_IRQHandler(void)
{
    if(UART_GET_INT_FLAG(UART0, UART_INTSTS_RDAINT_Msk | UART_INTSTS_RXTOINT_Msk))     /* UART receive data available flag */
    {
        while(UART_GET_RX_EMPTY(UART0) == 0)
        {
			UARTx_Process();
        }
    }

    if(UART0->FIFOSTS & (UART_FIFOSTS_BIF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_RXOVIF_Msk))
    {
        UART_ClearIntFlag(UART0, (UART_INTSTS_RLSINT_Msk| UART_INTSTS_BUFERRINT_Msk));
    }	
}

void UART0_Init(void)
{
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);

	/* Set UART receive time-out */
	UART_SetTimeoutCnt(UART0, 20);

	UART0->FIFO &= ~UART_FIFO_RFITL_4BYTES;
	UART0->FIFO |= UART_FIFO_RFITL_8BYTES;

	/* Enable UART Interrupt - */
	UART_ENABLE_INT(UART0, UART_INTEN_RDAIEN_Msk | UART_INTEN_TOCNTEN_Msk | UART_INTEN_RXTOIEN_Msk);
	
	NVIC_EnableIRQ(UART0_IRQn);

	#if (_debug_log_UART_ == 1)	//debug
	printf("\r\nCLK_GetCPUFreq : %8d\r\n",CLK_GetCPUFreq());
	printf("CLK_GetHXTFreq : %8d\r\n",CLK_GetHXTFreq());
	printf("CLK_GetLXTFreq : %8d\r\n",CLK_GetLXTFreq());	
	printf("CLK_GetPCLK0Freq : %8d\r\n",CLK_GetPCLK0Freq());
	printf("CLK_GetPCLK1Freq : %8d\r\n",CLK_GetPCLK1Freq());	

//    printf("Product ID 0x%8X\n", SYS->PDID);
	
	#endif
}

void Custom_Init(void)
{
	SYS->GPH_MFPL = (SYS->GPH_MFPL & ~(SYS_GPH_MFPL_PH0MFP_Msk)) | (SYS_GPH_MFPL_PH0MFP_GPIO);
	SYS->GPH_MFPL = (SYS->GPH_MFPL & ~(SYS_GPH_MFPL_PH1MFP_Msk)) | (SYS_GPH_MFPL_PH1MFP_GPIO);
	SYS->GPH_MFPL = (SYS->GPH_MFPL & ~(SYS_GPH_MFPL_PH2MFP_Msk)) | (SYS_GPH_MFPL_PH2MFP_GPIO);

	//EVM LED
	GPIO_SetMode(PH,BIT0,GPIO_MODE_OUTPUT);
	GPIO_SetMode(PH,BIT1,GPIO_MODE_OUTPUT);
	GPIO_SetMode(PH,BIT2,GPIO_MODE_OUTPUT);
	
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);

    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

//    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

//    CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);

//    CLK_EnableXtalRC(CLK_PWRCTL_LXTEN_Msk);
//    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(FREQ_192MHZ);
    /* Set PCLK0/PCLK1 to HCLK/2 */
    CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2);

    /* Enable UART clock */
    CLK_EnableModuleClock(UART0_MODULE);
    /* Select UART clock source from HXT */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    CLK_EnableModuleClock(TMR1_MODULE);
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HIRC, 0);
	
    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /* Lock protected registers */
    SYS_LockReg();
}

/*
 * This is a template project for M480 series MCU. Users could based on this project to create their
 * own application without worry about the IAR/Keil project settings.
 *
 * This template application uses external crystal as HCLK source and configures UART0 to print out
 * "Hello World", users may need to do extra system configuration based on their system design.
 */

int main()
{
    SYS_Init();

	UART0_Init();
	Custom_Init();	
	TIMER1_Init();

	LZ4_Demo1();
	LZ4_Demo2();
	LZ4_Demo3();
    LZ4_Demo4();

    /* Got no where to go, just loop forever */
    while(1)
    {


    }
}

/*** (C) COPYRIGHT 2016 Nuvoton Technology Corp. ***/
