
#ifndef __BSP_SI4730_H__
#define __BSP_SI4730_H__

#ifdef	__cplusplus
extern "C"{
#endif

#define I2C_ADDR_SI4730_W		0x22
#define I2C_ADDR_SI4730_R		0x23

/* Si4730 ¼Ä´æÆ÷¡¢ÃüÁî¶¨Òå */

/* FM/RDS Receiver Command Summary */
enum
{
	SI4730_CMD_POWER_UP		 	= 0x01, /*Power up device and mode selection. */
	SI4730_CMD_GET_REV 			= 0x10, /*Returns revision information on the device. */
	SI4730_CMD_POWER_DOWN 		= 0x11, /*Power down device. */
	SI4730_CMD_SET_PROPERTY 	= 0x12, /*Sets the value of a property. */
	SI4730_CMD_GET_PROPERTY 	= 0x13, /*Retrieves a property¡¯s value. */
	SI4730_CMD_GET_INT_STATUS 	= 0x14, /*Reads interrupt status bits. */
	SI4730_CMD_PATCH_ARGS		= 0x15, /*Reserved command used for patch file downloads. */
	SI4730_CMD_PATCH_DATA		= 0x16, /*Reserved command used for patch file downloads. */
	SI4730_CMD_FM_TUNE_FREQ 	= 0x20, /*Selects the FM tuning frequency. */
	SI4730_CMD_FM_SEEK_START 	= 0x21, /*Begins searching for a valid frequency. */
	SI4730_CMD_FM_TUNE_STATUS 	= 0x22, /*Queries the status of previous FM_TUNE_FREQ or FM_SEEK_START command. */
	SI4730_CMD_FM_RSQ_STATUS 	= 0x23, /*Queries the status of the Received Signal Quality (RSQ) of the current channel. */
	//SI4730_CMD_FM_RDS_STATUS 	= 0x24, /*Returns RDS information for current channel and reads an entry from RDS FIFO. */
	SI4730_CMD_FM_AGC_STATUS 	= 0x27, /*Queries the current AGC settings */
	SI4730_CMD_FM_AGC_OVERRIDE 	= 0x28, /*Override AGC setting by disabling and forcing it to a fixed value */
	SI4730_CMD_GPIO_CTL			= 0x80, /*Configures GPO1, 2, and 3 as output or Hi-Z. */
	SI4730_CMD_GPIO_SET			= 0x81, /*Sets GPO1, 2, and 3 output level (low or high). */
};

BOOL SI4730_Init(const struct i2c_ops *iic);

//u8 SI4730_SendCmd(u8 *_pCmdBuf, u8 _ucCmdLen);
//u32 SI4730_WaitStatus80(u32 _uiTimeOut, u8 _ucStopEn);

u8 SI4730_PowerUp_FM_Revice(void);
u8 SI4730_PowerUp_AM_Revice(void);
u8 SI4730_PowerDown(void);

u8 SI4730_GetRevision(u8 *_ReadBuf);

BOOL SI4730_SetFMFreq(u32 _uiFreq);
u8 SI4730_SetAMFreq(u32 _uiFreq);
u8 SI4730_SetAMFreqCap(u32 _uiFreq, u16 _usCap);

u8 SI4730_GetFMTuneStatus(u8 *_ReadBuf);
u8 SI4730_GetAMTuneStatus(u8 *_ReadBuf);

u8 SI4730_GetFMSignalQuality(u8 *_ReadBuf);
u8 SI4730_GetAMSignalQuality(u8 *_ReadBuf);


u8 SI4730_SetOutVolume(u8 _ucVolume);
u8 SI4730_SetProperty(u16 _usPropNumber, u16 _usPropValue);

#ifdef	__cplusplus
}
#endif

#endif	/*__BSP_SI4730_H__*/
