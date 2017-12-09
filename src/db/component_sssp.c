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

  /* Get total vert */
  int total = get_vertices_total(c);
  printf("TOTAL IS : %i\n",total);

  vertexid_t *vertices;
  vertices = malloc(total * sizeof(vertexid_t));
  load_vertices(c, vertices);
  for(int i = 0; i < total; i++){
    printf("vertex id is: %llu\n", vertices[i]);
}


     /*
	 * Figure out which attribute in the component edges schema you will
	 * use for your weight function
	 */

  attribute_t result = find_int_attr(c->se->attrlist);
  char* attr_name = result->name;
  printf("%s\n",attr_name);

	/*
	 * Execute Dijkstra on the attribute you found for the specified
	 * component
	 */



	/* Change this as needed */
	return (-1);
}


