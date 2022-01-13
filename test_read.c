#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include "jrb.h"
#include "jval.h"
#define SIZE 250
#define INFINITE_VALUE 1000
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
char* get_v(graph g, int id);
double get_edge_value(graph g, int v1, int v2);
int get_adjacent(graph g, int v, int out[]);
//
void add_bus_v(graph g, int id, int sub_id, char* name);//
//
void readfile(graph g, char* filename, int* n);
//
int check(graph g, int s, int t);
int extractmin(double dis[], int visited[]);
double shortest_path(graph g, int s, int d, int path[]);
void track_path(graph g, int path[], int s, int d);
// track path only print out the route from src to des 
// but not include src and des
int check_vertex(graph g, char* name, int* id);//return 0 when vertex existed
// return 1 otherwise
void busline(graph g, int id);
int go_through(graph g, int id, int out[]);
//
void menu();
int main(){
    menu();
    graph g = crt_graph();
    int point = 0;
    readfile(g, "data.txt", &point);
    while(1){
        int cmd;
        printf("enter command: ");
        scanf("%d", &cmd);
        if(cmd == 1){
            printf("enter bus id: ");
            int bus_truck; scanf("%d", &bus_truck);
            busline(g, bus_truck);
        }else if(cmd == 2){
            int out[10];
            printf("enter station id: ");
            int station; scanf("%d", &station);
            int l = go_through(g, station, out);
            if(l != 0){
                for(int i = 0; i < l; i ++){
                    printf("%d \n", out[i]);
                }
            }else{
                printf("not found!\n");
            }
        }else if(cmd == 3){
            printf("enter source id: ");
            int src; scanf("%d", &src);
            printf("enter destination id: ");
            int des; scanf("%d", &des);

            int* path = (int*)malloc(sizeof(int)*(point+1));
            memset(path, -1, sizeof(path));
            double dis = shortest_path(g, src, des, path);
            
            printf("shortest distance: ");
            printf("%0.2lf\n", dis);
            track_path(g, path, src, des);
        }else{break;}
    }
    return 0;
}
// functions
void menu(){
    printf("MINI PROJECT 2\n");
    printf("1. Show busline of a bus\n");
    printf("2. Show buses at a bus station\n");
    printf("3. Show the shortest path \n");
    printf("4. Exit\n");
}
void track_path(graph g, int path[], int s, int d){
    if(path[d] == s){
        return ;
    }
    d = path[d];
    track_path(g, path, s, d);
    printf("%s\n", get_v(g, d));
    int* out = (int*)malloc(sizeof(int)*SIZE);
    int l = go_through(g, d, out);
    for(int i = 0;i < l; i ++){
        if(i != l-1)
        printf("%d - ", out[i]);
        else
        printf("%d \n", out[i]);
    }
}
int extractmin(double dis[], int visited[]){
    int n = SIZE;
    double min = INFINITE_VALUE;
    int index = 0;
    for(int i = 0; i < n; i++){
        if(dis[i] < min && visited[i] == -1){
            min = dis[i];
            index = i;
        }
    }
    return index;
}
int check(graph g, int s, int t){
    JRB node2;
    int exist1 = 0;
    int exist2 = 0;
    jrb_traverse(node2, g.v){
        if(jval_i(node2->key) == s){ exist1 = 1;}
        if(jval_i(node2->key) == t){ exist2 = 1;}
    }
    if(exist1*exist2 == 0){
        return 0;
    }
    return 1;
}
double shortest_path(graph g, int s, int d, int path[]){
    int flag = check(g, s, d);
    if(flag == 0){
        return INFINITE_VALUE;
    }
    double dis[SIZE]; 
    for(int i = 0; i < SIZE ;i ++){
        dis[i] = INFINITE_VALUE;
    } dis[s] = 0;

    path[s] = s;

    int visited[SIZE]; 
    memset(visited, -1, sizeof(visited));

    int pq[SIZE]; memset(pq, -1, sizeof(pq));

    JRB node; int n = 0; 
    jrb_traverse(node, g.v){
        pq[n++] = jval_i(node->key);
    }
    int k = n;
    while(n != 0){
        int u = extractmin(dis, visited);
        visited[u] = 1;
        pq[u] = -1; n --;
        int adj[SIZE]; int l = get_adjacent(g, u, adj);
        for(int i = 0; i < l; i++){
            int v = adj[i];
            double d_uv = dis[u] + get_edge_value(g, u, v);
            if(dis[v] > d_uv){
                dis[v] = d_uv;
                path[v] = u;
            }
        }
    }
    return dis[d];
}
int go_through(graph g, int id, int out[]){
    int total = 0;
    JRB node;
    jrb_traverse(node, g.busline){
        JRB sub_node;
        JRB subtree = (JRB)jval_v(node->val);
        jrb_traverse(sub_node, subtree){
            if(jval_i(sub_node->key) == id){
                out[total++] = jval_i(node->key);
            }
        }
    }
    return total;
}
void busline(graph g, int id){
    JRB node = jrb_find_int(g.busline, id);
    if(node != NULL){
        JRB subtree = (JRB)jval_v(node->val);
        JRB subnode;
        jrb_traverse(subnode, subtree){
            printf("%s - %d\n", jval_s(subnode->val), jval_i(subnode->key));
        }
    }else {
        printf("not found!\n");
    }
    printf("\n");
}
void readfile(graph g, char* filename, int* n){
    FILE* file = fopen(filename, "r");
    double dis = 0;
    int station_number = 0; // number of busline stop
    int car_id;
    int v_id;
    int v_id1;
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
            if(check_vertex(g, tmp, &v_id)){
                add_v(g, station_number, tmp);
                add_bus_v(g, car_id, station_number, tmp);
                v_id = station_number;
                station_number ++;
            }else{
                add_bus_v(g, car_id, v_id, tmp);
            }
            // read next busline stop    
            c = getc(file);
            char* tmp1 = (char*)malloc(SIZE*sizeof(char));
            fgets(tmp1, SIZE, file);
            tmp1[strlen(tmp1)-2] = '\0';
            if(check_vertex(g, tmp1, &v_id1)){
                add_v(g, station_number, tmp1);
                add_bus_v(g, car_id, station_number, tmp1);
                v_id1 = station_number;
                station_number ++;
            }else{ 
                add_bus_v(g, car_id, v_id1, tmp1);
            }
        }else if(c == '-'){
            fscanf(file, "%lf", &dis);
            add_e(g, v_id, v_id1, dis);
        }else continue;
    }
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
int check_vertex(graph g, char* name, int* id){
    JRB node;
    jrb_traverse(node, g.v){
        if(strcmp(jval_s(node->val), name) == 0){
            *id = jval_i(node->key);
            return 0;
        }
    }
    return 1;
}
void add_v(graph g, int id, char* name){
    JRB node = jrb_find_int(g.v, id);
    if(node == NULL){
        jrb_insert_int(g.v, id, new_jval_s(name));
    }else if(node != NULL) return;
}
char* get_v(graph g, int id){
    JRB node = jrb_find_int(g.v, id);
    if(node != NULL){
        return jval_s(node->val);
    }else return NULL;
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
double get_edge_value(graph g, int v1, int v2){
    JRB node = jrb_find_int(g.e, v1);
    if(node == NULL) return INFINITE_VALUE;
    else{
        JRB subtree = (JRB)jval_v(node->val);
        JRB value = jrb_find_int(subtree, v2);
        if(value == NULL) return INFINITE_VALUE;
        return jval_d(value->val);
    }
}
int get_adjacent(graph g, int v, int out[]){
    JRB node = jrb_find_int(g.e, v);
    if(node == NULL) return 0;
    else{
        JRB tree = (JRB)jval_v(node->val);
        JRB tmp;
        int i = 0;
        jrb_traverse(tmp, tree){
            out[i] = jval_i(tmp->key);
            i++;
        }return i;
    }
}
