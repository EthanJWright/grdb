#include "graph.h"
#include "cli.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


void
set_file_desc(component_t c, vertexid_t v1){
  // Read in the vertecies from file system
  char s[BUFSIZE];
  /* Set the value of a vertex tuple */
  struct vertex v;
  vertex_init(&v);
  v.id = v1;

  /* Open the vertex file */
  memset(s, 0, BUFSIZE);
  sprintf(s, "%s/%d/%d/v", grdbdir, gno, cno);
  c->vfd = open(s, O_RDWR | O_CREAT, 0644);
  if (c->vfd < 0) {
      printf("Open vertex file failed\n");
      return;
  }
  /* Open the edge file */
  memset(s, 0, BUFSIZE);
  sprintf(s, "%s/%d/%d/e", grdbdir, gno, cno);
  printf("cli_graph_tuple: open edge file %s\n", s);
  c->efd = open(s, O_RDWR | O_CREAT, 0644);
  if (c->efd < 0) {
	  printf("Find edge ids failed\n");
	  return;
  }
}

attribute_t 
find_int_attr(attribute_t a){
  for(attribute_t attribute = a; attribute != NULL; attribute = attribute->next){
    if(attribute->bt == 4){
      return attribute;
    }
  }
  return NULL;
}

/* get number of vertices in component c */
int
get_vertices_total(component_t c){
    ssize_t extra, len;
    char* buffer;
    off_t offset;
    int readlen, count;
    
    if (c->sv == NULL){
      extra = 0;
    }else{
      extra = schema_size(c->sv);
    }
    
    readlen = sizeof(vertexid_t) + extra;
    buffer = malloc(readlen); 
    count = 0;

    // See how many vertices are in the file
    while(1){
      lseek(c->vfd, offset, SEEK_SET);
      len = read(c->vfd, buffer, readlen);
      if(len == -1 || len == 0){
        break;
      }
      offset += readlen;
      count++;
    }

    free(buffer);
    return count;
}

void
load_vertices(component_t c, vertexid_t *vertices){
    ssize_t extra, len;
    int readlen;
    char* buffer;
    off_t offset;
    vertexid_t v;

    if (c->sv == NULL){
      extra = 0;
    }else{
      extra = schema_size(c->sv);
    }
    
    readlen = sizeof(vertexid_t) + extra;
    buffer= malloc(readlen);

    int index = 0;
    while(1){
      lseek(c->vfd, offset, SEEK_SET);
      len = read(c->vfd,buffer, readlen);
      if (len <= 0){
          break;
      }  
      v = *((vertexid_t *)buffer);
      vertices[index] = v;

      offset += readlen;
      index++;
    }
    free(buffer);
}


int
get_weight(component_t c, vertexid_t v1, vertexid_t v2, char* attr_name){
  struct edge e;
  edge_t new_edge;
  edge_init(&e);
  edge_set_vertices(&e, v1, v2);

  new_edge = component_find_edge_by_ids(c, &e);

  if(new_edge == NULL){
    printf("v1 is: %llu v2 is: %llu \n", v1, v2);
    return 100;
  }

 int offset = tuple_get_offset(new_edge->tuple, attr_name);
  int weight = tuple_get_int(new_edge->tuple->buf + offset);
  return weight;
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

  /* Set Vertex File Desc */
  set_file_desc(c, v1);

  /* Get total vertices */
  int total = get_vertices_total(c);
  printf("TOTAL IS : %i\n",total);

  vertexid_t *vertices;
  vertices = malloc(total * sizeof(vertexid_t));
  load_vertices(c, vertices);

  for(int i = 0; i < total; i++){
    printf("vertex id is: %llu\n", vertices[i]);
  }

  attribute_t result = find_int_attr(c->se->attrlist);
  char* attr_name = result->name;
  printf("%s\n",attr_name);

  vertexid_t first = vertices[0];
  vertexid_t second = vertices[2];
  int weight = get_weight(c, first, second, attr_name);
  printf("Weight of first edge is: %i\n", weight);



     /*
	 * Figure out which attribute in the component edges schema you will
	 * use for your weight function
	 */



	/*
	 * Execute Dijkstra on the attribute you found for the specified
	 * component
	 */

  vertexid_t start = v1;
  vertexid_t end = v2;

  if(start != vertices[0]){
    printf("Start must be first vertex\n");
    return -1;
  }else if(start == end){
    printf("They are the same... efficient path I guess.\n");
    return -1;
  }else{
    // Code for running

  }


	/* Change this as needed */
	return (-1);
}


