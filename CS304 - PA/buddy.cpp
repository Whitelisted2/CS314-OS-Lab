#include<stdio.h>
#include<stdlib.h>
#include<cmath> // requires '-lm' flag during compilation if using .c file
#include<unordered_map>
// #include<ctype.h>  // for isdigit()

using namespace std;

bool verbose = false;

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
unordered_map<char, struct BSTree*> running_rec;
// A: (node)

struct LLNode{
    struct BSTree *treeNode;
    struct LLNode *next;
};
unordered_map<int, struct LLNode*> free_map;
// 256: (n1)->(n2)-> ... ->NULL

struct LLNode *create_free_node(struct BSTree *treeNode_in){ // create this after the free node is created in the tree
    struct LLNode *temp = (struct LLNode *)malloc(sizeof(struct LLNode));
    temp->next = NULL;
    temp->treeNode = treeNode_in;
    return temp;  
}

void LL_insert(struct LLNode **head, struct LLNode *newnode){
    // struct LLNode *temp;
    // temp = head;
    if(*head == NULL){
        *head = newnode;
    } else {
        newnode->next = *head;
        *head = newnode;
    }
}
/***************************************************************************/
// idea: unordered map: <int, LL of Tree Nodes>
// (2^L) , LL1 
// (2^(L+1)), LL2
// etc
// insertion: at beginning of node size LL
// removal: go to respective LL and remove
// coalescing: add to the parent node size LL, remove from the child size LL
/**************************************************************************/

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

int find_reqd_power_of_2(int pbytes){
    if(pbytes > total_size){
        printf("Process requires too much space!\n");
        exit(3);
    }
    int i = min_granularity;
    while(i < pbytes){
        i = i*2;
    }
    return i;
}

void print_status(struct BSTree *node){ // only root should be passed
    if(node == NULL){
        return;
    }
    print_status(node->left);
    print_status(node->right);
    if(node->left == NULL && node->right == NULL){
        if(verbose){
            printf("[%c(%d) %d] ", node->pname, node->bytes_occupied, node->size);
        } else {
            if(node->pname == '_'){
                printf("Free Block: %d\n", node->size);
            } else{
                printf("%c: %d\n", node->pname, node->bytes_occupied);
            }
        }
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



void split(struct BSTree *node){ // splits a given node, returns nothing. maybe add as free to LL
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

    // LL_insert(&free_map[curr_size/2], create_free_node(left_node));
    LL_insert(&free_map[curr_size/2], create_free_node(right_node));

}

void initialize_free_map(){
    free_map.clear();
    for(int i = min_granularity; i<=total_size; i=i*2){
        struct LLNode *head;
        head = NULL;
        free_map[i] = head; // size i: empty LL
    }
}


void print_free_map(){
    struct LLNode *head;
    printf("*****************Free Map*****************\n");
    for(int i = min_granularity; i<=total_size; i=i*2){
        printf("%d : ", i);
        struct LLNode *temp;
        temp = free_map[i];
        while(temp!=NULL){
            printf("%d ", temp->treeNode->size);
            temp = temp->next;
        }
        printf("\n");
        // head = NULL;
        // free_map[i] = head; // size i: empty LL
        
    }
    printf("******************************************\n");

}

void coalesce(struct BSTree *node){ // input: parent to coalesce. remove children from the free list also
    // sanity check
    // if(node->left->is_used == 0 && node->right->is_used == 0){
        struct BSTree *left_temp;
        struct BSTree *right_temp;
        left_temp = node->left; // establish ptrs to children
        right_temp = node->right;
        node->left = NULL; // unlink
        node->right = NULL;

        // free list clear children
        int child_size = left_temp->size;
        struct LLNode *temp;
        temp = free_map[child_size];
        if(temp->treeNode == left_temp){
            free_map[child_size] = free_map[child_size]->next;
            free(temp);
        } else if(temp->treeNode == right_temp){
            free_map[child_size] = free_map[child_size]->next;
            free(temp);
        } else {
            struct LLNode *prev;
            struct LLNode *temp2;
            prev = temp;
            while(temp != NULL){
                if(temp->treeNode == left_temp){
                    prev->next = temp->next;
                    temp2 = temp;
                    temp = temp->next;
                    free(temp2);
                    continue;
                } else if(temp->treeNode == right_temp){
                    prev->next = temp->next;
                    temp2 = temp;
                    temp = temp->next;
                    free(temp2);
                    continue;
                } else {
                    prev = temp;
                }
                temp = temp->next;
            }
        }
        // by this time, the children are out of free list
        free(left_temp); // get rid of the children from tree
        free(right_temp);
    // } 
    // else {
    //     printf("Cannot coalesce, unexpected error occurred. \n");
    //     exit(5);
    // }
}

void send_query(char pname_in, int pbytes){        // HQ of what to do with a request of the form P S
    if(verbose){
        printf("Query: %c %d\n", pname_in, pbytes);
    }
    if(pbytes == 0){
        // search for process, use unordered map 'running_rec'
        // must remove process and run coalescing sequence
        struct BSTree *loc;
        loc = running_rec[pname_in];
        running_rec.erase(pname_in);
        struct BSTree *temp;
        temp = loc;
        temp->is_used = 0;
        
        while(temp != root){
            if(temp == temp->parent->left){ // if temp is left child
                if(temp->parent->right->is_used == 0){
                    temp = temp->parent;
                    coalesce(temp);
                } else {
                    break;
                }
            } else if(temp == temp->parent->right){ // if temp is right child
                if(temp->parent->left->is_used == 0){
                    temp = temp->parent;
                    coalesce(temp);
                } else {
                    break;
                }
            }
        } 
        // after coalescing, temp is a non-reducible node, which is either childless root or has a sibling which is in use
        if(temp == root){ // tree has only one process in root, asking to be freed
            root->bytes_occupied = 0;
            root->is_used = 0;
            root->pid = -1;
            root->pname = '_';
            return;
        }
        temp->bytes_occupied = 0;
        temp->pid = -1;
        temp->pname = '_';
        temp->is_used = 0;
        // on reaching here in the code, temp has a sibling in use, but temp is free
        LL_insert(&free_map[temp->size], create_free_node(temp));


    } else {
        // search for available loc... should be from free list... ; split if necessary.
        // struct BSTree *dest_node;
        // printf("-%d-\n", find_reqd_power_of_2(pbytes));
        int want = find_reqd_power_of_2(pbytes);
        int can_get = want;
        int flag = 0; // assume can not
        while(can_get <= total_size){ // check if we can take the request
            if(free_map[can_get] != NULL){
                flag = 1;
                break; // if breaking here, there is an availability, and can_get holds what is possible!
            }
            can_get *= 2;
        }
        if(flag == 0){
            printf("Can't fulfill the request line '%c %d'. Will ignore it.\n", pname_in, pbytes);
            return;
        }
        // printf("Can get %d\n", can_get);
        // must insert pbytes to the free node with size can_get
        if(want == can_get){ // no splitting required, just remove from free list. allot.
            struct BSTree *curr;
            curr = free_map[can_get]->treeNode;
            free_map[can_get] = free_map[can_get]->next;
            
            curr->bytes_occupied = pbytes;
            curr->is_used = 1;
            pid_global++;
            curr->pid = pid_global;
            curr->pname = pname_in;
            running_rec[pname_in] = curr;

        } else { // splitting necessary. add whatever is collaterally free to free_map. allot.
            
            int temp = can_get;
            struct BSTree *curr;
            curr = free_map[can_get]->treeNode;
            while(temp != want){
                split(curr);
                curr->is_used = 1; // since it or its children are occupied.
                curr = curr->left;
                temp = curr->size;
            }
            // when we come out of this, curr is where we want to allot the process!
            // collateral things are free.
            free_map[can_get] = free_map[can_get]->next;
            
            curr->bytes_occupied = pbytes;
            curr->is_used = 1;
            pid_global++;
            curr->pid = pid_global;
            curr->pname = pname_in;
            running_rec[pname_in] = curr;
        }
    }
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
        if(U <= L || L == 0){
            printf("Invalid and Unfair to give a test case that's not according to given constraints. :(\n");
            char line[20];
            while(fgets(line, 20, in) != NULL){
                if(line[0] == '\n'){ // end of current test case
                    // printf("newline\n");
                    break;
                }
                char temp;
                int temp2;
                sscanf(line, "%c %d\n", &temp, &temp2);
            }
            printf("So, will ignore that specific case. \n");
            continue;
        }
        clear(root);
        
        if(verbose){
            printf("_________________________________________________________________________________\n");
            printf("Start of test case, with U: %d and L: %d\n", U, L);
        }

        // if(root == NULL){
        //     printf("root is null here");
        // }
        total_size = (int)pow(2, U); // max block size
        min_granularity = (int)pow(2, L); // min block size
        root = create(total_size);

        initialize_free_map();
        running_rec.clear();
        struct LLNode *temp;
        temp = create_free_node(root); // (root)
        // to the start of the resp. free list.
        // free_map[total_size] = temp;
                    // LL               llNode
        LL_insert(&free_map[total_size], temp);
    
        if(verbose){
            print_free_map(); 
        }
        // printf("%d\n", free_map[total_size]->treeNode->size);

        // print_status(root);
        // printf("-%d-----", root->left->size);

        // preferably initialize free lists of size 2^L, 2^(L+1), ... 2^U

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
            if(verbose){
                print_free_map();
                printf("Status of mem: ");
                print_status(root);
                printf("\n");
            }
            // exit(4); // testing
        }
        print_status(root);
        if(i<T-1){
            printf("\n");
        }
    }
    fclose(in);
    return 0;
}

