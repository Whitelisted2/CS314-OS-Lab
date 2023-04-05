#include<stdio.h>
#include<stdlib.h>
#include<math.h> // requires '-lm' flag during compilation
// #include<ctype.h>  // for isdigit()

// structure for a tree node
struct BSTree{
    struct BSTree *left;
    int size;               // size
    int is_used;            // if node has content
    int pid;                // process id
    char pname;             // process identifier
    int bytes_occupied;     // bytes used by process id
    struct BSTree *parent;
    struct BSTree *right;
};

// function to create a node
struct BSTree *create(int size_in){
    struct BSTree *temp = (struct BSTree *)malloc(sizeof(struct BSTree));
    temp->left = NULL;
    temp->size = size_in;
    temp->is_used = 0;
    // pid_global++;
    temp->pid = -1;
    temp->pname = '_';
    temp->bytes_occupied = 0;
    temp->parent = NULL;
    temp->right = NULL;
    return temp;               // returns node (as a structure)
}
struct BSTree *root = NULL;
int pid_global = -1;
int T;
int total_size, min_granularity;

struct CDLLNode{
    struct BSTree *treeNode;
    struct CDLLNode *next;
    struct CDLLNode *prev;
};

struct CDLLNode *create_cllnode(struct BSTree *treeNode_in){
    struct CDLLNode *temp = (struct CDLLNode *)malloc(sizeof(struct CDLLNode));
    temp->treeNode = treeNode_in;
    temp->next = NULL;
    temp->prev = NULL;
    return temp;               // returns node (as a structure)
}
struct CDLLNode *head = NULL;

int find_reqd_power_of_2(int pbytes){
    if(pbytes > total_size){
        printf("Process requires too much space!\n");
        exit(1);
    }
    int i = min_granularity;
    while((int)pow(2, i) < pbytes){
        i = 2*i;
    }
    return i;
}


struct BSTree *search(int size_req){
    
}

void send_query(char pname_in, int pbytes){        // HQ of what to do with a request of the form P S
    printf("Query: %c %d\n", pname_in, pbytes);
    if(pbytes == 0){
        // must remove process and run coalescing sequence

    } else {
        // search for available loc... should be from free list... ; split if necessary.
        struct BSTree *dest_node;
        printf("-%d-\n", find_reqd_power_of_2(pbytes));
        // dest_node = search(pbytes);

        // allot space 

    }
}

void print_status(struct BSTree *node){ // only root should be passed
    if(node == NULL){
        return;
    }
    print_status(node->left);
    print_status(node->right);
    if(node->left == NULL && node->right == NULL){
        printf("[%c %d] ", node->pname, node->size);
    }
}

void clear(struct BSTree *node){ // cleans up the tree for each test case
    if(node == NULL){
        return;
    }
    clear(node->left);
    clear(node->right);
    free(node);
}

void clean_list(struct CDLLNode *node){
    while(node->next != node){
        struct CDLLNode *temp;
        temp = node;
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
        node = node->next;
        free(temp);
    }
    free(node);
}

void split(struct BSTree *node){ // splits a given node, returns nothing
    struct BSTree *left_node;
    struct BSTree *right_node;
    int curr_size = node->size;
    if(curr_size/2 < min_granularity){
        printf("Trying to split min-size node!\n");
        return;
    }

    left_node = create(curr_size/2);
    right_node = create(curr_size/2);

    node->left = left_node;     //       n
    node->right = right_node;   //     /  \
                                //   l     r

    left_node->parent = node;
    right_node->parent = node;
}

void print_free_list(){
    struct CDLLNode *temp;
    temp = head;
    printf("Free List: ");
    while(temp->next != head){
        printf("--%d--", temp->treeNode->size);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    FILE *fp;                          // to empty the output file
    fp = fopen("output.txt", "w");
    fclose(fp);
 
    if(argc>2)                         // argc contains argument count. Argument count should be 2 for execution to proceed in this case
    {                                  // the arguments are stored using pointer array *argv[] 
        printf("too many arguments. enter exactly one file name.");
        exit(1);
    }
    else if(argc<2)
    {
        printf("too few arguments. enter exactly one file name.");
        exit(2);
    }
    
    // read input file line-by-line
    FILE *in;
    in = fopen(argv[1], "r");
    
    fscanf(in, "%d\n", &T);
    int i = 0;
    for(int i=0; i<T; i++){
        //fgets(line, 20, in) != NULL
        // fscanf(in, "\n", &line); // each case has newline before it
        int U, L, pbytes;
        
        fscanf(in, "%d %d\n", &U, &L);
        clear(root);

        head = create_cllnode(root);
        head->next = head;
        head->prev = head;
        // if(root == NULL){
        //     printf("root is null here");
        // }
        total_size = (int)pow(2, U); // max block size
        min_granularity = (int)pow(2, L); // min block size
        root = create(total_size);
        // print_status(root);
        // printf("-%d-----", root->left->size);


        char pname_in;
        char line[20];
        while(fgets(line, 20, in) != NULL){
            if(line[0] == '\n'){ // end of current test case
                // printf("newline\n");
                break;
            }
            sscanf(line, "%c %d\n", &pname_in, &pbytes);
            // fscanf(in, "%c %d\n", &pname_in, &pbytes) != EOF
            // if(pname_in == '\n'){
            //     break;
            // }
            
            send_query(pname_in, pbytes); // from here, we send info like A 20

            print_status(root);
            printf("\n");


            char* temp;
        }
        printf("end of this test, with U: %d and L: %d\n", U, L);
        
        clean_list(head);
    }
    fclose(in);
    return 0;
}

