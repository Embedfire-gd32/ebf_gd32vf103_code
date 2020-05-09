/*!
    \file  main.c
    \brief SPI sd demo
    
    \version 2019-6-5, V1.0.0, demo for GD32VF103
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include <stdio.h>
#include "gd32vf103.h"
#include "systick.h"
#include "gd32vf103v_eval.h"
#include "./sdcard/sdcard_test.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* USART parameter configuration */
    gd_eval_com_init(EVAL_COM0);

    printf("\r\n欢迎使用野火  STM32 开发板。\r\n");

	printf("在开始进行SD卡基本测试前，请给开发板插入32G以内的SD卡\r\n");
	printf("本程序会对SD卡进行 非文件系统 方式读写，会删除SD卡的文件系统\r\n");
	printf("实验后可通过电脑格式化或使用SD卡文件系统的例程恢复SD卡文件系统\r\n");
	printf("\r\n 但sd卡内的原文件不可恢复，实验前务必备份SD卡内的原文件！！！\r\n");

	printf("\r\n 若已确认，请按开发板的KEY1按键，开始SD卡测试实验....\r\n");

	printf("\r\n开始进行SD卡读写实验\r\n");

	SD_Test();
	printf("\r\nSD卡读写实验结束了\r\n");

    while(1){

    }
}


