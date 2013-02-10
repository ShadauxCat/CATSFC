#include "ds2_malloc.h"
#include "dma_adj.h"

#ifdef DS2_DMA

struct SPointerAdjustments PtrAdj;

void* AlignedMalloc (unsigned int Size, unsigned int Alignment, unsigned int* Adjustment)
{
	if (Alignment == 0) Alignment = 1;

	unsigned char* result = (unsigned char*) malloc(Size + Alignment);
	if (!result) {
		return result;
	} else {
		*Adjustment = Alignment - ((unsigned int) result & (Alignment - 1));
		return (void*) (result + *Adjustment);
	}
}

void AlignedFree (void* Memory, unsigned int Adjustment)
{
	free((void*) ((unsigned char*) Memory - Adjustment));
}

#endif // DS2_DMA
