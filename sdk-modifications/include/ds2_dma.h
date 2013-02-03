#ifndef _DS2_DMA_H__
#define _DS2_DMA_H__

#ifdef __cplusplus
extern "C" {
#endif


#define MAX_DMA_NUM 6  /* max 6 channels */


// DMA request source register
#define DMAC_DRSR_RS_BIT    0
#define DMAC_DRSR_RS_AUTO (8 << DMAC_DRSR_RS_BIT)


// DMA channel command register
#define DMAC_DCMD_SAI       (1 << 23) /* source address increment */
#define DMAC_DCMD_DAI       (1 << 22) /* dest address increment */

#define DMAC_DCMD_SWDH_BIT  14  /* source port width */
#define DMAC_DCMD_SWDH_32 (0 << DMAC_DCMD_SWDH_BIT)
#define DMAC_DCMD_SWDH_8  (1 << DMAC_DCMD_SWDH_BIT)
#define DMAC_DCMD_SWDH_16 (2 << DMAC_DCMD_SWDH_BIT)

#define DMAC_DCMD_DWDH_BIT  12  /* dest port width */
#define DMAC_DCMD_DWDH_32 (0 << DMAC_DCMD_DWDH_BIT)
#define DMAC_DCMD_DWDH_8  (1 << DMAC_DCMD_DWDH_BIT)
 #define DMAC_DCMD_DWDH_16 (2 << DMAC_DCMD_DWDH_BIT)

#define DMAC_DCMD_DS_BIT    8  /* transfer data size of a data unit */
#define DMAC_DCMD_DS_32BIT    (0 << DMAC_DCMD_DS_BIT)
#define DMAC_DCMD_DS_8BIT (1 << DMAC_DCMD_DS_BIT)
#define DMAC_DCMD_DS_16BIT    (2 << DMAC_DCMD_DS_BIT)
#define DMAC_DCMD_DS_16BYTE   (3 << DMAC_DCMD_DS_BIT)
#define DMAC_DCMD_DS_32BYTE   (4 << DMAC_DCMD_DS_BIT)

#define DMAC_DCMD_TM        (1 << 7)  /* transfer mode: 0-single 1-block */


//detect if channel has completed job
#define DMAC_DCCSR_TT   (1 << 3)  /* transfer terminated */
#define DMAC_BASE 0xB3020000
#define REG32(addr) *((volatile u32 *)(addr))
#define DMAC_DCCSR(n) (DMAC_BASE + (0x10 + (n) * 0x20)) /* DMA control/status */
#define REG_DMAC_DCCSR(n) REG32(DMAC_DCCSR((n)))

#define ds2_DMA_isBusy(n) \
  !(  REG_DMAC_DCCSR((n)) & DMAC_DCCSR_TT )


/*
Copy modes

*/

#define DMA_MODE32BYTE DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 | \
                   DMAC_DCMD_DS_32BYTE | DMAC_DCMD_TM

#define DMA_MODE16BYTE DMAC_DCMD_SWDH_16 | DMAC_DCMD_DWDH_16 | \
                   DMAC_DCMD_DS_16BYTE | DMAC_DCMD_TM

#define DMA_MODE32BIT DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 | \
                   DMAC_DCMD_DS_32BIT

#define DMA_MODE16BIT DMAC_DCMD_SWDH_16 | DMAC_DCMD_DWDH_16 | \
                   DMAC_DCMD_DS_16BIT

#define DMA_MODE8BIT DMAC_DCMD_SWDH_8 | DMAC_DCMD_DWDH_8 | \
                   DMAC_DCMD_DS_8BIT | DMAC_DCMD_TM

#define DMA_MODECOPY DMAC_DCMD_SAI



extern int _dmaCopy(int ch, void *dest, void *src, unsigned int size, unsigned int flags);

//copy from 32 bit source to 32 bit dest
//data must be 32 byte aligned
#define ds2_DMAcopy_32Byte(ch, dest, src, size)\
  _dmaCopy(ch, dest, src, size, DMA_MODECOPY | DMA_MODE32BYTE)

//copy from 16 bit source to 16 bit dest
//data must be 16 byte aligned
#define ds2_DMAcopy_16Byte(ch, dest, src, size)\
  _dmaCopy(ch, dest, src, size, DMA_MODECOPY | DMA_MODE16BYTE);

//copy from 32 bit source to 32 bit dest
//data must be 32 bit aligned
#define ds2_DMAcopy_32Bit(ch, dest, src, size)\
  _dmaCopy(ch, dest, src, size, DMA_MODECOPY | DMA_MODE32BIT);

//copy from 16 bit source to 16 bit dest
//data must be 16 bit aligned
#define ds2_DMAcopy_16Bit(ch, dest, src, size)\
  _dmaCopy(ch, dest, src, size, DMA_MODECOPY | DMA_MODE16BIT)

//copy from 8 bit source to 8 bit dest
//data must be 8 bit aligned
#define ds2_DMAcopy_8Bit(ch, dest, src, size)\
  _dmaCopy(ch, dest, src, size, DMA_MODECOPY | DMA_MODE8BIT)



//Stop DMA transfer
extern void dma_stop(int ch);

#define ds2_DMA_stop(ch)\
  dma_stop(ch)

//Wait DMA transfer over
extern int dma_wait_finish(int ch);

#define ds2_DMA_wait(ch)\
  dma_wait_finish(ch)



#ifdef __cplusplus
}
#endif

#endif //__DMA_H__

