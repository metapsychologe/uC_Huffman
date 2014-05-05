/* file:      huffman.h
 * brief: 		huffman coding and encoding
 * author: 		alex
 * note:      all fnctions return 0 if successful, something else otherwise
 * date:			27.04.2014
 * label:			HUFFMAN
 */


#ifndef HUFFMAN_H_
#define HUFFMAN_H_

/*---------------------- P U B L I C   I N C L U D E S ----------------------*/
#include <stdint.h>

/*----------------------- P U B L I C   D E F I N E S -----------------------*/
#define HUFFMAN_N_BYTES (256u) // we are doing 8 bit

/*---------------------- P U B L I C   T Y P E D E F S ----------------------*/
typedef char * HUFFMAN_string_t;
typedef unsigned int HUFFMAN_scatter_t[HUFFMAN_N_BYTES];
typedef uint16_t HUFFMAN_path; // fist bit indicates wether its a leave or
                               // a character

typedef HUFFMAN_path HUFFMAN_node_t[2]; // node[0] for zero and node[1] for one

typedef struct HUFFMAN_tree_s
{
  unsigned int nNodes;
  HUFFMAN_node_t * nodes;
}HUFFMAN_tree_t;

typedef struct HUFFMAN_inverseNode_s
{
  unsigned int nBytes;
  uint8_t code[HUFFMAN_N_BYTES / 8]; // maximum length
}HUFFMAN_encoder_t;

typedef HUFFMAN_encoder_t HUFFMAN_encoderList_t[HUFFMAN_N_BYTES];

/*--------------------- P U B L I C   V A R I A B L E S ---------------------*/


/*--------------------- P U B L I C   F U N C T I O N S ---------------------*/
extern int HUFFMAN_clear(HUFFMAN_scatter_t      /* scatter */);

extern int HUFFMAN_addFile(HUFFMAN_scatter_t    /* scatter */,
                           char const []        /* filePath */);

extern int HUFFMAN_addMemory(HUFFMAN_scatter_t  /* scatter */,
                             void const *       /* pData */,
                             unsigned int       /* nData */);

extern int HUFFMAN_crateTree(HUFFMAN_scatter_t const  /* scatter */,
                             HUFFMAN_tree_t *         /* tree */,
                             HUFFMAN_encoderList_t *  /* encoder */ );


extern int HUFFMAN_printScatter(HUFFMAN_scatter_t const/* scatter */);
extern int HUFFFMAN_printTree(HUFFMAN_tree_t const /* tree */);

#endif /* HUFFMAN_H_ */
