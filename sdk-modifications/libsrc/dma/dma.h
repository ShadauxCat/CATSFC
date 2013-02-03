#ifndef __DMA_H__
#define __DMA_H__

//register a DMA transfer request
//ch: channel id request, there are 6 channles, 
//irq_handler: the DMA interruption handle
//arg: argument to the handle
//mode: DMA mode, such as port width, address increased/fixed, and so on
//type: DMA request type
extern int dma_request(int ch, void (*irq_handler)(unsigned int), unsigned int arg,
		 unsigned int mode, unsigned int type);

//start DMA transfer, must request a DMA first
//ch: channel id
//srcAddr: DMA source address
//dstAddr: DMA destination address
//count: DMA transfer count, the total bytes due the mode in dma_request
extern void dma_start(int ch, unsigned int srcAddr, unsigned int dstAddr,
	       unsigned int count);

//Stop DMA transfer
extern void dma_stop(int ch);

//Wait DMA transfer over
extern int dma_wait_finish(int ch);


/*
 * Copy 'size' bytes from src to dest, in blocks of 32 bytes.
 * size is in bytes and must be a multiple of 32.
 * Both src and dest must be aligned to 32 bytes.
 * Returns 0 on failure, non-zero on success.
 */
extern int dma_copy32Byte(int ch, void *dest, void *src, unsigned int size);
// Blocks of 16 bytes, aligned to 16 bytes
extern int dma_copy16Byte(int ch, void *dest, void *src, unsigned int size);
// Blocks of 4 bytes, aligned to 4 bytes
extern int dma_copy32Bit(int ch, void *dest, void *src, unsigned int size);
// Blocks of 2 bytes, aligned to 2 bytes
extern int dma_copy16Bit(int ch, void *dest, void *src, unsigned int size);
extern int dma_isBusy(int ch);
extern int dma_isFree(int ch);
extern int dma_getFree(void);

#endif //__DMA_H__

