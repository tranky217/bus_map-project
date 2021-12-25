#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include "jrb.h"
#include "jval.h"
#define SIZE 200
// define structure
typedef struct{
    JRB e;
    JRB v;
    JRB busline;
}graph;
// prototype
graph crt_graph(); // 
void add_v(graph g, int id, char* name); //
void add_e(graph g, int v1, int v2, double w); //
//
void add_bus_v(graph g, int id, int sub_id, char* name);//
//
void readfile(graph g, char* filename, int* n);
int main(){
    graph g = crt_graph();
    int n = 0;
    readfile(g, "data.txt", &n);
    
    return 0;
}
// functions 
void readfile(graph g, char* filename, int* n){
    FILE* file = fopen(filename, "r");
    double dis = 0;
    int station_number = 0; // number of busline stop
    int car_id;
    while(!feof(file)){
        char c = getc(file);
        if(c == '*'){
            int new_bus;
            fscanf(file, "%d\n", &new_bus);
            car_id = new_bus;
        }else if(c == '#'){
            char* tmp = (char*)malloc(SIZE*sizeof(char));
            fgets(tmp, SIZE, file);
            tmp[strlen(tmp)-2] = '\0';
            add_bus_v(g, car_id, station_number, tmp);
            add_v(g, station_number, tmp);
            station_number ++;
            // read next busline stop    
            c = getc(file);
            char* tmp1 = (char*)malloc(SIZE*sizeof(char));
            fgets(tmp1, SIZE, file);
            tmp1[strlen(tmp1)-2] = '\0';
            add_bus_v(g, car_id, station_number, tmp1);    
            add_v(g, station_number, tmp1);
        }else if(c == '-'){
            fscanf(file, "%lf", &dis);
            add_e(g, station_number - 1, station_number, dis);
        }else continue;
    }
    printf("test 12\n");
    fclose(file);
    *n = station_number;
}
graph crt_graph(){
    graph g;
    g.e = make_jrb();
    g.v = make_jrb();
    g.busline = make_jrb();
    return g;
}
void add_bus_v(graph g, int id, int sub_id, char* name){
    JRB tmp_node = jrb_find_int(g.busline, id);
    if(tmp_node != NULL){
        JRB subtree = (JRB)jval_v(tmp_node->val);
        JRB sub_node = jrb_find_int(subtree, sub_id);
        if(sub_node == NULL)
            jrb_insert_int(subtree, sub_id, new_jval_s(name));
        else return;
    }else {
        JRB subtree = make_jrb();
        jrb_insert_int(g.busline, id, new_jval_v(subtree));
        jrb_insert_int(subtree, sub_id, new_jval_s(name));
    }
}
void add_v(graph g, int id, char* name){
    JRB node = jrb_find_int(g.v, id);
    if(node == NULL){
        jrb_insert_int(g.v, id, new_jval_s(name));
    }else if(node != NULL) return;
}
void add_e(graph g, int v1, int v2, double w){
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
    if(node2 == NULL){
        JRB tree2 = make_jrb();
        jrb_insert_int(g.e, v2, new_jval_v(tree2));
        jrb_insert_int(tree2, v1, new_jval_d(w));
    }else{
        JRB tree2 = (JRB)jval_v(node2->val);
        jrb_insert_int(tree2, v1, new_jval_d(w));
    }
}