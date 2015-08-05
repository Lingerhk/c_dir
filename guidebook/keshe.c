/*
 *  数 据 结 构 课 程 设 计
 * 
 *     $ 校园导游系统 $
 *
 *
 *  @linger <lingerhk@gmail.com>
 *  @2014.12.8 xiyou
 *  @filename: keshe.c
 *  @function: 带权无向图
 */


//邻接表表示的有向图

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAXVEX 20
#define MAX_NAME 30
#define MAX_INTR 100
#define MAX_KEY 40
#define INF 32867
#define isLetter(a)  ((((a)>='a')&&((a)<='z')) || (((a)>='A')&&((a)<='Z')))

int road[MAXVEX];
int visited[MAXVEX];
    
// 邻接表中表对应的链表的顶点
typedef struct ArcNode
{
    int adjvex;                   // 该边所指向的顶点的位置
    int weight;
    struct ArcNode *next;   // 指向下一条弧的指针
} ArcNode;

// 邻接表中表的顶点
typedef struct VertexNode
{
    char data;              // 顶点信息
    char addr_name[MAX_NAME];
    char addr_intr[MAX_INTR];
    char addr_key[MAX_KEY];
    ArcNode *head;      // 指向第一条依附该顶点的弧
} VertexNode;

// 邻接表
typedef struct AdjList
{
    int vexnum;             // 图的顶点的数目
    int arcnum;             // 图的边的数目
    VertexNode vex[MAXVEX];
} AdjList;

// 保存地点结构体数组
struct addrs {
    char data;
    char addr_name[MAX_NAME];
    char addr_intr[MAX_INTR];
    char addr_key[MAX_KEY];
} addrs[MAXVEX];

// 保存路径结构体数组
struct ways {
    char src;
    char des;
    int len;
} way[MAXVEX *(MAXVEX-1)];    


//返回ch在matrix矩阵中的位置
int localvex(AdjList G, char ch)
{
    int i;
    for(i = 0; i < G.vexnum; i ++)
        if(G.vex[i].data == ch)
            return i;
    return -1;
}

//读取一个输入字符
char read_char()
{
    char ch;

    do {
        ch = getchar();
    } while(!isLetter(ch));

    return ch;
}

//将node链接到list的末尾
void link_last(ArcNode *list, ArcNode *node)
{
    assert(list != NULL);
    ArcNode *p = list;

    while(p->next)
        p = p->next;
    p->next = node;
}


//打印所有地点信息
void print_addr(AdjList G)
{
    int j;

    printf("\nYou have inputed all the \033[33m%d\033[0m address:\n\n",G.vexnum);
    for(j = 0; j < G.vexnum; j ++) {
        printf("Address of %c:\n",G.vex[j].data);
        printf("\tAddress Name: %s\n",G.vex[j].addr_name);
        printf("\tIntroduction: %s\n",G.vex[j].addr_intr);
        printf("\tKey  words:   %s\n\n",G.vex[j].addr_key);
    }
}

//查询一个地点信息
void print_one_addr(AdjList G)
{
    int i;
    int k = 0;
    char name[MAX_NAME];
    printf("Give an address name to find(under %d chars):\n\033[34m>>\033[0m ",MAX_NAME);
    scanf("%s",name);
    for(i = 0; i < G.vexnum; i ++) {
        if(strcmp(name,G.vex[i].addr_name) == 0) {
            printf("Address of %c:\n",G.vex[i].data);
            printf("\tAddress Name: %s\n",G.vex[i].addr_name);
            printf("\tIntroduction: %s\n",G.vex[i].addr_intr);
            printf("\tKey  words:   %s\n",G.vex[i].addr_key);
        } else {
            k ++;
        }
    }
    if(k == G.vexnum)
        printf("Sorry, there is no address find!\n");
}

//显示所有路径
void print_path(AdjList G)
{
    int i;
    int p1,p2;

    printf("There are all the paths:\n");
    for(i = 0; i < G.arcnum; i ++) {  
        p1 = localvex(G,way[i].src);
        p2 = localvex(G,way[i].des);
        printf("%d > %s == %d/km ==> %s\n", i+1, G.vex[p1].addr_name,way[i].len,G.vex[p2].addr_name);
    }
}


//从文件中读取地点
int read_address()
{   
    int i;
    FILE *addre;

    addre = fopen("db_address.txt","rt");
    
    i = 0;
    while((fread(&addrs[i], sizeof(struct addrs), 1, addre)) != 0) {  // is !=NULL ?
        i ++;
    }
    fclose(addre);
    printf("Read \033[33m%d\033[0m address from the databases!\n",i);
    return i;
}

//从文件中读取路径
int read_path()
{
    int i;
    ArcNode *node;
    FILE *path;

    i = 0;
    path = fopen("db_path.txt","rt");
    while((fread(&way[i], sizeof(struct ways), 1, path)) != 0) {  // is != NULL ?
//        printf("<%c-%d-%c>\n", way[i].src, way[i].len, way[i].des);
        i ++;
    }
    fclose(path);
    printf("Read \033[33m%d\033[0m paths from the databases!\n",i);
    return i; 
}

//将地点写入文件
void write_address(AdjList G)
{
    int i;
    ArcNode *node;
    FILE *addr;
    
    addr = fopen("db_address.txt","wt");
 
    for(i = 0; i < G.vexnum; i ++) {
        addrs[i].data = G.vex[i].data;
        strcpy(addrs[i].addr_name,G.vex[i].addr_name);
        strcpy(addrs[i].addr_intr,G.vex[i].addr_intr);
        strcpy(addrs[i].addr_key,G.vex[i].addr_key);
    }
    for(i = 0; i < G.vexnum; i ++) {
        if(fwrite(&addrs[i], sizeof(struct addrs), 1, addr) == 0) {
            printf("Error:db_address.txt cann't write!\n");
            exit(0);
        }
    }
    fclose(addr);
    printf("db_address.txt write successed!\n");
}

//将路径写入文件
void write_path(AdjList G)
{
    int i;
    ArcNode *node;
    FILE *path;
    
    path = fopen("db_path.txt","wt");

    for(i = 0; i < G.arcnum; i ++) {
//        printf("<%c-%d-%c>\n", way[i].src, way[i].len, way[i].des);
        if(fwrite(&way[i], sizeof(struct ways), 1, path) == 0) {
            printf("Error:db_path.txt cann't write!\n");
            exit(0);
        }
    }   
    fclose(path);
    printf("db_path.txt write successed!\n");
}

//



//初始化信息库
int setup()
{
    int i;
    FILE *f1, *f2;
    
    f1 = fopen("db_address.txt","rt");
    f2 = fopen("db_path.txt","rt");
    // 判断有无文件信息
    if((f1 != NULL) && (f2 != NULL)) {
        // 读取信息库，初始化创建
        fclose(f1);
        fclose(f2);
        printf("Reading the Databases success!\n");
        return 1;  //
    } else {
        // 未读取到信息库，初始化创建
//        fclose(f1);
//        fclose(f2);
        printf("\033[33mWarning: can't find the Databases!\033[0m\n");
        printf("You need input some informations first!\n");
        return 0;
    }
    return -1;
}

//菜单
int menu()
{
    printf("=========================================================\n\n");
    printf("\t   Welcome to campus's gurb system\n\n\n");
    printf("\t\033[31mA\033[0m. print all the address\n");
    printf("\t\033[31mB\033[0m. print all the paths\n");
    printf("\t\033[31mC\033[0m. print give an address\n");
    printf("\t\033[31mD\033[0m. print all paths from two address\n");
    printf("\t\033[31mE\033[0m. search an address by key words\n");
    printf("\t\033[31mF\033[0m. search the best path from an address\n");
    printf("\t\033[31mG\033[0m. search the best project from an address\n");
    printf("\t\033[31mH\033[0m. add some address and path\n");
    printf("\t\033[31mJ\033[0m. del some address and path\n");
    printf("\t\033[31mP\033[0m. print this menu\n");
    printf("\t\033[31mS\033[0m. save data and exit\n\n");
    printf("=========================================================\n");
    printf("\033[31m>>\033[0m ");
}




//输入数据创建无向网
AdjList* create_lgraph_input()
{
    int i, j, k;
    int p1, p2;
    int weight;
    char c1, c2;
    ArcNode *node1, *node2;
    AdjList *G;

    G = (AdjList *)malloc(sizeof(AdjList));
    G->vexnum = 0;
    G->arcnum = 0;
    i = 0;

    printf("Now input some address:\n");

    //输入新地点及其相关信息
    while(1) {
        printf("\033[33mAddress[%d]\033[0m\n",i+1);
        printf("Give an ID of the address(only 'a' to 'Z'):\n\033[34m>>\033[0m ");
        G->vex[i].data = read_char();
        printf("Give a name of the address(under %d chars):\n\033[34m>>\033[0m ",MAX_NAME);
        scanf("%s",G->vex[i].addr_name);
        printf("Give an introduction of the address(under %d chars):\n\033[34m>>\033[0m ",MAX_INTR);
        scanf("%s",G->vex[i].addr_intr);
        printf("Give some key words in order to find(under %d chars):\n\033[34m>>\033[0m ",MAX_KEY);
        scanf("%s",G->vex[i].addr_key);
        printf("Would you want to input more address?(y/n): ");
        if('n' == read_char()) {
            break;
        }
        printf("\n");
        G->vex[i].head = NULL;
        i ++;
    }
    G->vexnum = i+1;


    //获取路径数
    printf("How many path doi you need:\n\033[34m>>\033[0m ");
    scanf("%d",&G->arcnum);
    j = 0;
        
    //输入两地点间的路径及其距离
    for(j; j < G->arcnum; j ++) {
        printf("Input an way of two address(use an ID):\n");
        printf("\033[33mways[%d]\033[0m ", j+1);
        c1 = read_char();
        c2 = read_char();

        printf("Give the length between the two address(unit/km):\n\033[34m>>\033[0m ");
        scanf("%d",&weight);

        //记录到路径结构体数组中
        way[j].src = c1;
        way[j].des = c2;
        way[j].len = weight;
        
        //获取顶点的位置
        p1 = localvex(*G, c1);
        p2 = localvex(*G, c2);

        //初始化node1
        node1 = (ArcNode *)malloc(sizeof(ArcNode));
        node1->adjvex = p2;
        node1->weight = weight;       
        if(G->vex[p1].head == NULL) {
            G->vex[p1].head = node1;
        } else {             
            link_last(G->vex[p1].head, node1);
        }
      
        //初始化node2
        node2 = (ArcNode *)malloc(sizeof(ArcNode));
        node2->adjvex = p1;
        node2->weight = weight;
            
        if(G->vex[p2].head == NULL) {
            G->vex[p2].head = node2;
        } else {
            link_last(G->vex[p2].head, node2);
        }
    }
    return G;    
}

//从文件创建无向网
AdjList* create_lgraph_file()
{
    char c1, c2; 
    int i, p1, p2;
    int weight;
    ArcNode *node1, *node2;
    AdjList *G; 

    G = (AdjList *)malloc(sizeof(AdjList));

    //读取db文件，并初始化顶点数和边数
    G->vexnum = read_address();
    G->arcnum = read_path();

    // 初始化"邻接表"的顶点
    for(i = 0; i < G->vexnum; i ++) {   
        G->vex[i].data = addrs[i].data;
        strcpy(G->vex[i].addr_name,addrs[i].addr_name);
        strcpy(G->vex[i].addr_intr,addrs[i].addr_intr);
        strcpy(G->vex[i].addr_key,addrs[i].addr_key);
        G->vex[i].head = NULL;
    }
 
    // 初始化两地点间的路径及其距离
    for(i = 0; i < G->arcnum; i ++) {
        // 读取边的起始顶点,结束顶点,权值
        c1 = way[i].src;
        c2 = way[i].des;
        weight = way[i].len;

        p1 = localvex(*G, c1);
        p2 = localvex(*G, c2);

        /*test*/
        /* maybe there have some problems! */
//        printf("[%c-%d %c-%d]\n",c1,p1,c2,p2);
    


        // 初始化node1
        node1 = (ArcNode *)malloc(sizeof(ArcNode));
        node1->adjvex = p2; 
        node1->weight = weight;
        if(G->vex[p1].head == NULL) {
            G->vex[p1].head = node1;
        } else {
            link_last(G->vex[p1].head, node1);
        }

        // 初始化node2
        node2 = (ArcNode *)malloc(sizeof(ArcNode));
        node2->adjvex = p1;
        node2->weight = weight;
        if(G->vex[p2].head == NULL) {
            G->vex[p2].head = node2;
        } else {
            link_last(G->vex[p2].head, node2);
        }
    }  /*test end*/
    return G;
}

// 找到第一个邻接点
int first_vex(AdjList G,int i)
{
    int j;
    ArcNode *node;

    node = G.vex[i].head;
    while(node != NULL)
    {
        return (node->adjvex);
        node = node->next;
    }
    return -1;
}

// 找到下一个邻接点 */
int next_vex(AdjList G,int i,int j)
{
    ArcNode *node;
    node = G.vex[i].head;

    while(node != NULL)
    {
        if(node->adjvex == j)
        {
            node = node->next;
            while(node != NULL)
            {
                if(visited[node->adjvex] == 0)
                    return(node->adjvex);
                node = node->next;
            }
            return -1;
        }
        node = node->next;
    }   
    return -1; 
}

void DFS(AdjList G,int u,int v,int n)
{  
    int j;  
    road[++n] = u;
    visited[u] = 1;
    if(u == v)
    {   
        for(j = 0; j <= n; j ++)
            printf("%s > ",G.vex[road[j]].addr_name);
        printf("\n");
        visited[u] = 0;
        return;
    }

    for(j = first_vex(G,u); j != -1; j = next_vex(G,u,j))
    {
        if(!visited[j])
            DFS(G,j,v,n);
    }
    visited[u] = 0;
}

// 任意两地点间的所有路径
void find_all_path(AdjList G)
{
    int u, v,n=-1;
    char c1, c2;
    
    printf("Give two address to find all paths(use an ID):\n\033[34m>>\033[0m ");
    c1 = read_char();
    c2 = read_char();
    
    u = localvex(G,c1);
    v = localvex(G,c2);
    
    printf("All the paths from %s to %s:\n",G.vex[u].addr_name,G.vex[v].addr_name);
    DFS(G,u,v,n);
}


//获取边start,end的权值；若不连通，则返回无穷大
int get_weight(AdjList G, int start, int end)
{
     ArcNode *node;

     if (start == end)
         return 0;

     node = G.vex[start].head;
     while (node != NULL) {
         if(end == node->adjvex)
            return node->weight;
         node = node->next;
     }
    return INF;
}

//任意两顶点间的最短路径(dijkstra算法)

void best_path(AdjList G, int prev[], int dist[])
{
    int i, j, k;
    char c1,c2;
    int u ,v;
    int min;
    int tmp;
    int t;
    char paths[MAXVEX];
    char flag[MAXVEX];
    
    printf("Give an adddress to find best way(use an ID):\n\033[34m>>\033[0m ");
    c1 = read_char();
//    c2 = read_char();
    //获取输入地点的编号
    u = localvex(G,c1);
//    v = localvex(G,c2);
  
    // 初始化 
    for(i = 0; i < G.vexnum; i ++) {
        flag[i] = 0;  // 顶点i还未获取到最短路径
        prev[i] = 0;  // 顶点i的前驱顶点为0
        dist[i] = get_weight(G, u, i); // 获取u到i的权值
    }
    
    // 初始化顶点u
    flag[u] = 0;
    dist[u] = 0;
    t = 0;
    
    // 遍历vexnum-1次，每次找出一个顶点的最短路径
    for(i = 1; i < G.vexnum; i ++) {
        
        // 在未获取最短路径的顶点中，找离u最近的顶点k
        min = INF;
        for( j = 0; j < G.vexnum; j ++) {
            if(flag[j] == 0 && dist[j] < min) {
                min = dist[j];
                k = j;
            }
        }
        // 标记顶点k为已经获取到最短路径
        flag[k] = 1;
        paths[t] = k;
        t ++;
        
        // 修正当前最短路径和前驱顶点(更新)
        for(j = 0; j < G.vexnum; j ++) {
            tmp = get_weight(G, k, j);
            if(flag[j] == 0 && (tmp < dist[j])) {
                dist[j] = tmp;
                prev[j] = k;
            }
        }
    }
    
 /*   printf("The best way from %s to %s:\n",G.vex[u].addr_name,G.vex[v].addr_name);
    
     for(i = 0; i< t; i ++) {
        printf("%c > ",G.vex[paths[i]].data);
    }

    for(i = 0;i < t; i ++) {
        printf("%s > ",G.vex[paths[i]].addr_name);
    }
    printf("%s\n",G.vex[v].addr_name);

    printf("And the length is: %d\n",dist[v]);
*/

    for (i = 0; i < G.vexnum; i++)
        printf("Shortest from %s to %s is %d\n", G.vex[u].addr_name, G.vex[i].addr_name, dist[i]);

}

//最小生成数树(prim算法)
void best_project(AdjList G)
{
    int min, i, j, k, m, n, tmp, sum;
    int index = 0;  // prim最小树的索引，即prims数组的索引
    int start;
    char ch;
    char prims[MAXVEX];  // prim最小树的结果数组
    int weights[MAXVEX];  // 顶点间边的权值
    
    printf("Give an address to run(use an ID):\n\033[34m>>\033[0m ");
    ch = read_char();
    start = localvex(G,ch);

    prims[index ++] = G.vex[start].data;
    
    // 初始化顶点的权值数组
    for(i = 0; i < G.vexnum; i ++)
        weights[i] = get_weight(G, start, i);

    for(i = 0; i < G.vexnum; i ++) {
   
        if(start == i)  // 从strat开始
            continue;
        
        j = 0;
        k = 0;
        min = INF;
        // 在未被加入到最小生成树的顶点中，找出权值最小的顶点
        while(j < G.vexnum) {
            
            // 若weights[j]=0，意味着"第j个节点已经被排序过"(或者说已经加入了最小生成树中) 
            if(weights[j] != 0 && weights[j] < min) {
                min = weights[j];
                k = j;
            }
            j ++;
        }

        // 将上面获取的权值最小的顶点k加入到最小生成树的结果数组中,并将其标志为0
        prims[index ++] = G.vex[k].data;
        weights[k] = 0;
        
        // 更新其它顶点的权值
        for(j = 0; j < G.vexnum; j++) {
            tmp = get_weight(G, k, j);  // 获取第k个顶点到第j个顶点的权值
            // 当第j个节点没有被处理，并且需要更新时才被更新
            if(weights[j] != 0 && tmp < weights[j]) {
                weights[j] = tmp;
            }
        }
    }
    
    //计算最小生成树的权值
    sum = 0;
    for(i = 1; i < index; i ++) {
        min = INF;
        n = localvex(G,prims[i]);
        
        // 在vex[0...i]中，找出到j的权值最小的顶点
        for(j = 0; j < i; j ++) {
            m = localvex(G,prims[j]);
            tmp = get_weight(G, m, n);
            if(tmp < min) 
                min = tmp;
        }
        sum += min;
    }
    
    //打印最小生成树
    printf("The best project from %s is:\n",G.vex[start].addr_name);
    for(i = 0; i < index; i ++) {
         printf("%s > ",G.vex[i].addr_name);
    }
    printf("\n");
    printf("And the length is: %d\n",sum);
    
}

//按关键词进行景点查询
void find_key_words(AdjList G)
{
    int i;
    int local;
    char a[INF] = {0};
    char key[MAX_KEY];
    char *pa, *pb;
        
    printf("Here are all the key words of the address:\n");
    for(i = 0; i < G.vexnum; i ++) {
        printf("%d >  %s\n", i+1, G.vex[i].addr_key);
        strcat(G.vex[i].addr_key," ");
        strcat(a,G.vex[i].addr_key);
    }    

    printf("Give some key to find address(under %d words):\n\033[34m>>\033[0m ",MAX_KEY);
    scanf("%s",key);

    local = 0;
    pa = a;
    pb = key;
    while(*pa != '\0') {
        if(*pa == ' ')
            local ++;
        if(*pb == *pa) {
            while(*pb == *pa) {
                pb ++;
                pa ++;
            }
            if(*pb == '\0')
                break;
            else
                pb = key;
        }
        pa ++;
    }
    if(i <= G.vexnum) { 
        printf("Address of %c:\n",G.vex[local].data);
        printf("\tAddress Name: %s\n",G.vex[local].addr_name);
        printf("\tIntroduction: %s\n",G.vex[local].addr_intr);
        printf("\tKey  words:   %s\n\n",G.vex[local].addr_key);
    } else {
        printf("Sorry,there is no key words of %s",key);
    }
}

//增加新地点和新路线
void add_addr_path(AdjList *G)
{
    int i, j, k;
    int p1, p2;
    int weight;
    char c1, c2, ch;
    ArcNode *node1, *node2;
    
    printf("Which do you need (\033[033mA\033[0mddress/\033[33mP\033[0math) to add?\n\033[34m>>\033[0m ");
    ch = read_char();

    if(ch == 'A') {  //输入新地点及其相关信息
        i = G->vexnum;
        while(1) {
            printf("\033[33mAddress[%d]\033[0m\n",i+1);
            printf("Give an ID of the address(only 'a' to 'Z'):\n\033[34m>>\033[0m ");
            G->vex[i].data = read_char();
            printf("Give a name of the address(under %d chars):\n\033[34m>>\033[0m ",MAX_NAME);
            scanf("%s",G->vex[i].addr_name);
            printf("Give an introduction of the address(under %d chars):\n\033[34m>>\033[0m ",MAX_INTR);
            scanf("%s",G->vex[i].addr_intr);
            printf("Give some key words in order to find(under %d chars):\n\033[34m>>\033[0m ",MAX_KEY);
            scanf("%s",G->vex[i].addr_key);
            printf("Would you want to input more address?(y/n): ");
            if('n' == read_char()) {
                break;
            }   
            printf("\n");
            G->vex[i].head = NULL;
            i ++; 
        }   
        G->vexnum = i+1;
        printf("\nAddress added successed!\n");

    } else if(ch = 'P') { // 输入新路径及其相关信息
        printf("How many path do you need to add:\n\033[34m>>\033[0m ");
        scanf("%d",&k);
        j = G->arcnum;
        G->arcnum += k; 

        //输入两地点间的路径及其距离
        for(j; j < G->arcnum; j ++) {
            printf("Input an way of two address(use an ID):\n");
            printf("\033[33mways[%d]\033[0m ", j+1);
            c1 = read_char();
            c2 = read_char();

            printf("Give the length between the two address(unit/km):\n\033[34m>>\033[0m ");
            scanf("%d",&weight);

            //记录到路径结构体数组中
            way[j].src = c1;
            way[j].des = c2;
            way[j].len = weight;

            //获取顶点的位置
            p1 = localvex(*G, c1);
            p2 = localvex(*G, c2);
    
            //初始化node1
            node1 = (ArcNode *)malloc(sizeof(ArcNode));
            node1->adjvex = p2;
            node1->weight = weight;
            if(G->vex[p1].head == NULL) {
                G->vex[p1].head = node1;
            } else {
                link_last(G->vex[p1].head, node1);
            }

            //初始化node2
            node2 = (ArcNode *)malloc(sizeof(ArcNode));
            node2->adjvex = p1;
            node2->weight = weight;

            if(G->vex[p2].head == NULL) {
                G->vex[p2].head = node2;
            } else {
                link_last(G->vex[p2].head, node2);
            }
        }
        printf("\nPath added successed!\n");
    } else {
        printf("Input error!\n");
    }
}


//删除旧地点和旧路线
void del_addr_path(AdjList *G)
{
    int i, t;
    int p1, p2;
    int f1, f2;
    char ch, c1, c2;
    char name[MAX_NAME];
    
    printf("Which do you need (\033[033mA\033[0mddress/\033[33mP\033[0math) to delete?\n\033[34m>>\033[0m ");
    ch = read_char();
    if( ch == 'A') {
        printf("Give an address name to delete(under %d chars):\n\033[34m>>\033[0m ",MAX_NAME);
        scanf("%s",name);
        t = 0;
        for(i = 0; i < G->vexnum; i ++) {
            if(strcmp(G->vex[i].addr_name,name) == 0) {
                printf("Find an address of %s,should to delete(y/n):",name);
                ch = read_char();
                if(ch == 'y') {
                    G->vex[i].data = G->vex[G->vexnum].data;
                    strcpy(G->vex[i].addr_name,G->vex[G->vexnum-1].addr_name);
                    strcpy(G->vex[i].addr_intr,G->vex[G->vexnum-1].addr_intr);
                    strcpy(G->vex[i].addr_key,G->vex[G->vexnum].addr_key);
                    G->vexnum -= 1;
                    printf("\nAddress deleted successed!\n");
                    break;
                }
            }
            t ++;
        }
        if(t == G->vexnum)
            printf("Sorry,there is no address name of %s\n",name);
    } else if(ch == 'P') {
        printf("Give an path to delete(eg: a,b):\n\033[34m>>\033[0m ");
        c1 = read_char();
        c2 = read_char();

        f1 = f2 = 0;
        for(i = 0; i < G->arcnum; i ++) {
            if(c1 == G->vex[i].data) f1 = 1;
            if(c2 == G->vex[i].data) f2 = 1;
        }
        if(f1 && f2) {
            for(i = 0; i < G->arcnum; i ++) {
                if(((c1 == way[i].src) && (c2 == way[i].des)) || ((c2 == way[i].src) && (c1 == way[i].des))) {
                    /*  delete an path   */
                    way[i].src = way[G->arcnum].src;
                    way[i].len = way[G->arcnum].len;
                    way[i].des = way[G->arcnum].des;
                    G->arcnum -= 1;
                    printf("\nPath deleted successed!\n");
                    break;
                }
            }
        } else {
            printf("There is no address ID of %c or %c!\n",c1,c2);
        } 
    } else {
        printf("Input error!\n");
    }
}


//打印邻接表图
void print_lgraph(AdjList G)
{
    int i;
    ArcNode *node;

    for (i = 0; i < G.vexnum; i ++) {
        node = G.vex[i].head;
        printf("%c",G.vex[i].data);
        while (node != NULL) {
            printf("-->%c",G.vex[node->adjvex].data);
            node = node->next;
        }   
        printf("\n");
    }
}

//保存并退出系统
void save_exit(AdjList G)
{
    write_address(G);
    write_path(G);
    exit(0);
}

//main()
void main()
{
    char k;
    AdjList *G;
    int ok = 1;
    int prev[MAXVEX] = {0};
    int dist[MAXVEX] = {0};
    
    //初始化系统
    k = setup();
    if( k == 0) {
        G = create_lgraph_input();
    } else if(k == 1) {
        G = create_lgraph_file();
    } else {
        printf("Initialize failed...\n");
        exit(0);
    }
    //进入菜单
    menu();
    fflush(stdin);
    while(ok) {
        k = read_char();
        switch(k) {
            case 'A': print_addr(*G);break;            //显示所有地点ok
            case 'B': print_path(*G);break;            //显示所有路径 ok
            case 'C': print_one_addr(*G);break;        //按地点名查询一个地点信息 ok
            case 'D': find_all_path(*G);break;         //查询任意两地点间的所有路径 ok
            case 'E': find_key_words(*G);break;        //按关键词进行地点查询 ok         
            case 'F': best_path(*G, prev, dist);break; //查询任意两地点间的最短路径 ok
            case 'G': best_project(*G);break;          //查询任意地点的最佳布网方案 ok
            case 'H': add_addr_path(G);break;           //增加新地点和新路线
            case 'J': del_addr_path(G);break;           //删除旧地点和旧路线
            case 'P': system("clear");menu();break;    //显示此菜单 ok
            case 'S': save_exit(*G);break;             //保存并提出 ok
            case 'N':  print_lgraph(*G);break;
            default: printf("Input error!\n");break;  
        }
        printf("\n\033[31m>>\033[0m ");
    }    
}
