#include<stdio.h>
#include<stdlib.h>
typedef struct node_t{
	int data;
	struct node_t *next;
}link_list_t;
link_list_t *createEmptyList(){
	link_list_t *h=malloc(sizeof(link_list_t));
	h->next=NULL;
	return h;
}
int LenghtList(link_list_t *p){
	int i=0;
	while(p->next!=NULL){
		i++;
		p=p->next;
	}
	return i;
}
int insertList(link_list_t *p,int pos,int x){
	int i;
	for(i=0;i<pos-1;i++){
		if(pos>LenghtList(p))
			return -1;
		p=p->next;
	}
	link_list_t *q=malloc(sizeof(link_list_t));
	q->data=x;
	q->next=p->next;
	p->next=q;
	return 0;
}
int deleteList(link_list_t *p,int pos){
	int i;
	for(i=0;i<pos-1;i++)
		p=p->next;
	link_list_t *q=p->next;
	p->next=q->next;
	free(q);
	return 0;
}
void printList(link_list_t *p){
	while(p->next!=NULL){
		p=p->next;
		printf("%d\n",p->data);
	}
}
void reversePrint(link_list_t *p){
	if(p->next!=NULL){
		reversePrint(p->next);
		printf("%d\n",p->next->data);
	}
}
void freeAll(link_list_t *p){
	while(p!=NULL){
		link_list_t *q=p->next;
		free(p);
		p=q;
	}
}
int main()
{
    link_list_t *h = createEmptyList();
    insertList(h, 1, 40);
    insertList(h, 1, 30);   
    insertList(h, 1, 10);   
    insertList(h, 2, 20);
	printf("%d\n",insertList(h,6,60));
    printList(h);       //10, 20, 30, 40
    deleteList(h, 2);
    printList(h);       //10, 30, 40
    reversePrint(h);    //40, 30, 10, 只要逆序输出即可
    freeAll(h);
}
