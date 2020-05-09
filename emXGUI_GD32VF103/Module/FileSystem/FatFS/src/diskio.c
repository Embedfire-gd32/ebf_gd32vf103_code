/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "Kernel_Arch.h"

/*=========================================================================================*/

extern	blk_dev_t*	fatfs_blk_dev[];

/*=========================================================================================*/

/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */

#define ATA		0
#define MMC		1
#define USB		2


/*=========================================================================================*/

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */

DSTATUS disk_initialize (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;
	

	return	RES_OK;


}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	
	int status;

	if(blk_dev_ioctl(fatfs_blk_dev[pdrv],BLK_DEV_GET_STATUS,&status,sizeof(status)))
	{
		return RES_OK;
	}
	else
	{
		return STA_NOINIT;
	}


}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	if(blk_dev_read(fatfs_blk_dev[pdrv],sector,buff,count))
	{
		
		return	RES_OK;
	}
	else
	{
		return	RES_PARERR;
	}

}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	if(blk_dev_write(fatfs_blk_dev[pdrv],sector,(void*)buff,count))
	{
		
		return	RES_OK;
	}
	else
	{
		return	RES_PARERR;
	}

}



/*-----------------------------------------------------------------------*/
/* Sync                                                    */

static int	disk_sync(
	BYTE drv	/* Physical drive nmuber (0..) */
)
{
	return RES_OK;

}

/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	blk_dev_info_t info;
	  
	if(blk_dev_ioctl(fatfs_blk_dev[pdrv],BLK_DEV_GET_INFORMATION,&info,sizeof(blk_dev_info_t)))
	{
		switch(cmd)
		{
			
			
			case	CTRL_SYNC:
			disk_sync(pdrv);
			break;
			////
			
			case	GET_SECTOR_SIZE:
			*(WORD*)buff	=info.sector_size;;
		//	DebugPrintf("disk[%d]:sector_size=%d\r\n",drv,*(WORD*)buff);
			break;
			////
			
			case	GET_SECTOR_COUNT:
			*(DWORD*)buff	=info.sector_count;
			break;
			////
			
			case	GET_BLOCK_SIZE:
			*(DWORD*)buff	=info.block_size;
			break;
			////
		}
		
		return RES_OK;
	}

	return RES_PARERR;
}

/*=========================================================================================*/
/*=========================================================================================*/
