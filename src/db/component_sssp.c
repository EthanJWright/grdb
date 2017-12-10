#include "graph.h"
#include "cli.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>


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
#if _DEBUG
  printf("cli_graph_tuple: open edge file %s\n", s);
#endif
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
    return INT_MAX;
  }

 int offset = tuple_get_offset(new_edge->tuple, attr_name);
  int weight = tuple_get_int(new_edge->tuple->buf + offset);
  return weight;
}

int
num_neighbor(component_t c, 
    vertexid_t v1, 
    vertexid_t *vertices, 
    int total, 
    char *attr_name){
  int total_edges = 0;
  for(int i = 0; i < total; i++){
    int weight = get_weight(c, v1, vertices[i], attr_name);
    if(weight != INT_MAX){
      total_edges++;
    }
  }
  return total_edges;
}



/* This is an utter crap method of gettig the vertex ids connected to the
 * vertex. I am not a good C programmer. This is an exercise in inefficiency. */
vertexid_t*
get_edges(component_t c, 
    vertexid_t v1, 
    vertexid_t *vertices, 
    int total, 
    char *attr_name)
{
  int total_edges = 0;
  for(int i = 0; i < total; i++){
    int weight = get_weight(c, v1, vertices[i], attr_name);
    if(weight != INT_MAX){
      total_edges++;
    }
  }
  vertexid_t *edges = malloc(total_edges * sizeof(vertexid_t));
  int added = 0;
  for(int i = 0; i < total; i++){
    int weight = get_weight(c, v1, vertices[i], attr_name);
    if(weight != INT_MAX){
      edges[added] = vertices[i];
      added ++;
    }
  }
  return edges;
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

  vertexid_t *vertices;
  vertexid_t *visited;
  vertexid_t *unvisited;
  int *costs;
  vertices = malloc(total * sizeof(vertexid_t));
  visited = malloc(total * sizeof(vertexid_t));
  unvisited = malloc(total * sizeof(vertexid_t));
  costs = malloc(total * sizeof(int));

  load_vertices(c, vertices);

  // set visited, unvisited, and costs
  visited[0] = vertices[0];
  for(int i = 1; i < total; i++){
    unvisited[i-1] = vertices[i];
  }
  for(int i = 0; i < total; i++){
    costs[i] = INT_MAX;
  }

  attribute_t result = find_int_attr(c->se->attrlist);
  char* attr_name = result->name;

  vertexid_t first = vertices[0];
  vertexid_t second = vertices[2];
  int weight = get_weight(c, first, second, attr_name);



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
    // Set initial cost
    costs[start-1] = 0;
    for(int i = 0; i < total; i++){
      vertexid_t current = vertices[i];
      vertexid_t *connected = get_edges(c, current, vertices, total, attr_name);
      int total_neighbors = num_neighbor(c, current, vertices,total, attr_name);
      for(int j = 0; j < total_neighbors; j++){
        vertexid_t neighbor = connected[j];
        int weight = get_weight(c, current, neighbor, attr_name);
        if(costs[current - 1] + weight < costs[neighbor - 1]){
          costs[neighbor - 1] = costs[current - 1] + weight;
        }
      }
    }
    printf("Cost between %i and %i is %i.\n", start, end, costs[end-1]);
    return(costs[end-1]);
  }


	/* Change this as needed */
	return (-1);
}

