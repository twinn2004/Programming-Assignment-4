/*
Taylor Winn
Programming Assignment 4
March 2024
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN 19   // the max numer of characters within a name is 19

typedef struct customer {
    char name[MAXLEN+1];    // copying in struct using the maxlen for the name memory allocation
    int points;
} customer;

typedef struct treenode {
    customer* cPtr;
    int size;
    struct treenode* left;
    struct treenode* right;
} treenode;

int main(){

    treenode* DeleteFromTree(treenode *root, const char * name);
    void AddToTree(treenode ** root, customer * customer);
    treenode * AllocateTree(customer * customer);
    customer* FindInTree(treenode * root, const char *name);
    customer * NewCustomer(const char *name, int points);
    int DepthTracker(treenode* root, const char* name);
    int CountSmaller(treenode* root, const char* name);
    void FinalNodes(treenode *root, customer * arry[], int * i);
    int NodeCount(treenode* root);
    void Sort(customer * arry[], int left, int right);


    FILE *file = NULL;  // file pointer
    file = fopen(argv[1], "r"); // command line argument for file
    if(file == NULL){       
        printf("cannot open file"); // tedious condition just in case
        return 1;
    } 

    int numPeople = 0;
    fscanf(file, "%d", &numPeople); // scanning the number of commands

    treenode * root = NULL; // initializing the root of the tree to null before reading in the first node
    for(int i = 0; i < numPeople; i++){

        char name[MAXLEN + 1];
        char instruction[14];
        int points;

        fscanf(file, "%s %s %d", instruction, name, &points);

        switch(instruction[0]){ // switch statement because we only have a few commands to process
            
            case 'a':   // executes the first letter of the command is a (for add)
                if(strcmp(instruction, "add") == 0){    // checking for redundency 
                    customer * checkforperson = FindInTree(root, name); // making sure the name entered is actually in the tree as of current
                    if(checkforperson != NULL){
                        checkforperson->points += points;
                        printf("\n%s %d", checkforperson->name, checkforperson->points);    // if in tree, incriment their points accordingly
                    } else {
                        customer * newC = NewCustomer(name, points);
                        AddToTree(&root, newC); // if not in tree, add them to the tree 
                        printf("\n%s %d", newC->name, newC->points);
                    }
                }
                break;

            case 's':
                if(strcmp(instruction, "sub") == 0){    // first command that starts with s 

                    customer * checkforperson = FindInTree(root, name);
                    if(checkforperson != NULL){
                        if(checkforperson->points >= points){
                            checkforperson->points -= points;   // if in tree and their current # of points > # being sub, decriment their points accordingly
                            printf("\n%s %d", checkforperson->name, checkforperson->points);
                        } else {
                            checkforperson->points = 0; // if # being subtracted is > than their current number, they will just have no points
                            printf("\n%s %d", checkforperson->name, checkforperson->points);
                        }
                    } else {
                        printf("\n%s not found", name); // if the person was not found in the tree you cannot subtract anything
                    }
                } else if (strcmp(instruction, "search") == 0){ // the other command that starts with s

                    customer * checkforperson = FindInTree(root, name);
                    if(checkforperson != NULL){
                        if(strcmp(root->cPtr->name, name) == 0){
                            printf("\n%s %d %d", checkforperson->name, checkforperson->points, 0);
                        } else {
                            int smaller = DepthTracker(root, name); // finding how far from the root the name being tested is 
                            printf("\n%s %d %d", checkforperson->name, checkforperson->points, smaller);
                        }
                    } else {
                        printf("\n%s not found", name); // if not added int0 the tree of previously deleted
                    }
                }

                break;

            case 'd':
                if(strcmp(instruction, "del") == 0){    // deleting 
                    root = DeleteFromTree(root, name);
                }
                printf("\n%s deleted", name);
                break;

            case 'c':
                if(strcmp(instruction, "count_smaller") == 0){
                    int smaller = CountSmaller(root, name);
                    printf("\n%d", smaller);
                }
                break;

            default:
                printf("\ninstruction not recognized\n");   // else case, if the input was not one of the specified commands
                break;
        }
    }

    int finalCount = NodeCount(root);   // tracking the number of nodes left in the tree after all commands have excecuted
    customer * arry[finalCount];    // creating array to store those nodes and sort through them
    int i = 0;
    FinalNodes(root, arry, &i);
    Sort(arry, 0, finalCount-1);

    for(int i = 0; i < finalCount; i++){
        printf("\n%s %d", arry[i]->name, arry[i]->points);
    }


fclose(file);   // closing file
return 0;
}

customer * NewCustomer(const char *name, int points){   // allocating memory for a new customer

    customer *new = malloc(sizeof(customer));   
    strcpy(new->name, name);    
    new->points = points;

    return new;
}

customer* FindInTree(treenode * root, const char *name){    // searching for a name in the tree
    if(root == NULL || strcmp(root->cPtr->name, name) == 0){    // if the root is null there is nothing in the tree
        if(root == NULL){   
            return NULL;
        } else {
            return root->cPtr;  // if the root is the node we were looking for, just return the root's customer struct
        }
    } 
    
    if(strcmp(name, root->cPtr->name) < 0){ // if the name being searched for has a lesser lexico value than the root, we will need to look left
        return FindInTree(root->left, name);
    } else {
        return FindInTree(root->right, name);   // if greater, we will need to look right
    }
    
}

treenode * AllocateTree(customer * customer){   // allocating memory for nodes within the tree

    treenode * node = malloc(sizeof(treenode));
    node->cPtr = customer;
    node->left = NULL;
    node->right = NULL;

    return node;

}

void AddToTree(treenode ** root, customer * customer){  // adding a node to the tree at apropriate index
    
    treenode * node = AllocateTree(customer);   // making sure we allocate space for the new node within the tree
    if(*root == NULL){  // if the tree is empty, the root becomes the node that we are adding
        *root = node;
        node->size = 0;
        return;
    } else {
        treenode * hold = *root;    // holding the value of the real root constant while we loop through the tree
        while(1){
            if(strcmp(customer->name, hold->cPtr->name) < 0){   // if the name is smaller lexicographically than the root, we need to look left
                if(hold->left == NULL){ // if the node being checked dosent have any children on the appropriate side, we make the new node the first
                    hold->left = node; 
                    return;
                }
                hold->left->size += 1;
                hold = hold->left;  // index through the tree until we find the leftmost node without any children on the appropriate side
            } else {    // if greater, we will go right
                if(hold->right == NULL){
                    hold->right = node;
                    return;
                }
                hold = hold->right; // continuing the indexing through the right side of the tree
            }
        }
    }
}

treenode* DeleteFromTree(treenode *root, const char * name){
    if(root == NULL){
        return root;    // if there is nothing to delete 
    }

    if(strcmp(name, root->cPtr->name) > 0){
        root->right = DeleteFromTree(root->right, name);    // moving right
    } else if(strcmp(name, root->cPtr->name) < 0){
        root->left = DeleteFromTree(root->left, name);  // moving left
    } else {    // if the node that we are deleting is the root, we need to re assign with the leftmost value in the tree
        if(root->right == NULL){
            treenode * hold;
            hold = root->left;  // if the node dosent have any right children, then the left child (assuming there is one) will be the replacement
            free(root);
            return hold;
        } else if (root->left == NULL){
            treenode * hold;
            hold = root->right; // same as the right just inverted
            free(root);
            return hold;
        }

        treenode * replacement = root->left;    // given the root has both left and right children
        while(replacement->left->left != NULL){ // moving through the left side until we get to the last node with a left child to check if there are more right children after
            replacement = replacement->left;
        }

        while(replacement->right != NULL){
            replacement = replacement->right;   // if there is a value in the right side of the last left node, we need to find it
        }

        strcpy(root->cPtr->name, replacement->cPtr->name);  // copy the value of the replacement node to the root node index
        root->cPtr->points = replacement->cPtr->points; // copy points aswell

        root->left = DeleteFromTree(root->left, replacement->cPtr->name);   // call recursive to execute until balanced
    }

    root->size += 1;    

    return root;
}

int DepthTracker(treenode* root, const char* name) {    // finding how deep a node is (how far from the root node)
    if (root == NULL) {
        return -1; // if the tree is empty, return -1
    }

    int crossCheck = strcmp(name, root->cPtr->name);   // making sure the name being searched isnt the root

    if (crossCheck == 0) {
        return 0; // if the root is the name being searched, there cannot be any distance between it 
    } else if (crossCheck < 0) {   // if the name is lexicographically smaller than the root, we need to 
        int left = DepthTracker(root->left, name);
        if(left == -1){
            return -1;
        } else {
            return left + 1;
        }
    } else {    // if greater, we need to move right
        int right = DepthTracker(root->right, name);
        if(right == -1){
            return -1;
        } else {
            return right + 1;
        }
    }
}

int CountSmaller(treenode* root, const char* name) {    // counting the number of nodes with a less lexicographic value than the node being tested
    if (root == NULL) {
        return 0;
    }

    int count = 0;
    
    count += CountSmaller(root->left, name);    // moving through the left subtree as that is the side storing smaller values
    if (strcmp(name, root->cPtr->name) > 0) {
        count++;    // only incrimenting count if the node is not equal to the node being tested 
    }
    count += CountSmaller(root->right, name);   // if there are right children in the left subtree we must also account for them, if not it will just count as root == null

    return count;
}

int NodeCount(treenode* root) { // function to count the number of nodes within the tree to help with creating sorting array
    if (root == NULL) {
        return 0;
    }

    return 1 + NodeCount(root->left) + NodeCount(root->right);  // moving through tree until the tested node is null
}


void FinalNodes(treenode *root, customer * arry[], int * i){
    if(root != NULL){
        FinalNodes(root->left, arry, i);

        arry[*i] = root->cPtr;  // filling the array with the values stored within the nodes (customer struct values)
        (*i)++;

        FinalNodes(root->right, arry, i);
    }
}



void merge(customer * arry[], int left, int right, int middle){ // mergesort
    int a = 0;
    int b = 0;
    int c = left;

    int firstIndex = middle - left + 1; // number of values on the left side
    int secondIndex = right - middle;   // number of values on the left side

    customer * leftSide[firstIndex];    // array for the values on the left side
    customer * rightSide[secondIndex];  // for the right side

    for(int i = 0; i < firstIndex; i++){
        leftSide[i] = arry[left + i];   // copying values from the left side
    }
    for(int i = 0; i < secondIndex; i++){
        rightSide[i] = arry[middle + 1 + i];    // copying from the right side
    }

    while(a < firstIndex && b < secondIndex){
        if(leftSide[a]->points > rightSide[b]->points){
            arry[c] = leftSide[a];  // sorting by points, adding the index with the larger number of points first
            a++;
        } else if (leftSide[a]->points < rightSide[b]->points){
            arry[c] = rightSide[b];
            b++;
        } else {
            if(strcmp(leftSide[a]->name, rightSide[b]->name) <= 0){ // if the number of points are equal, we check which name comes first lexicographically using string compare
                arry[c] = leftSide[a];
                a++;
            } else {
                arry[c] = rightSide[b];
                b++;
            }
        }
        c++;
    }
    
    while(b < secondIndex){ // adding leftover values if any
        arry[c] = rightSide[b];
        b++;
        c++;
    }

    while(a < firstIndex){  // adding leftover values if any
        arry[c] = leftSide[a];
        a++;
        c++;
    }
}

void Sort(customer * arry[], int left, int right){  // calling the merge function  
    if(left < right){  
        int middle = 0;
        middle = left + (right - left) / 2; // finding the middle index

        Sort(arry, left, middle);
        Sort(arry, middle+1, right);    // sorting the array 

        merge(arry, left, right, middle);   // merging the array   
    }
}



