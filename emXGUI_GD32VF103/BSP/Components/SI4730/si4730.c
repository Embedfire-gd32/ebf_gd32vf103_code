#include "BSP.h"
#include "GUI_Drv.h"
#include "Kernel.h"
#include "si4730.h"

/*============================================================================*/

static const struct i2c_ops *pIIC = NULL;

/*============================================================================*/

/*
	FM (64–108 MHz)
	AM (520–1710 kHz)

*/

/*
	i2c 总线时序，见 AN332 page = 226

	每个命令必须有完整的START + STOP信号，例如: [] 表示读取器件返回
	START ADDR+W [ACK] CMD  [ACK] ARG1 [ACK] ARG2 [ACK] ARG3 [ACK] STOP
	START  0x22    0  0x30    0   0x00   0   0x27   0   0x7E   0  STOP

	循环读取器件返回的状态，只到 STARTUS = 0x80
	START ADDR+R [ACK] [STATUS] NACK STOP
	START  0x23    0    0x00    1   STOP

	读取器件返回的数据
	START ADDR+R [ACK] STATUS ACK RESP1 ACK RESP2 ACK RESP3 NACK STOP
	START  0x23    0   0x80   0  0x00   0  0x00   0  0x00   1   STOP

	备注: [ACK] 是CPU发送一个SCL, 然后读取SDA
		  ACK   是CPU设置SDA=0 ,然后发送一个SCL
*/

/*
	AN223 page = 271    FM 收音机模式配置流程
	12.2. Programming Example for the FM/RDS Receiver
*/



/*
*********************************************************************************************************
*	函 数 名: bsp_InitSi4730
*	功能说明: 配置Si4703工作模式
*	形    参:无
*	返 回 值: 无
*********************************************************************************************************
*/
BOOL SI4730_Init(const struct i2c_ops *iic)
{
	pIIC =iic;

	SYS_msleep(50);
	return TRUE;
}

/*
*********************************************************************************************************
*	函 数 名: SI4730_Delay
*	功能说明: 延迟一段时间
*	形    参: n 循环次数
*	返 回 值: 无
*********************************************************************************************************
*/
static void delay_ms(u32 ms)
{
	SYS_msleep(ms);
}

/*
*********************************************************************************************************
*	函 数 名: SI4730_SendCmd
*	功能说明: 向Si4730发送CMD
*	形    参: _pCmdBuf : 命令数组
*			 _CmdLen : 命令串字节数
*	返 回 值: 0 失败(器件无应答)， 1 成功
*********************************************************************************************************
*/
static BOOL SI4730_SendCmd(u8 *_pCmdBuf, u8 _ucCmdLen)
{
	int i;

	pIIC->Open();

	pIIC->Start();
	if(!pIIC->WriteByte(I2C_ADDR_SI4730_W,IIC_ACK))
	{
		goto err;
	}

	for (i = 0; i < _ucCmdLen; i++)
	{
		if(!pIIC->WriteByte(_pCmdBuf[i],IIC_ACK))
		{
			goto err;
		}
	}
	pIIC->Stop();

	pIIC->Close();
	return TRUE;

err:
	pIIC->Stop();
	pIIC->Close();
	return FALSE;
}

/*
*********************************************************************************************************
*	函 数 名: SI4730_WaitStatus80
*	功能说明: 读取Si4730的状态，等于0x80时返回。
*	形    参: _uiTimeOut : 轮询次数
*			  _ucStopEn : 状态0x80检测成功后，是否发送STOP
*	返 回 值: 0 失败(器件无应答)， > 1 成功, 数字表示实际轮询次数
*********************************************************************************************************
*/
static BOOL SI4730_WaitStatus80(u32 timeout, u8 _ucStopEn)
{
	u8 ack;
	u8 status;
	u32 i;

	pIIC->Open();

	/* 等待器件状态为 0x80 */
	for (i = 0; i < timeout; i++)
	{
		pIIC->Start();
		if(!pIIC->WriteByte(I2C_ADDR_SI4730_R,IIC_ACK))
		{
			pIIC->Stop();

			pIIC->Close();
			return FALSE;	/* 器件无应答，失败 */
		}
		status = pIIC->ReadByte(IIC_NoACK);
		if ((status == 0x80) || (status == 0x81))	/* 0x81 是为了执行0x23指令 读取信号质量 */
		{
			break;
		}
	}

	pIIC->Stop();
	pIIC->Close();

	if(i >= timeout)
	{
		return FALSE;
	}
	return TRUE;
}

/*
*********************************************************************************************************
*	函 数 名: SI4730_SetProperty
*	功能说明: 设置Si4730属性参数
*	形    参: _usPropNumber : 参数号
*			  _usPropValue : 参数值
*	返 回 值: 0 失败(器件无应答)， > 1 成功
*********************************************************************************************************
*/
u8 SI4730_SetProperty(u16 _usPropNumber, u16 _usPropValue)
{
	u8 ucCmdBuf[32];
	u32 uiTimeOut;

	ucCmdBuf[0] = 0x12;
	ucCmdBuf[1] = 0x00;

	ucCmdBuf[2] = _usPropNumber >> 8;
	ucCmdBuf[3] = _usPropNumber;

	ucCmdBuf[4] = _usPropValue >> 8;
	ucCmdBuf[5] = _usPropValue;
	SI4730_SendCmd(ucCmdBuf, 6);

	uiTimeOut = SI4730_WaitStatus80(100000, 1);
	if (uiTimeOut == 0)
	{
		return 0;
	}

	return 1;
}

/* 下面2个函数是按I2C总线时序书写。可以发现很多代码是可以共用的。因此我们对部分代码进行封装，已便于实现其他命令 */
#if 0
	/*
	*********************************************************************************************************
	*	函 数 名: SI4730_PowerUp_FM_Revice
	*	功能说明: 配置Si4703为FM接收模式， 模拟模式（非数字模式)
	*	形    参:无
	*	返 回 值: 0 失败， 1 成功
	*********************************************************************************************************
	*/
	u8 SI4730_PowerUp_FM_Revice(void)
	{
		u8 ack;
		u8 status;

		/* AN332  page = 277
			Powerup in Analog Mode
			CMD      0x01     POWER_UP
			ARG1     0xC0     Set to FM Receive. Enable interrupts.
			ARG2     0x05     Set to Analog Audio Output
			STATUS   →0x80   Reply Status. Clear-to-send high.
		*/
		i2c_Start();
		i2c_SendByte(I2C_ADDR_SI4730_W);
		ack = i2c_WaitAck();
		i2c_SendByte(0x01);
		ack = i2c_WaitAck();
		i2c_SendByte(0xC0);
		ack = i2c_WaitAck();
		i2c_SendByte(0x05);
		ack = i2c_WaitAck();
		i2c_Stop();

		/* 等待器件返回状态 0x80 */
		{
			u32 i;

			for (i = 0; i < 2500; i++)
			{
				i2c_Start();
				i2c_SendByte(I2C_ADDR_SI4730_R);	/* 读 */
				ack = i2c_WaitAck();
				status = i2c_ReadByte();
				i2c_NAck();
				i2c_Stop();

				if (status == 0x80)
				{
					break;
				}
			}

			/* 实测 535 次循环应该正常退出 */
			if (i == 2500)
			{
				return 0;
			}
		}

		return 1;
	}

	/*
	*********************************************************************************************************
	*	函 数 名: SI4730_GetRevision
	*	功能说明: 读取器件、固件信息。 返回8字节数据
	*	形    参:_ReadBuf  返回结果存放在此缓冲区，请保证缓冲区大小大于等于8
	*	返 回 值: 0 失败， 1 成功
	*********************************************************************************************************
	*/
	u8 SI4730_GetRevision(u8 *_ReadBuf)
	{
		u8 ack;
		u8 status;
		u32 i;

		/* AN223 page = 67 */

		/* 发送 0x10 命令 */
		i2c_Start();
		i2c_SendByte(I2C_ADDR_SI4730_W);
		ack = i2c_WaitAck();
		i2c_SendByte(0x10);
		ack = i2c_WaitAck();
		i2c_Stop();

		/* 等待器件状态为 0x80 */
		for (i = 0; i < 50; i++)
		{
			i2c_Start();
			i2c_SendByte(I2C_ADDR_SI4730_R);	/* 读 */
			ack = i2c_WaitAck();
			status = i2c_ReadByte();
			if (status == 0x80)
			{
				break;
			}
		}
		/* 实测 2 次循环应该正常退出 */
		if (i == 50)
		{
			i2c_NAck();
			i2c_Stop();
			return 0;
		}

		/* 连续读取8个字节的器件返回信息 */
		for (i = 0; i < 8; i++)
		{
			i2c_Ack();
			_ReadBuf[i] = i2c_ReadByte();
		}
		i2c_NAck();
		i2c_Stop();
		return 1;
	}
#endif

/*
*********************************************************************************************************
*	函 数 名: SI4730_PowerUp_FM_Revice
*	功能说明: 配置Si4703为FM接收模式， 模拟模式（非数字模式)
*	形    参:无
*	返 回 值: 0 失败， 1 成功
*********************************************************************************************************
*/
u8 SI4730_PowerUp_FM_Revice(void)
{
	/* AN332  page = 277
		Powerup in Analog Mode
		CMD      0x01     POWER_UP
		ARG1     0xC0     Set to FM Receive. Enable interrupts.
		ARG2     0x05     Set to Analog Audio Output
		STATUS   →0x80   Reply Status. Clear-to-send high.
	*/

	u8 ucCmdBuf[3];
	u32 uiTimeOut;

	ucCmdBuf[0] = 0x01;
	ucCmdBuf[1] = 0xD0; //0xC0;
	ucCmdBuf[2] = 0x05;
	SI4730_SendCmd(ucCmdBuf, 3);

	/*
		第1个形参表示最大轮询次数； 如果成功，返回值uiTimeOut > 0 表示实际轮询次数
		第2个形参1表示结束后发送STOP
	*/
	uiTimeOut = SI4730_WaitStatus80(1000, 1);
	if (uiTimeOut > 0)
	{
		return 1;
	}

	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: SI4730_PowerUp_AM_Revice
*	功能说明: 配置Si4703为AM接收模式， 模拟模式（非数字模式)
*	形    参:无
*	返 回 值: 0 失败， 1 成功
*********************************************************************************************************
*/
u8 SI4730_PowerUp_AM_Revice(void)
{
	/* AN332  page = 277
		Powerup in Analog Mode
		CMD      0x01     POWER_UP
		ARG1     0xC0     Set to FM Receive. Enable interrupts.
		ARG2     0x05     Set to Analog Audio Output
		STATUS   →0x80   Reply Status. Clear-to-send high.
	*/

	u8 ucCmdBuf[3];
	u32 uiTimeOut;

	ucCmdBuf[0] = 0x01;
	ucCmdBuf[1] = 0x91;
	ucCmdBuf[2] = 0x05;
	SI4730_SendCmd(ucCmdBuf, 3);

	/*
		第1个形参表示最大轮询次数； 如果成功，返回值uiTimeOut > 0 表示实际轮询次数
		第2个形参1表示结束后发送STOP
	*/
	uiTimeOut = SI4730_WaitStatus80(1000, 1);
	if (uiTimeOut > 0)
	{
		return 1;
	}


    SI4730_SetProperty(0x3403, 5);
    SI4730_SetProperty(0x3404, 25);

    SI4730_SetProperty(0x3402, 10); // Set spacing to 10kHz
    SI4730_SetProperty(0x3400, 520); // Set the band bottom to 520kHz
    SI4730_SetProperty(0x3401, 1710);   // Set the band top to 1710kHz

	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: SI4730_PowerDown
*	功能说明: 关闭 Si470电源，模拟输出关闭
*	形    参:无
*	返 回 值: 0 失败， 1 成功
*********************************************************************************************************
*/
u8 SI4730_PowerDown(void)
{
	/*
		AN332  page = 15

		Moves the device from powerup to powerdown mode.
			Command 0x11. POWER_DOWN
	*/

	u8 ucCmdBuf[3];
	u32 uiTimeOut;

	ucCmdBuf[0] = 0x11;
	SI4730_SendCmd(ucCmdBuf, 1);

	/*
		第1个形参表示最大轮询次数； 如果成功，返回值uiTimeOut > 0 表示实际轮询次数
		第2个形参1表示结束后发送STOP
	*/
	uiTimeOut = SI4730_WaitStatus80(1000, 1);
	if (uiTimeOut > 0)
	{
		return 1;
	}

	return 0;
}


/*
*********************************************************************************************************
*	函 数 名: SI4730_GetRevision
*	功能说明: 读取器件、固件信息。 返回8字节数据
*	形    参:_ReadBuf  返回结果存放在此缓冲区，请保证缓冲区大小大于等于8
*	返 回 值: 0 失败， 1 成功
*********************************************************************************************************
*/
u8 SI4730_GetRevision(u8 *_ReadBuf)
{
	u8 ucCmdBuf[2];
	u32 uiTimeOut;
	u8 i;

	/* AN223 page = 67 */

	/* 发送 0x10 命令 */
	ucCmdBuf[0] = 0x10;
	SI4730_SendCmd(ucCmdBuf, 1);

	/*
		第1个形参表示最大轮询次数； 如果成功，返回值uiTimeOut > 0 表示实际轮询次数
		第2个形参0表示结束后不发送STOP， 因为还需要读取器件返回数据
	*/
	uiTimeOut = SI4730_WaitStatus80(10, 0);
	if (uiTimeOut == 0)
	{
		return 0;
	}

	/* 连续读取8个字节的器件返回信息 */
	for (i = 0; i < 8; i++)
	{
		i2c_Ack();
		_ReadBuf[i] = i2c_ReadByte();
	}
	i2c_NAck();
	i2c_Stop();
	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: SI4730_SetFMFreq
*	功能说明: 设置FM调谐频率
*	形    参:_uiFreq : 频率值, 单位 10kHz
*	返 回 值: 0 失败， 1 成功
*********************************************************************************************************
*/
BOOL SI4730_SetFMFreq(u32 _uiFreq)
{
	/* AN332 page = 70 */

	/*
		CMD		 0x20 	FM_TUNE_FREQ
		ARG1     0x00
		ARG2     0x27	Set frequency to 102.3 MHz = 0x27F6
		ARG3     0xF6
		ARG4     0x00   Set antenna tuning capacitor to auto.
		STATUS   ?0x80	Reply Status. Clear-to-send high.
	*/

	/* 64 and 108 MHz in 10 kHz units. */

	u8 ucCmdBuf[32];
	u32 uiTimeOut;
	u32 i;
	u8 status;

	ucCmdBuf[0] = 0x20;
	ucCmdBuf[1] = 0x00;
	ucCmdBuf[2] = _uiFreq >> 8;
	ucCmdBuf[3] = _uiFreq;
	ucCmdBuf[4] = 0x00;
	SI4730_SendCmd(ucCmdBuf, 5);

	uiTimeOut = SI4730_WaitStatus80(1000, 1);
	if (uiTimeOut == 0)
	{
		return 0;
	}

	/* 等待器件状态为 0x81 */
	for (i = 0; i < 10; i++)
	{
		/* 0x14. GET_INT_STATUS */
		ucCmdBuf[0] = 0x14;
		SI4730_SendCmd(ucCmdBuf, 1);

		delay_ms(50);

		pIIC->Open();

		pIIC->Start();
		pIIC->WriteByte(I2C_ADDR_SI4730_R,IIC_ACK);	/* 读 */
		status =pIIC->ReadByte(IIC_NoACK);
		pIIC->Stop();

		pIIC->Close();
		if (status == 0x81)
		{
			break;
		}
	}

	if (i >=10)
	{
		return FALSE;	/* 失败 */
	}
	return TRUE;
}

/*
*********************************************************************************************************
*	函 数 名: SI4730_SetAMFreq
*	功能说明: 设置AM调谐频率
*	形    参:_uiFreq : 频率值, 单位 10kHz
*	返 回 值: 0 失败， 1 成功
*********************************************************************************************************
*/
u8 SI4730_SetAMFreq(u32 _uiFreq)
{
	/* AN332 page = 70 */

	/*
		CMD       0x40        AM_TUNE_FREQ
		ARG1      0x00
		ARG2      0x03        Set frequency to 1000 kHz = 0x03E8
		ARG3      0xE8
		ARG4      0x00        Automatically select tuning capacitor
		ARG5      0x00
		STATUS    ?0x80       Reply Status. Clear-to-send high.
	*/

	/* 64 and 108 MHz in 10 kHz units. */

	u8 ucCmdBuf[32];
	u32 uiTimeOut;
	u32 i;
	u8 status;

	ucCmdBuf[0] = 0x40;
	ucCmdBuf[1] = 0x00;
	ucCmdBuf[2] = _uiFreq >> 8;
	ucCmdBuf[3] = _uiFreq;
	ucCmdBuf[4] = 0x00;
	ucCmdBuf[5] = 0x00;
	SI4730_SendCmd(ucCmdBuf, 6);

	uiTimeOut = SI4730_WaitStatus80(10000, 1);
	if (uiTimeOut == 0)
	{
		return 0;
	}



	/* 等待器件状态为 0x81 */
	for (i = 0; i < 5000; i++)
	{
		/* 0x14. GET_INT_STATUS */
		ucCmdBuf[0] = 0x14;
		SI4730_SendCmd(ucCmdBuf, 1);

		SI4730_Delay(10000);

		i2c_Start();
		i2c_SendByte(I2C_ADDR_SI4730_R);	/* 读 */
		i2c_WaitAck();
		status = i2c_ReadByte();
		i2c_Stop();
		if (status == 0x81)
		{
			break;
		}
	}

	if (i == 5000)
	{
		return 0;	/* 失败 */
	}
	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: SI4730_SetAMFreqCap
*	功能说明: 设置AM调谐频率
*	形    参:_uiFreq : 频率值, 单位 10kHz    _usCap : 调谐电容
*	返 回 值: 0 失败， 1 成功
*********************************************************************************************************
*/
u8 SI4730_SetAMFreqCap(u32 _uiFreq, u16 _usCap)
{
	/* AN332 page = 70 */

	/*
		CMD       0x40        AM_TUNE_FREQ
		ARG1      0x00
		ARG2      0x03        Set frequency to 1000 kHz = 0x03E8
		ARG3      0xE8
		ARG4      0x00        Automatically select tuning capacitor
		ARG5      0x00
		STATUS    ?0x80       Reply Status. Clear-to-send high.
	*/

	/* 64 and 108 MHz in 10 kHz units. */

	u8 ucCmdBuf[32];
	u32 uiTimeOut;
	u32 i;
	u8 status;

	ucCmdBuf[0] = 0x40;
	ucCmdBuf[1] = 0x00;
	ucCmdBuf[2] = _uiFreq >> 8;
	ucCmdBuf[3] = _uiFreq;
	ucCmdBuf[4] = _usCap >> 8;
	ucCmdBuf[5] = _usCap;
	SI4730_SendCmd(ucCmdBuf, 6);

	uiTimeOut = SI4730_WaitStatus80(10000, 1);
	if (uiTimeOut == 0)
	{
		return 0;
	}

	/* 等待器件状态为 0x81 */
	for (i = 0; i < 5000; i++)
	{
		/* 0x14. GET_INT_STATUS */
		ucCmdBuf[0] = 0x14;
		SI4730_SendCmd(ucCmdBuf, 1);

		SI4730_Delay(10000);

		i2c_Start();
		i2c_SendByte(I2C_ADDR_SI4730_R);	/* 读 */
		i2c_WaitAck();
		status = i2c_ReadByte();
		i2c_Stop();
		if (status == 0x81)
		{
			break;
		}
	}

	if (i == 5000)
	{
		return 0;	/* 失败 */
	}
	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: SI4730_GetAMTuneStatus
*	功能说明: 读取AM调谐状态
*	形    参: 返回结果存放在此缓冲区，请保证缓冲区大小大于等于7
*	返 回 值: 0 失败， 1 成功
*********************************************************************************************************
*/
u8 SI4730_GetAMTuneStatus(u8 *_ReadBuf)
{
	/*
		CMD       0x42           AM_TUNE_STATUS
		ARG1      0x01           Clear STC interrupt.
		STATUS    ?0x80          Reply Status. Clear-to-send high.

		RESP1     ?0x01          Channel is valid, AFC is not railed, and seek did not wrap at AM band boundary
		RESP2     ?0x03
		RESP3     ?0xE8          Frequency = 0x03E8 = 1000 kHz
		RESP4     ?0x2A          RSSI = 0x2A = 42d = 42 dBμV
		RESP5     ?0x1A          SNR = 0x1A = 26d = 26 dB
		RESP6     ?0x0D          Value the antenna tuning capacitor is set to.
		RESP7     ?0x95          0x0D95 = 3477 dec.
	
		电容计算 The tuning capacitance is 95 fF x READANTCAP + 7 pF	
	*/
	u8 ucCmdBuf[32];
	u32 uiTimeOut;
	u32 i;

	ucCmdBuf[0] = 0x42;
	ucCmdBuf[1] = 0x01;
	SI4730_SendCmd(ucCmdBuf, 2);

	uiTimeOut = SI4730_WaitStatus80(100, 0);
	if (uiTimeOut == 0)
	{
		return 0;
	}

	/* 连续读取7个字节的器件返回信息 */
	for (i = 0; i < 7; i++)
	{
		i2c_Ack();
		_ReadBuf[i] = i2c_ReadByte();
	}
	i2c_NAck();
	i2c_Stop();
	return 1;

}

/*
*********************************************************************************************************
*	函 数 名: SI4730_GetFMTuneStatus
*	功能说明: 读取FM调谐状态
*	形    参: 返回结果存放在此缓冲区，请保证缓冲区大小大于等于7
*	返 回 值: 0 失败， 1 成功
*********************************************************************************************************
*/
u8 SI4730_GetFMTuneStatus(u8 *_ReadBuf)
{
	/*
		CMD      0x22     FM_TUNE_STATUS
		ARG1     0x01     Clear STC interrupt.
		STATUS   ?0x80    Reply Status. Clear-to-send high.

		RESP1    ?0x01    Valid Frequency.
		RESP2    ?0x27    Frequency = 0x27F6 = 102.3 MHz
		RESP3    ?0xF6
		RESP4    ?0x2D    RSSI = 45 dBμV
		RESP5    ?0x33    SNR = 51 dB
		RESP6    ?0x00    MULT[7:0]
		RESP7    ?0x00    Antenna tuning capacitor = 0 (range = 0–191)  READANTCAP[7:0] (Si4704/05/06/2x only)
	*/
	u8 ucCmdBuf[32];
	u32 uiTimeOut;
	u32 i;

	ucCmdBuf[0] = 0x22;
	ucCmdBuf[1] = 0x01;
	SI4730_SendCmd(ucCmdBuf, 2);

	uiTimeOut = SI4730_WaitStatus80(100, 0);
	if (uiTimeOut == 0)
	{
		return 0;
	}

	/* 连续读取7个字节的器件返回信息 */
	for (i = 0; i < 7; i++)
	{
		i2c_Ack();
		_ReadBuf[i] = i2c_ReadByte();
	}
	i2c_NAck();
	i2c_Stop();
	return 1;

}

/*
*********************************************************************************************************
*	函 数 名: SI4730_GetAMSignalQuality
*	功能说明: 读取AM接收信号质量
*	形    参: _ReadBuf 返回结果存放在此缓冲区，请保证缓冲区大小大于等于5
*	返 回 值: 0 失败， 1 成功
*********************************************************************************************************
*/
u8 SI4730_GetAMSignalQuality(u8 *_ReadBuf)
{
	/*
		AM_RSQ_STATUS
		Queries the status of the Received Signal Quality (RSQ) for
		the current channel.

		CMD        0x43      AM_RSQ_STATUS
		ARG1       0x01      Clear STC interrupt.
		STATUS     ?0x80     Reply Status. Clear-to-send high.

		RESP1      ?0x00     No SNR high, low, RSSI high, or low interrupts.
		RESP2      ?0x01     Channel is valid, soft mute is not activated, and AFC is not railed
		RESP3      ?0x00
		RESP4      ?0x2A     RSSI = 0x2A = 42d = 42 dBμV
		RESP5      ?0x1A     SNR = 0x1A = 26d = 26 dB
	*/
	u8 ucCmdBuf[32];
	u32 uiTimeOut;
	u32 i;

	ucCmdBuf[0] = 0x43;
	ucCmdBuf[1] = 0x01;
	SI4730_SendCmd(ucCmdBuf, 2);

	uiTimeOut = SI4730_WaitStatus80(100, 0);
	if (uiTimeOut == 0)
	{
		return 0;
	}

	/* 连续读取5个字节的器件返回信息 */
	for (i = 0; i < 5; i++)
	{
		i2c_Ack();
		_ReadBuf[i] = i2c_ReadByte();
	}
	i2c_NAck();
	i2c_Stop();
	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: SI4730_GetFMSignalQuality
*	功能说明: 读取FM接收信号质量
*	形    参: _ReadBuf 返回结果存放在此缓冲区，请保证缓冲区大小大于等于7
*	返 回 值: 0 失败， 1 成功
*********************************************************************************************************
*/
u8 SI4730_GetFMSignalQuality(u8 *_ReadBuf)
{
	/*
		FM_RSQ_STATUS
		Queries the status of the Received Signal Quality (RSQ) for
		the current channel.

		CMD      0x23    FM_RSQ_STATUS
		ARG1     0x01    Clear RSQINT
		STATUS   ?0x80   Reply Status. Clear-to-send high.
		RESP1    ?0x00   No blend, SNR high, low, RSSI high or low interrupts.
		RESP2    ?0x01   Soft mute is not engaged, no AFC rail, valid frequency.
		RESP3    ?0xD9   Pilot presence, 89% blend
		RESP4    ?0x2D   RSSI = 45 dBμV
		RESP5    ?0x33   SNR = 51 dB
		RESP6    ?0x00
		RESP7    ?0x00   Freq offset = 0 kHz
	*/
	u8 ucCmdBuf[32];
	u32 uiTimeOut;
	u32 i;

	ucCmdBuf[0] = 0x23;
	ucCmdBuf[1] = 0x01;
	SI4730_SendCmd(ucCmdBuf, 2);

	uiTimeOut = SI4730_WaitStatus80(1000, 0);
	if (uiTimeOut == 0)
	{
		return 0;
	}

	/* 连续读取7个字节的器件返回信息 */
	for (i = 0; i < 7; i++)
	{
		i2c_Ack();
		_ReadBuf[i] = i2c_ReadByte();
	}
	i2c_NAck();
	i2c_Stop();
	return 1;
}

/*
*********************************************************************************************************
*	函 数 名: SI4730_SetOutVlomue
*	功能说明: 设置Si4730输出音量
*	形    参: _ucVolume; 值域[0-63];
*	返 回 值: 0 失败， 1 成功
*********************************************************************************************************
*/
u8 SI4730_SetOutVolume(u8 _ucVolume)
{
	/* AN332 page = 123 */

	/*
		Property 0x4000. RX_VOLUME

		Sets the output volume level, 63 max, 0 min. Default is 63.
	*/

	if (_ucVolume > 63)
	{
		_ucVolume = 63;
	}

	return SI4730_SetProperty(0x4000, _ucVolume);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
