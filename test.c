#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "jrb.h"
#include "jval.h"
#define SIZE 200
#define INFINITE_VALUE 1000
// define structure
typedef struct
{
    JRB e;
    JRB v;
    JRB busline;
} graph;

typedef struct
{
    int parent;
    double distance;
    int visited;
} node;

// prototype
graph crt_graph();                             //
void add_v(graph g, int id, char *name);       //
void add_e(graph g, int v1, int v2, double w); //
char *get_v(graph g, int id);
double get_edge_value(graph g, int v1, int v2);
int get_adjacent(graph g, int v, int out[]);
//
void add_bus_v(graph g, int id, int sub_id, char *name); //
//
void readfile(graph g, char *filename, int *n);
//

int findShortestNode(graph g, node nodes[]);
double shortestPath(graph g, int sourceVertex, int destinationVertex, int path[], int *length);

// int extractmin(double dis[], int visited[], int total);
// double shortest_path(graph g, int s, int d, int path[], int total);
// void track_path(graph g, int path[], int s, int d);
// // track path only print out the route from src to des
// // but not include src and des
// int check_vertex(graph g, char* name, int* id);//return 0 when vertex existed
// return 1 otherwise
int main()
{
    graph g = crt_graph();
    int n = 0; // total number of vertices
    readfile(g, "data.txt", &n);
    //
    JRB node;
    printf("%d \n", n);
    jrb_traverse(node, g.v)
    {
        printf("%d - %s\n", jval_i(node->key), jval_s(node->val));
    }
    int path[200];
    int length = 0;
    double weight = shortestPath(g, 48, 14, path, &length);
    for (int i = --length; i >= 0; i--)
    {
        printf("\n%d: %s", path[i], get_v(g, path[i]));
    }
    printf("\n%lf", weight);
    // track_path(g, path, 1, 9);
    // 19
    printf("\n");
    int output[20];
    int k;
    getBusline(g, 26);
    k = getStation(g, 37, output);
    for (i = 0; i < k; i++)
    {
        printf("%d ", output[i]);
    }
    printf("\n");
    return 0;
}
// functions
void readfile(graph g, char *filename, int *n)
{
    FILE *file = fopen(filename, "r");
    double dis = 0;
    int station_number = 0; // number of busline stop
    int car_id;
    int v_id;
    int v_id1;
    while (!feof(file))
    {
        char c = getc(file);
        if (c == '*')
        {
            int new_bus;
            fscanf(file, "%d\n", &new_bus);
            car_id = new_bus;
        }
        else if (c == '#')
        {
            char *tmp = (char *)malloc(SIZE * sizeof(char));
            fgets(tmp, SIZE, file);
            tmp[strlen(tmp) - 2] = '\0';
            // v_id = -1;
            if (check_vertex(g, tmp, &v_id))
            {
                station_number++;
                add_bus_v(g, car_id, station_number, tmp);
                add_v(g, station_number, tmp);
                v_id = station_number;
            }
            // read next busline stop
            c = getc(file);
            char *tmp1 = (char *)malloc(SIZE * sizeof(char));
            fgets(tmp1, SIZE, file);
            tmp1[strlen(tmp1) - 2] = '\0';
            // v_id1 = -1;
            if (check_vertex(g, tmp1, &v_id1))
            {
                station_number++;
                v_id1 = station_number;
                add_bus_v(g, car_id, station_number, tmp1);
                add_v(g, station_number, tmp1);
            }
        }
        else if (c == '-')
        {
            fscanf(file, "%lf", &dis);
            add_e(g, v_id, v_id1, dis);
        }
        else
            continue;
    }
    fclose(file);
    *n = station_number;
}
graph crt_graph()
{
    graph g;
    g.e = make_jrb();
    g.v = make_jrb();
    g.busline = make_jrb();
    return g;
}
void add_bus_v(graph g, int id, int sub_id, char *name)
{
    JRB tmp_node = jrb_find_int(g.busline, id);
    if (tmp_node != NULL)
    {
        JRB subtree = (JRB)jval_v(tmp_node->val);
        JRB sub_node = jrb_find_int(subtree, sub_id);
        if (sub_node == NULL)
            jrb_insert_int(subtree, sub_id, new_jval_s(name));
        else
            return;
    }
    else
    {
        JRB subtree = make_jrb();
        jrb_insert_int(g.busline, id, new_jval_v(subtree));
        jrb_insert_int(subtree, sub_id, new_jval_s(name));
    }
//
void getBusline(graph g, int id)
{
    JRB node;
    node = jrb_find_int(g.busline, id);
    if (node == NULL)
    {
        printf("Not found this Bus line !\n");
    }
    else
    {
        JRB tree = (JRB)jval_v(node->val);
        JRB temp;
        jrb_traverse(temp, tree)
        {
            printf("%s-", jval_s(temp->val));
        }
        printf("\n");
    }
}
int getStation(graph g, int id, int *output)
{
    JRB node;
    int i = 0;
    jrb_traverse(node, g.busline)
    {
        JRB tree = (JRB)jval_v(node->val);
        JRB temp;
        jrb_traverse(temp, tree)
        {
            if (jval_i(temp->key) == id)
            {
                output[i] = jval_i(node->key);
                i++;
            }
        }
    }
    return i;
}
int check_vertex(graph g, char *name, int *id)
{
    JRB node;
    jrb_traverse(node, g.v)
    {
        if (strcmp(jval_s(node->val), name) == 0)
        {
            *id = jval_i(node->key);
            return 0;
        }
    }
    return 1;
}
void add_v(graph g, int id, char *name)
{
    JRB node = jrb_find_int(g.v, id);
    if (node == NULL)
    {
        jrb_insert_int(g.v, id, new_jval_s(name));
    }
    else if (node != NULL)
        return;
}
char *get_v(graph g, int id)
{
    JRB node = jrb_find_int(g.v, id);
    if (node != NULL)
    {
        return jval_s(node->val);
    }
    else
        return NULL;
}
void add_e(graph g, int v1, int v2, double w)
{
    JRB node = jrb_find_int(g.e, v1);
    if (node == NULL)
    {
        JRB tree = make_jrb();
        jrb_insert_int(g.e, v1, new_jval_v(tree));
        jrb_insert_int(tree, v2, new_jval_d(w));
    }
    else
    {
        JRB tree = (JRB)jval_v(node->val);
        jrb_insert_int(tree, v2, new_jval_d(w));
    }
    JRB node2 = jrb_find_int(g.e, v2);
    if (node2 == NULL)
    {
        JRB tree2 = make_jrb();
        jrb_insert_int(g.e, v2, new_jval_v(tree2));
        jrb_insert_int(tree2, v1, new_jval_d(w));
    }
    else
    {
        JRB tree2 = (JRB)jval_v(node2->val);
        jrb_insert_int(tree2, v1, new_jval_d(w));
    }
}
double get_edge_value(graph g, int v1, int v2)
{
    JRB node = jrb_find_int(g.e, v1);
    if (node == NULL)
        return INFINITE_VALUE;
    else
    {
        JRB subtree = (JRB)jval_v(node->val);
        JRB value = jrb_find_int(subtree, v2);
        if (value == NULL)
            return INFINITE_VALUE;
        return jval_d(value->val);
    }
}
int get_adjacent(graph g, int v, int out[])
{
    JRB node = jrb_find_int(g.e, v);
    if (node == NULL)
        return 0;
    else
    {
        JRB tree = (JRB)jval_v(node->val);
        JRB tmp;
        int i = 0;
        jrb_traverse(tmp, tree)
        {
            out[i] = jval_i(tmp->key);
            i++;
        }
        return i;
    }
}

int findShortestNode(graph g, node nodes[])
{
    double shortestDistance = INFINITE_VALUE;
    int shortestNode = -1;
    JRB nodeTemp;
    jrb_traverse(nodeTemp, g.v)
    {
        if (nodes[nodeTemp->key.i].distance < shortestDistance && !nodes[nodeTemp->key.i].visited)
        {
            shortestDistance = nodes[nodeTemp->key.i].distance;
            shortestNode = nodeTemp->key.i;
        }
    }
    return shortestNode;
}

double shortestPath(graph g, int sourceVertex, int destinationVertex, int path[], int *length)
{
    node nodes[200];
    JRB nodeTemp;
    jrb_traverse(nodeTemp, g.v)
    {
        if (nodeTemp->key.i == sourceVertex)
            nodes[sourceVertex].distance = 0;
        else
            nodes[nodeTemp->key.i].distance = INFINITE_VALUE;
        nodes[nodeTemp->key.i].parent = -1;
        nodes[nodeTemp->key.i].visited = 0;
    }
    int indexOfShortestNode = findShortestNode(g, nodes);
    while (indexOfShortestNode != -1)
    {
        double distance = nodes[indexOfShortestNode].distance;
        int neighbors[100];
        int lengthOfNeighbors = get_adjacent(g, indexOfShortestNode, neighbors);
        for (int i = 0; i < lengthOfNeighbors; i++)
        {
            if (nodes[neighbors[i]].distance > distance + get_edge_value(g, indexOfShortestNode, neighbors[i]))
            {
                nodes[neighbors[i]].distance = distance + get_edge_value(g, indexOfShortestNode, neighbors[i]);
                nodes[neighbors[i]].parent = indexOfShortestNode;
            }
        }
        nodes[indexOfShortestNode].visited = 1;
        indexOfShortestNode = findShortestNode(g, nodes);
    }
    int temp = destinationVertex;
    path[(*length)++] = destinationVertex;
    while (nodes[temp].parent != -1)
    {
        path[(*length)++] = nodes[temp].parent;
        temp = nodes[temp].parent;
    }
    return nodes[destinationVertex].distance;
}
