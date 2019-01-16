#include <stdio.h>
#include <stdlib.h>
#include "LinkedListAPI.h"

Node *initializeNode(void *data)
{
	Node * node = malloc(sizeof(Node));
	node -> data = data;
	node -> previous = NULL;
	node -> next = NULL;
	return node;
}

List initializeList(char* (*printFunction)(void *toBePrinted),void (*deleteFunction)(void *toBeDeleted),int (*compareFunction)(const void *first,const void *second))
{
	List list;
	list.head = NULL;
	list.tail = NULL;
    list.printData = printFunction;
    list.deleteData = deleteFunction;
    list.compare = compareFunction;
	list.length = 0;
return list;
}
void insertBack(List* list, void* toBeAdded){
	if (list == NULL || toBeAdded == NULL){
		return;
	}

	Node* newNode = initializeNode(toBeAdded);

    if (list->head == NULL && list->tail == NULL){
        list->head = newNode;
				list->tail = newNode;
				list->length++;
        //list->tail = list->head;
    }else{
		newNode->previous = list->tail;
      list->tail->next = newNode;
    	list->tail = newNode;
			list->length++;
    }
}

void insertFront(List* list, void* toBeAdded){
	if (list == NULL || toBeAdded == NULL){
		return;
	}

	Node* newNode = initializeNode(toBeAdded);

    if (list->head == NULL && list->tail == NULL){
      list->head = newNode;
      list->tail = newNode;
			list->length++;
    }else{
			newNode->next = list->head;
      list->head->previous = newNode;
    	list->head = newNode;
			list->length++;
    }
}
/*
void * deleteList(List list)
{
	Node * temp = list.head;
	Node * temp2;
  if (temp == NULL)
	{
		printf("List is Empty!\n");
	}
  else
	{
    while(temp != NULL)
    {
      temp2 = temp->next;
      free(temp);
      temp = temp2;
      printf("List successfully deleted\n");
    }
	}
  //free(list);
  return NULL;
}
*/
void insertSorted(List * list, void * toBeAdded){
	if (list == NULL || toBeAdded == NULL){
		return;
	}

	if (list->head == NULL){
		insertBack(list, toBeAdded);
		//list->length++;
		return;
	}

	if (list->compare(toBeAdded, list->head->data) <= 0){
		insertFront(list, toBeAdded);
		//list->length++;
		return;
	}

	if (list->compare(toBeAdded, list->tail->data) > 0){
		insertBack(list, toBeAdded);
		//list->length++;
		return;
	}

	Node* currNode = list->head;

	while (currNode != NULL){
		if (list->compare(toBeAdded, currNode->data) <= 0){

			char* currDescr = list->printData(currNode->data);
			char* newDescr = list->printData(toBeAdded);

			printf("Inserting %s before %s\n", newDescr, currDescr);

			free(currDescr);
			free(newDescr);

			Node* newNode = initializeNode(toBeAdded);
			newNode->next = currNode;
			newNode->previous = currNode->previous;
			currNode->previous->next = newNode;
			currNode->previous = newNode;
			list->length++;

			return;
		}
		currNode = currNode->next;
	}
	//list->length++;
	return;
}

void* deleteDataFromList(List* list, void *toBeDeleted)
{
	if (list == NULL || toBeDeleted == NULL){
		return NULL;
	}

	Node* tmp = list->head;

	while(tmp != NULL){
		if (list->compare(toBeDeleted, tmp->data) == 0){
			//Unlink the node
			Node* delNode = tmp;

			if (tmp->previous != NULL){
				tmp->previous->next = delNode->next;
			}else{
				list->head = delNode->next;
			}

			if (tmp->next != NULL){
				tmp->next->previous = delNode->previous;
			}else{
				list->tail = delNode->previous;
			}

			void* data = delNode->data;
			free(delNode);
			list->length--;
			return data;

		}else{
			tmp = tmp->next;
			list->length--;
		}
	}
	list->length--;
	return NULL;
}

void *getFromFront(List list)
{
	void * temp;
	if (list.head == NULL){
		return NULL;
	}
	temp = list.head->data;
	return temp;
}


void *getFromBack(List list)
{
	Node * temp;
	if (list.head == NULL){
		return NULL;
	}
	temp = list.head;
	while(temp->next !=  NULL)
	{
	    temp =temp->next;
	}
	return temp->data;
}

char* toString(List list){
	ListIterator iter = createIterator(list);
	char* str;

	str = (char*)malloc(sizeof(char));
	strcpy(str, "");

	void* elem;
	while( (elem = nextElement(&iter)) != NULL){
		char* currDescr = list.printData(elem);
		int newLen = strlen(str)+50+strlen(currDescr);
		str = (char*)realloc(str, newLen);
		strcat(str, "\n");
		strcat(str, currDescr);

		free(currDescr);
	}

	return str;
}

void clearList(List* list){

    if (list == NULL){
		return;
	}

	if (list->head == NULL && list->tail == NULL){
		return;
	}

	Node* tmp;

	while (list->head != NULL){
		list->deleteData(list->head->data);
		tmp = list->head;
		list->head = list->head->next;
		free(tmp);
	}

	list->head = NULL;
	list->tail = NULL;
}

ListIterator createIterator(List list){
    ListIterator iter;

    //Assert creates a partial function...
    //assert (list.head != NULL);

    iter.current = list.head;

    return iter;
}

void* nextElement(ListIterator* iter){
    Node* tmp = iter->current;

    if (tmp != NULL){
        iter->current = iter->current->next;
        return tmp->data;
    }else{
        return NULL;
    }
}

int getLength(List list){
	int count = 0;

	if (list.head != NULL){
		while(list.head != NULL){
			count++;
			list.head = list.head->next;
		}
	}
	return count;
}

void* findElement(List list, bool (*customCompare)(const void* first,const void* second), const void* searchRecord){
	Node * temp = list.head;
		if (list.head == NULL){
			return NULL;
		}
		else{
			while(temp != NULL){
				if (customCompare(temp->data, searchRecord) == true){
					return temp->data;
				}
				temp = temp->next;
			}
			return NULL;
		}
}
