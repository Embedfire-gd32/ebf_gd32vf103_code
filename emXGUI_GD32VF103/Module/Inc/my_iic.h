#ifndef __MYIIC_H__
#define __MYIIC_H__

#ifdef	__cplusplus
extern "C"{
#endif

#include "def.h"

/*=========================================================================================*/

#define	IIC_SPEED_100KHZ	0
#define	IIC_SPEED_200KHZ	1
#define	IIC_SPEED_400KHZ	2
#define	IIC_SPEED_800KHZ	3

//// ACK Type
#define	IIC_NoACK	0
#define	IIC_ACK		1

struct	i2c_ops{

	BOOL (*Init)(void);
	BOOL (*Open)(int iic_speed);
	void (*Close)(void);
	void (*Start)(void);
	void (*Stop)(void);
	BOOL (*WriteByte)(u8 byte,u8 ack);
	u8   (*ReadByte)(u8 ack);

	u8   (*ReadOnly)(void);
	void (*SendAck)(void);
	void (*SendNoAck)(void);
};

/*=========================================================================================*/

#ifdef	__cplusplus
}
#endif

#endif /*__MYIIC_H__*/














