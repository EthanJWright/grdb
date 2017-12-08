#include "graph.h"
#include <stdio.h>


/* Place the code for your Dijkstra implementation in this file */
attribute_t 
find_int_attr(attribute_t a){
  for(attribute_t attribute = a; attribute != NULL; attribute = attribute->next){
    if(attribute->bt == 4){
      return attribute;
    }
  }
  return NULL;
}

int
component_sssp(
        component_t c,
        vertexid_t v1,
        vertexid_t v2,
        int *n,
        int *total_weight,
        vertexid_t **path)
{	

	/*
	 * Figure out which attribute in the component edges schema you will
	 * use for your weight function
	 */
//    attribute_t result = find_int_attr(c->se->attrlist);
    printf("Int value: \n");
//    printf("%s\n",result->name);
    printf("Ending now\n");

	/*
	 * Execute Dijkstra on the attribute you found for the specified
	 * component
	 */



	/* Change this as needed */
	return (-1);
}
