

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

/* Includes */

#include <stdint.h>

/* Definitions */

#define BUFSIZE				16
#define BUFSIZE_MULT		2

/*~~~~~~~~~~ REALLOCATION SELECTION ~~~~~~~~~~~~~~*/
#define ERROR_MODE				0
#define	REALLOC_MODE			1
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define	REALLOCATE_BUFFER		ERROR_MODE

#define HALF_WORDS
//#define FULL_WORDS


/* Structures */

#ifdef HALF_WORDS
typedef struct CircularBuffer_t
{
	uint16_t * buffer_start;		// Beginning of allocated buffer
	uint16_t * head;				// Pointer modified with ADD operations
	uint16_t * tail;				// Pointer modified with REMOVE operations
	uint8_t length;			// The current length of the buffer
	uint32_t capacity;		// The character capacity of the buffer
	uint8_t numReallocs;	// If realloc is enabled, how many times has it been reallocated
} CircularBuffer_t;
#else	// FULL_WORDS

typedef struct CircularBuffer_t
{
	uint32_t * buffer_start;		// Beginning of allocated buffer
	uint32_t * head;				// Pointer modified with ADD operations
	uint32_t * tail;				// Pointer modified with REMOVE operations
	uint8_t length;			// The current length of the buffer
	uint32_t capacity;		// The character capacity of the buffer
	uint8_t numReallocs;	// If realloc is enabled, how many times has it been reallocated
} CircularBuffer_t;
#endif

typedef enum
{
	BUF_SUCCESS = 0,
	BUF_FULL = 1,
	BUF_EMPTY = 2,
	BUF_FAIL = 3
} CircBufferReturn_t;


/* Function Prototypes */
CircularBuffer_t * CircBufCreate();
CircBufferReturn_t 	CircBufInit(CircularBuffer_t * buf, uint8_t size);
CircBufferReturn_t CircBufReset(CircularBuffer_t * buf);
CircBufferReturn_t 	CircBufRealloc(CircularBuffer_t * buf);
CircBufferReturn_t	CircBufAdd(CircularBuffer_t * buf, uint16_t c);
CircBufferReturn_t	CircBufRemove(CircularBuffer_t * buf, uint16_t * c);
CircBufferReturn_t	CircBufIsFull(CircularBuffer_t * buf);
CircBufferReturn_t	CircBufIsEmpty(CircularBuffer_t * buf);
CircBufferReturn_t	CircBufIsValid(CircularBuffer_t * buf);
CircBufferReturn_t	CircBufIsInitialized(CircularBuffer_t * buf);
CircBufferReturn_t 	CircBufDestroy(CircularBuffer_t * buf);


#endif
