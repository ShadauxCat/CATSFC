#include <stdlib.h>
#include "ds2_dma.h"
#include "ds2_types.h"

#define MAX_DMA_NUM 6  /* max 6 channels */


// DMA request source register
#define DMAC_DRSR_RS_BIT    0
#define DMAC_DRSR_RS_MASK   (0x2f << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_AUTO (8 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_UART0OUT (20 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_UART0IN  (21 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_SSIOUT   (22 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_SSIIN    (23 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_AICOUT   (24 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_AICIN    (25 << DMAC_DRSR_RS_BIT)
  #define DMAC_DRSR_RS_MSCOUT   (26 << DMAC_DRSR_RS_BIT)        //SD0
  #define DMAC_DRSR_RS_MSCIN    (27 << DMAC_DRSR_RS_BIT)


// DMA channel command register
#define DMAC_DCMD_SAI       (1 << 23) /* source address increment */
#define DMAC_DCMD_DAI       (1 << 22) /* dest address increment */
#define DMAC_DCMD_SWDH_BIT  14  /* source port width */
#define DMAC_DCMD_SWDH_MASK (0x03 << DMAC_DCMD_SWDH_BIT)
  #define DMAC_DCMD_SWDH_32 (0 << DMAC_DCMD_SWDH_BIT)
  #define DMAC_DCMD_SWDH_8  (1 << DMAC_DCMD_SWDH_BIT)
  #define DMAC_DCMD_SWDH_16 (2 << DMAC_DCMD_SWDH_BIT)
#define DMAC_DCMD_DWDH_BIT  12  /* dest port width */
#define DMAC_DCMD_DWDH_MASK (0x03 << DMAC_DCMD_DWDH_BIT)
  #define DMAC_DCMD_DWDH_32 (0 << DMAC_DCMD_DWDH_BIT)
  #define DMAC_DCMD_DWDH_8  (1 << DMAC_DCMD_DWDH_BIT)
  #define DMAC_DCMD_DWDH_16 (2 << DMAC_DCMD_DWDH_BIT)
#define DMAC_DCMD_DS_BIT    8  /* transfer data size of a data unit */
#define DMAC_DCMD_DS_MASK   (0x07 << DMAC_DCMD_DS_BIT)
  #define DMAC_DCMD_DS_32BIT    (0 << DMAC_DCMD_DS_BIT)
  #define DMAC_DCMD_DS_8BIT (1 << DMAC_DCMD_DS_BIT)
  #define DMAC_DCMD_DS_16BIT    (2 << DMAC_DCMD_DS_BIT)
  #define DMAC_DCMD_DS_16BYTE   (3 << DMAC_DCMD_DS_BIT)
  #define DMAC_DCMD_DS_32BYTE   (4 << DMAC_DCMD_DS_BIT)
#define DMAC_DCMD_TM        (1 << 7)  /* transfer mode: 0-single 1-block */
#define DMAC_DCCSR_TT   (1 << 3)  /* transfer terminated */

#define DMAC_BASE 0xB3020000
#define REG32(addr) *((volatile u32 *)(addr))
#define DMAC_DCCSR(n) (DMAC_BASE + (0x10 + (n) * 0x20)) /* DMA control/status */
#define REG_DMAC_DCCSR(n) REG32(DMAC_DCCSR((n)))

#define __dmac_channel_transmit_end_detected(n) \
  (  REG_DMAC_DCCSR((n)) & DMAC_DCCSR_TT )

/*
 * Copy 'size' bytes from src to dest, in blocks of 32 bytes.
 * size is in bytes and must be a multiple of 32.
 * Both src and dest must be aligned to 32 bytes.
 * Returns 0 on failure, non-zero on success.
 */
int dma_copy32Byte(int ch, void *dest, void *src, unsigned int size){
  int test = 0;
  if(!(test = dma_request(ch, NULL, 0,
                        //increment dest addr, increment source addr
                        DMAC_DCMD_DAI | DMAC_DCMD_SAI |
                        //set src width 32 bytes, set dest width 32 bytes
                        DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 |
                        //set copy mode to 32 bytes, copy in blocks
                        DMAC_DCMD_DS_32BYTE | DMAC_DCMD_TM,
                        //auto request type
                        DMAC_DRSR_RS_AUTO)))
  {
    dma_start(ch, (unsigned int)src, (unsigned int)dest, size);
  }
  return test;
}

/*

int sampleUsage(in ch, void *dest, void *src, unsigned int size){
  //channel 0 is used for mmc stuff, so its best to avoid using it

  //initialize and start copy
  if(dma_copy32Byte(ch, dest, src, size)){
    dma_wait_finish(ch);//wait for copy to finish
    dma_stop(ch);//must stop after transfer is done to reset channel
    return 0;
  }
  return -1;
}

*/

/*
 * Copy 'size' bytes from src to dest, in blocks of 16 bytes.
 * size is in bytes and must be a multiple of 16.
 * Both src and dest must be aligned to 16 bytes.
 * Returns 0 on failure, non-zero on success.
 */
int dma_copy16Byte(int ch, void *dest, void *src, unsigned int size){
  int test = 0;
  if(!(test = dma_request(ch, NULL, 0,
              DMAC_DCMD_DAI | DMAC_DCMD_SAI | DMAC_DCMD_SWDH_16 |
              DMAC_DCMD_DWDH_16 | DMAC_DCMD_DS_16BYTE | DMAC_DCMD_TM,
              DMAC_DRSR_RS_AUTO)))
  {
    dma_start(ch, (unsigned int)src, (unsigned int)dest, size);
  }

  return test;
}

/*
 * Copy 'size' bytes from src to dest, in blocks of 32 bits (4 bytes).
 * size is in bytes and must be a multiple of 4.
 * Both src and dest must be aligned to 32 bits (4 bytes).
 * Returns 0 on failure, non-zero on success.
 */
int dma_copy32Bit(int ch, void *dest, void *src, unsigned int size){
  int test = 0;
  if(!(test = dma_request(ch, NULL, 0,
              DMAC_DCMD_DAI | DMAC_DCMD_SAI | DMAC_DCMD_SWDH_32 | DMAC_DCMD_DWDH_32 |
              DMAC_DCMD_DS_32BIT  | DMAC_DCMD_TM,
              DMAC_DRSR_RS_AUTO)))
  {
    dma_start(ch, (unsigned int)src, (unsigned int)dest, size);
  }

  return test;
}

/*
 * Copy 'size' bytes from src to dest, in blocks of 16 bits (2 bytes).
 * size is in bytes and must be a multiple of 2.
 * Both src and dest must be aligned to 16 bits (2 bytes).
 * Returns 0 on failure, non-zero on success.
 */
int dma_copy16Bit(int ch, void *dest, void *src, unsigned int size){
  int test = 0;

  if(!(test = dma_request(ch, NULL, 0,
              DMAC_DCMD_DAI | DMAC_DCMD_SAI | DMAC_DCMD_SWDH_16 | DMAC_DCMD_DWDH_16 |
              DMAC_DCMD_DS_16BIT,
              DMAC_DRSR_RS_AUTO)))
  {
    dma_start(ch, (unsigned int)src, (unsigned int)dest, size);
  }

  return test;
}


//returns if a channel is still copying
int dma_isBusy(int ch){
  if(ch < 1 || ch >= MAX_DMA_NUM)
    return 0;

  return !__dmac_channel_transmit_end_detected(ch);
}

//returns the first non busy channel
int dma_getFree(void){
  int i;
  for(i = 1; i < MAX_DMA_NUM; i++){
    if(!dma_isBusy(i))
      return i;
  }
  return -1;
}
