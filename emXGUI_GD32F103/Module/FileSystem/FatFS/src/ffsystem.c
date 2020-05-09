/*------------------------------------------------------------------------*/
/* Sample code of OS dependent controls for FatFs                         */
/* (C)ChaN, 2017                                                          */
/*------------------------------------------------------------------------*/

#include	<stddef.h>

#include "ff.h"

#include	"x_libc.h"
#include	"Kernel.h"
#include	"Kernel_Arch.h"

/*=========================================================================================*/

#if FF_USE_LFN == 3	/* Dynamic memory allocation */

/*------------------------------------------------------------------------*/
/* Allocate a memory block                                                */
/*------------------------------------------------------------------------*/

void* ff_memalloc (	/* Returns pointer to the allocated memory block (null on not enough core) */
	UINT msize		/* Number of bytes to allocate */
)
{
	return vmalloc(msize);	/* Allocate a new memory block with POSIX API */
}


/*------------------------------------------------------------------------*/
/* Free a memory block                                                    */
/*------------------------------------------------------------------------*/

void ff_memfree (
	void* mblock	/* Pointer to the memory block to free */
)
{
	vfree(mblock);	/* Free the memory block with POSIX API */
}

#endif

/*=========================================================================================*/

#if FF_FS_NORTC == 0
DWORD	get_fattime(void)
{
	u8 year,month,day,hour,min,sec;
	local_time_t time;
	////

	SYS_get_local_time(&time);

	year	=time.year-1980;
	month	=time.mon;
	day		=time.day;
	hour	=time.hour;
	min		=time.min;
	sec		=time.sec>>1;

/*
Year 	31:25
Month	24:21
Day		20:16
Hour	15:11
Min		10:5
Sec		4:0
*/

	return (year<<25)|(month<<21)|(day<<16)|(hour<<11)|(min<<5)|(sec);

}
#endif

/*=========================================================================================*/

#if FF_FS_REENTRANT	/* Mutal exclusion */

/*------------------------------------------------------------------------*/
/* Create a Synchronization Object
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to create a new
/  synchronization object for the volume, such as semaphore and mutex.
/  When a 0 is returned, the f_mount() function fails with FR_INT_ERR.
*/

//const osMutexDef_t Mutex[FF_VOLUMES];	/* CMSIS-RTOS */


int ff_cre_syncobj (	/* 1:Function succeeded, 0:Could not create the sync object */
	BYTE vol,			/* Corresponding volume (logical drive number) */
	FF_SYNC_t *sobj		/* Pointer to return the created sync object */
)
{
	mutex_obj *mutex;

	mutex =SYS_mutex_create(NULL);
	if(mutex!=NULL)
	{
		*sobj	=mutex;
		return 1;
	}
	return 0;
}


/*------------------------------------------------------------------------*/
/* Delete a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to delete a synchronization
/  object that created with ff_cre_syncobj() function. When a 0 is returned,
/  the f_mount() function fails with FR_INT_ERR.
*/

int ff_del_syncobj (	/* 1:Function succeeded, 0:Could not delete due to an error */
	FF_SYNC_t sobj		/* Sync object tied to the logical drive to be deleted */
)
{
	SYS_mutex_destroy(sobj);
	return 1;
}


/*------------------------------------------------------------------------*/
/* Request Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on entering file functions to lock the volume.
/  When a 0 is returned, the file function fails with FR_TIMEOUT.
*/

int ff_req_grant (	/* 1:Got a grant to access the volume, 0:Could not get a grant */
	FF_SYNC_t sobj	/* Sync object to wait */
)
{
  	if(SYS_mutex_lock(sobj,0xFFFFFFFF)==SYS_NO_ERR)
  	{
  		return 1;
  	}
  	return	0;
}


/*------------------------------------------------------------------------*/
/* Release Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on leaving file functions to unlock the volume.
*/

void ff_rel_grant (
	FF_SYNC_t sobj	/* Sync object to be signaled */
)
{
	SYS_mutex_unlock(sobj);
}

#endif

/*=========================================================================================*/

#if	FF_USE_LFN

#if 1

WCHAR ff_uni2oem (	/* Returns OEM code character, zero on error */
	WCHAR	uni,	/* Unicode character to be converted */
	WORD	cp		/* Code page for the conversion */
)
{
	char buf[4];
	WCHAR wbuf[2];

	wbuf[0]	=uni;
	wbuf[1]	=L'0';
	x_wcstombs(buf,wbuf,2);

	if(buf[0]<0x80)
	{
		return buf[0];
	}
	else
	{
		return (buf[0]<<8)|buf[1];
	}
}

WCHAR ff_oem2uni (	/* Returns Unicode character, zero on error */
	WCHAR	oem,	/* OEM code to be converted */
	WORD	cp		/* Code page for the conversion */
)
{
	char buf[4];
	WCHAR wbuf[2];
	if(oem<0x80)
	{
		buf[0]	=oem;
		buf[1]	='\0';
		buf[2]	='\0';
		buf[3]	='\0';
	}
	else
	{
		buf[0]	=oem>>8;
		buf[1]	=oem;
		buf[2]	='\0';
		buf[3]	='\0';
	}
	x_mbstowcs(wbuf,buf,2);
	return	wbuf[0];

}

/*------------------------------------------------------------------------*/
/* Unicode up-case conversion                                             */
/*------------------------------------------------------------------------*/

WCHAR ff_wtoupper (	/* Returns up-converted character */
	WCHAR uni		/* Unicode character to be upper converted (BMP only) */
)
{
	/* Compressed upper conversion table */
	static const WCHAR cvt1[] = {	/* U+0000 - U+0FFF */
		/* Basic Latin */
		0x0061,0x031A,
		/* Latin-1 Supplement */
		0x00E0,0x0317,  0x00F8,0x0307,  0x00FF,0x0001,0x0178,
		/* Latin Extended-A */
		0x0100,0x0130,  0x0132,0x0106,  0x0139,0x0110,  0x014A,0x012E,  0x0179,0x0106,
		/* Latin Extended-B */
		0x0180,0x004D,0x0243,0x0181,0x0182,0x0182,0x0184,0x0184,0x0186,0x0187,0x0187,0x0189,0x018A,0x018B,0x018B,0x018D,0x018E,0x018F,0x0190,0x0191,0x0191,0x0193,0x0194,0x01F6,0x0196,0x0197,0x0198,0x0198,0x023D,0x019B,0x019C,0x019D,0x0220,0x019F,0x01A0,0x01A0,0x01A2,0x01A2,0x01A4,0x01A4,0x01A6,0x01A7,0x01A7,0x01A9,0x01AA,0x01AB,0x01AC,0x01AC,0x01AE,0x01AF,0x01AF,0x01B1,0x01B2,0x01B3,0x01B3,0x01B5,0x01B5,0x01B7,0x01B8,0x01B8,0x01BA,0x01BB,0x01BC,0x01BC,0x01BE,0x01F7,0x01C0,0x01C1,0x01C2,0x01C3,0x01C4,0x01C5,0x01C4,0x01C7,0x01C8,0x01C7,0x01CA,0x01CB,0x01CA,
		0x01CD,0x0110,  0x01DD,0x0001,0x018E,  0x01DE,0x0112,  0x01F3,0x0003,0x01F1,0x01F4,0x01F4,  0x01F8,0x0128,
		0x0222,0x0112,  0x023A,0x0009,0x2C65,0x023B,0x023B,0x023D,0x2C66,0x023F,0x0240,0x0241,0x0241,  0x0246,0x010A,
		/* IPA Extensions */
		0x0253,0x0040,0x0181,0x0186,0x0255,0x0189,0x018A,0x0258,0x018F,0x025A,0x0190,0x025C,0x025D,0x025E,0x025F,0x0193,0x0261,0x0262,0x0194,0x0264,0x0265,0x0266,0x0267,0x0197,0x0196,0x026A,0x2C62,0x026C,0x026D,0x026E,0x019C,0x0270,0x0271,0x019D,0x0273,0x0274,0x019F,0x0276,0x0277,0x0278,0x0279,0x027A,0x027B,0x027C,0x2C64,0x027E,0x027F,0x01A6,0x0281,0x0282,0x01A9,0x0284,0x0285,0x0286,0x0287,0x01AE,0x0244,0x01B1,0x01B2,0x0245,0x028D,0x028E,0x028F,0x0290,0x0291,0x01B7,
		/* Greek, Coptic */
		0x037B,0x0003,0x03FD,0x03FE,0x03FF,  0x03AC,0x0004,0x0386,0x0388,0x0389,0x038A,  0x03B1,0x0311,
		0x03C2,0x0002,0x03A3,0x03A3,  0x03C4,0x0308,  0x03CC,0x0003,0x038C,0x038E,0x038F,  0x03D8,0x0118,
		0x03F2,0x000A,0x03F9,0x03F3,0x03F4,0x03F5,0x03F6,0x03F7,0x03F7,0x03F9,0x03FA,0x03FA,
		/* Cyrillic */
		0x0430,0x0320,  0x0450,0x0710,  0x0460,0x0122,  0x048A,0x0136,  0x04C1,0x010E,  0x04CF,0x0001,0x04C0,  0x04D0,0x0144,
		/* Armenian */
		0x0561,0x0426,

		0x0000
	};
	static const WCHAR cvt2[] = {	/* U+1000 - U+FFFF */
		/* Phonetic Extensions */
		0x1D7D,0x0001,0x2C63,
		/* Latin Extended Additional */
		0x1E00,0x0196,  0x1EA0,0x015A,
		/* Greek Extended */
		0x1F00,0x0608,  0x1F10,0x0606,  0x1F20,0x0608,  0x1F30,0x0608,  0x1F40,0x0606,
		0x1F51,0x0007,0x1F59,0x1F52,0x1F5B,0x1F54,0x1F5D,0x1F56,0x1F5F,  0x1F60,0x0608,
		0x1F70,0x000E,0x1FBA,0x1FBB,0x1FC8,0x1FC9,0x1FCA,0x1FCB,0x1FDA,0x1FDB,0x1FF8,0x1FF9,0x1FEA,0x1FEB,0x1FFA,0x1FFB,
		0x1F80,0x0608,  0x1F90,0x0608,  0x1FA0,0x0608,  0x1FB0,0x0004,0x1FB8,0x1FB9,0x1FB2,0x1FBC,
		0x1FCC,0x0001,0x1FC3,  0x1FD0,0x0602,  0x1FE0,0x0602,  0x1FE5,0x0001,0x1FEC,  0x1FF3,0x0001,0x1FFC,
		/* Letterlike Symbols */
		0x214E,0x0001,0x2132,
		/* Number forms */
		0x2170,0x0210,  0x2184,0x0001,0x2183,
		/* Enclosed Alphanumerics */
		0x24D0,0x051A,  0x2C30,0x042F,
		/* Latin Extended-C */
		0x2C60,0x0102,  0x2C67,0x0106, 0x2C75,0x0102,
		/* Coptic */
		0x2C80,0x0164,
		/* Georgian Supplement */
		0x2D00,0x0826,
		/* Full-width */
		0xFF41,0x031A,

		0x0000
	};
	const WCHAR *p;
	WCHAR bc, nc, cmd;


	p = uni < 0x1000 ? cvt1 : cvt2;
	for (;;) {
		bc = *p++;								/* Get block base */
		if (!bc || uni < bc) break;
		nc = *p++; cmd = nc >> 8; nc &= 0xFF;	/* Get processing command and block size */
		if (uni < bc + nc) {	/* In the block? */
			switch (cmd) {
			case 0:	uni = p[uni - bc]; break;		/* Table conversion */
			case 1:	uni -= (uni - bc) & 1; break;	/* Case pairs */
			case 2: uni -= 16; break;				/* Shift -16 */
			case 3:	uni -= 32; break;				/* Shift -32 */
			case 4:	uni -= 48; break;				/* Shift -48 */
			case 5:	uni -= 26; break;				/* Shift -26 */
			case 6:	uni += 8; break;				/* Shift +8 */
			case 7: uni -= 80; break;				/* Shift -80 */
			case 8:	uni -= 0x1C60; break;			/* Shift -0x1C60 */
			}
			break;
		}
		if (!cmd) p += nc;
	}

	return uni;
}

#else
#include	"option/ffunicode.c"
#endif

#endif
/*=========================================================================================*/

