/* file:      huffman.c
 * brief: 		
 * author: 		alex
 * date:			27.04.2014
 * label:			
 */

/*--------------------- P R I V A T E   I N C L U D E S ----------------------*/
#include "huffman.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

/*---------------------- P R I V A T E   D E F I N E S -----------------------*/
#define IS_LEAVE(node)  (((uint16_t)(node))&(0x8000))
#define GET_NODE(node)  (((uint16_t)(node))&(~0x8000))
#define MAKE_LEAVE(node) ((node)|(0x8000))

/*--------------------- P R I V A T E   T Y P E D E F S ----------------------*/
typedef struct HUFFMAN_workNode_s
{
  unsigned int quantity;
  _Bool isLeave;
  uint8_t byte; // set when it's a leave
  unsigned int zero;
  unsigned int one;
}HUFFMAN_workNode_t;

/*----------- P R I V A T E   F U N C T I O N P R O T O T Y P E S ------------*/
static _Bool getLeastPropableNodes(HUFFMAN_workNode_t const workTree[],
                                  int nNodes,
                                  unsigned int * node1,
                                  unsigned int * node2);

static void HUFFMAN_createEncoderList(HUFFMAN_tree_t const * tree,
                                      HUFFMAN_encoderList_t * inverseTree,
                                      HUFFMAN_encoder_t * encoder);

void HUFFMAN_createEncodeEntries(HUFFMAN_tree_t const * const tree,
                                 int currentNode,
                                 HUFFMAN_encoder_t * const currentCode,
                                 HUFFMAN_encoderList_t * const list);

/*-------------------- P R I V A T E   V A R I A B L E S ---------------------*/


/*--------------------- P U B L I C   V A R I A B L E S ----------------------*/


/*--------------------- P U B L I C   F U N C T I O N S ----------------------*/
int HUFFMAN_clear(HUFFMAN_scatter_t scatter)
// returns 0 if successful, negative number on error
{
  if(NULL == scatter) return -1;

  memset(scatter, 0, sizeof(HUFFMAN_scatter_t));
  return EXIT_SUCCESS;
}

int HUFFMAN_addFile(HUFFMAN_scatter_t scatter, char const filePath[])
{
  FILE * pFile;
  int character;
  _Bool isLineEnd = false;
  if(NULL == filePath) return -1;

  pFile = fopen(filePath, "r");
  if(NULL == pFile)
  {
    perror("Error opening file");
    return -2;
  }

  while(EOF != (character = fgetc(pFile)))
  { // until we hit the end of file
    if(('\n' == character)
        || ('\r' == character))
    { // fill count newline characters as '\0', but only once per line
      if(!isLineEnd){ // only once per line
        scatter[0]++;
      }
      isLineEnd = true;
    }else
    {
      if((character < HUFFMAN_N_BYTES)
          && (character >= 0)){ // just to get sure
        scatter[character]++;
      }

      isLineEnd = false;
    }
  }

  return EXIT_SUCCESS;
}

int HUFFMAN_addMemory(HUFFMAN_scatter_t scatter,
                      void const * pData,
                      unsigned int nData)
{
  uint8_t const * pByte = (uint8_t const *) pData;
  if(NULL == pData) return -1;
  if(NULL == scatter) return -2;

  while(nData--)
  {
    scatter[*pByte]++;
    pByte++;
  }

  return EXIT_SUCCESS;
}

extern int HUFFMAN_crateTree(HUFFMAN_scatter_t const scatter,
                             HUFFMAN_tree_t * tree,
                             HUFFMAN_encoderList_t * encoder)
{
  HUFFMAN_workNode_t workTree[2*HUFFMAN_N_BYTES - 1];
  int nNodes;
  unsigned int wn1;
  unsigned int wn2;
  HUFFMAN_workNode_t const * readNode;
  HUFFMAN_node_t * writeNode;
  int nBytes = 0;
  int currentNode;
  HUFFMAN_encoder_t workEncoder;

  if(NULL == scatter)     return -1;
  if(NULL == tree)        return -2;
  if(NULL == encoder) return -3;

  memset(workTree, 0, sizeof(workTree));

  // count the occuring bytes
  for(int i = 0; i < HUFFMAN_N_BYTES; i++)
  {
    if(scatter[i]) nBytes++;
  }

  // index of the last node
  nNodes = (nBytes * 2) - 1;
  currentNode = nNodes - 1;

  //create leaves
  for(int i = 0; i < HUFFMAN_N_BYTES; i++)
  {
    if(scatter[i])
    { // only if they even appear
      // first one is the head node
      workTree[currentNode].quantity = scatter[i];
      workTree[currentNode].isLeave = true;
      workTree[currentNode].byte = i;
      // next node
      currentNode--;
    }
  }

  while(getLeastPropableNodes(workTree, nNodes, &wn1, &wn2))
  { // until we are finished
    assert(currentNode >= 0);
    if(currentNode < 0) return -3;
    workTree[currentNode].isLeave = false;
    workTree[currentNode].one = wn1;
    workTree[currentNode].zero = wn2;
    workTree[currentNode].quantity = workTree[wn1].quantity + workTree[wn2].quantity;

    // make wn1 and wn2 void
    workTree[wn1].quantity = 0;
    workTree[wn2].quantity = 0;
    currentNode--;
  }

  nNodes = nBytes - 1;

  tree->nodes = (HUFFMAN_node_t *) calloc(nNodes, sizeof(HUFFMAN_node_t));
  if(NULL == tree->nodes) return -3;

  tree->nNodes = nNodes;

  for(int i = 0; i < nNodes; i++)
  { // use pointers so that I don't have to index everything
    readNode = &workTree[i];
    writeNode = &tree->nodes[i];

    // we should not meet a leave here
    assert(!readNode->isLeave);
    if(readNode->isLeave) return -4;

    // copy the zero-path
    if(workTree[readNode->zero].isLeave)
    { // we need to copy the byte and tell that it is a leave
      (*writeNode)[0] = MAKE_LEAVE(workTree[readNode->zero].byte);
    }else
    { // we need to copy the index
      (*writeNode)[0] = readNode->zero;
    }

    // the same with the one-path
    if(workTree[readNode->one].isLeave)
    { // we need to copy the byte and tell that it is a leave
      (*writeNode)[1] = MAKE_LEAVE(workTree[readNode->one].byte);
    }else
    { // we need to copy the index
      (*writeNode)[1] = readNode->one;
    }
  }

  HUFFMAN_createEncoderList(tree, encoder, & workEncoder);

  return EXIT_SUCCESS;
}

int HUFFMAN_printScatter(HUFFMAN_scatter_t const scatter)
{
  int i;
  unsigned long long int   allNum = 0;

  if(NULL == scatter) return -1;

  for(i = 0; i < HUFFMAN_N_BYTES; i++)
  {
    allNum += scatter[i];
  }

  printf("number of Characters: %llu\n", allNum);
  for(i = 0; i < HUFFMAN_N_BYTES; i++)
  {
    if(scatter[i]){ // only if it even occured once
      printf("\t%#0.2x: %c: %u\n", i, isprint(i)?i:' ', scatter[i]);
    }
  }

  return EXIT_SUCCESS;
}

int HUFFFMAN_printTree(HUFFMAN_tree_t const tree)
{
  unsigned int nNodes = tree.nNodes;
  if(NULL == tree.nodes) return -1;

  puts("\t0\t1");
  puts("----------");

  for(int i = 0; i < nNodes; i++)
  {
    printf("%d\t%hx\t%hx\n", i, tree.nodes[i][0], tree.nodes[i][1]);
  }

  return EXIT_SUCCESS;
}

/*-------------------- P R I V A T E   F U N C T I O N S ---------------------*/
static _Bool getLeastPropableNodes(HUFFMAN_workNode_t const workTree[],
                                  int nNodes,
                                  unsigned int * node1,
                                  unsigned int * node2)
// return true if two nodes with quantity > 0 are found
{
  unsigned int currentWn;
  int wn1 = -1;
  int wn2 = -1; // the smaller of the 2

  assert(NULL != workTree);
  assert(NULL != node1);
  assert(NULL != node2);
  assert(nNodes >= 2);

  // find first legal node
  for(int i = 0; i < nNodes; i++)
  {
    if(0 != workTree[i].quantity){
      wn1 = i;
      break;
    }
  }

  // find 2nd legal node
  // find first legal node
  for(int i = wn1 + 1; i < nNodes; i++)
  {
    if(0 != workTree[i].quantity){
      wn2 = i;
      break;
    }
  }

  // ensure wn2 is not smaller than wn1
  if(workTree[wn1].quantity < workTree[wn2].quantity)
  { // switch wn1 and wn2
    currentWn = wn1;
    wn1 = wn2;
    wn2 = currentWn;
  }

  for(int i = wn2 + 1; i < nNodes; i++)
  {
    currentWn = i;
    if((workTree[currentWn].quantity > 0) &&
        (workTree[currentWn].quantity < workTree[wn1].quantity))
    {
      wn1 = currentWn;
      if(workTree[wn1].quantity < workTree[wn2].quantity)
      { // switch wn1 and wn2
        currentWn = wn1;
        wn1 = wn2;
        wn2 = currentWn;
      }
    }
  }

  *node1 = wn1;
  *node2 = wn2;

  return ((wn1 >= 0) && (wn2 >= 0));
}

static void HUFFMAN_createEncoderList(HUFFMAN_tree_t const * tree,
                                      HUFFMAN_encoderList_t * inverseList,
                                      HUFFMAN_encoder_t * encoder)
{
  assert(NULL != tree);
  assert(NULL != inverseList);

  memset(inverseList, 0, sizeof(*inverseList));
  memset(encoder, 0, sizeof(*encoder));
  HUFFMAN_createEncodeEntries(tree,0, encoder, inverseList);
}

void HUFFMAN_createEncodeEntries(HUFFMAN_tree_t const * const tree,
                                 int currentNode,
                                 HUFFMAN_encoder_t * const currentCode,
                                 HUFFMAN_encoderList_t * const list)
{
  HUFFMAN_node_t const * pNode = tree->nodes[currentNode];
  HUFFMAN_encoder_t * pEncoder;
  list->nBytes++;
  // handle the 0
  if(IS_LEAVE(pNode[0]))
  {
    pEncoder = (*list)[GET_NODE(pNode[0])];
    pEncoder->nBytes = list->nBytes;
    memcpy(pEncoder->code, list->code, sizeof(pEncoder->code));
  }else
  {
    HUFFMAN_createEncodeEntries(tree, pNode[0], currentCode, list);
  }
  // currentCode should be 0 at the current State
  // handle the 1
  if(IS_LEAVE(pNode[1]))
    {
      pEncoder = (*list)[GET_NODE(pNode[1])];
      pEncoder->nBytes = list->nBytes;
      memcpy(pEncoder->code, list->code, sizeof(pEncoder->code));
    }else
    {
      //TODO: add 1 to the currentCode
      HUFFMAN_createEncodeEntries(tree, pNode[1], currentCode, list);
    }
}

