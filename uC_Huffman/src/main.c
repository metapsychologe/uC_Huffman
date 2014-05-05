/* file:      main.c
 * brief: 		main entry point
 * author: 		alex
 * date:			27.04.2014
 * label:			MAIN
 */


/*--------------------- P R I V A T E   I N C L U D E S ----------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "huffman.h"

/*---------------------- P R I V A T E   D E F I N E S -----------------------*/


/*--------------------- P R I V A T E   T Y P E D E F S ----------------------*/


/*----------- P R I V A T E   F U N C T I O N P R O T O T Y P E S ------------*/


/*-------------------- P R I V A T E   V A R I A B L E S ---------------------*/
char const testString[] = "Hello World!";

/*--------------------- P U B L I C   V A R I A B L E S ----------------------*/


/*--------------------- P U B L I C   F U N C T I O N S ----------------------*/
int main(int argc, char * argv[])
{
  HUFFMAN_scatter_t scatter;
  HUFFMAN_tree_t tree;
  HUFFMAN_encoderList_t encoders;

  if(HUFFMAN_clear(scatter)) return EXIT_FAILURE;
//  for(int i = 1; i < argc; i++)
//  { // over all inputs
//    if(HUFFMAN_addFile(scatter, argv[i])) return EXIT_FAILURE;
//  }

  if(HUFFMAN_addMemory(scatter, testString, strlen(testString)))
    return EXIT_FAILURE;

  if(HUFFMAN_printScatter(scatter)) return  EXIT_FAILURE;

  fflush(stdout);

  if(HUFFMAN_crateTree(scatter, &tree, &encoders)) return  EXIT_FAILURE;

  if(HUFFFMAN_printTree(tree)) return  EXIT_FAILURE;



//  puts("press return to finish");
//  fflush(stdout);
//  fflush(stdin);
//  getchar();

  return EXIT_SUCCESS;
}

/*-------------------- P R I V A T E   F U N C T I O N S ---------------------*/

