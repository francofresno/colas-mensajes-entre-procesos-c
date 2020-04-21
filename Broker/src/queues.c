///*
// ============================================================================
// Name        : Queues
// Author      : Fran and Co
// Description : Funciones de creación de colas
// ============================================================================
// */
//
//#include "queues.h"
//
//
//void enqueue(void* new_message)
//{
//    t_node *nuevo;
//    nuevo = malloc(sizeof(struct node));
//    nuevo->message = new_message;
//    nuevo->sig = NULL;
//
//    if (isEmpty()) {
//        raiz = nuevo;
//        fondo = nuevo;
//    } else {
//        fondo->sig = nuevo;
//        fondo = nuevo;
//    }
//}
//
//void* dequeue()
//{
//    if (!isEmpty()) {
//        void* message = raiz->message;
//        t_node *bor = raiz;
//
//        if (raiz == fondo) {
//            raiz = NULL;
//            fondo = NULL;
//        } else {
//            raiz = raiz->sig;
//        }
//
//        free(bor);
//        return message;
//    }
//
//    return NULL;
//}
//
//void free_queue()
//{
//	t_node *reco = raiz;
//	t_node *bor;
//    while (reco != NULL) {
//        bor = reco;
//        reco = reco->sig;
//        free(bor);
//    }
//}
//
//int isEmpty()
//{
//	return raiz == NULL;
//}
//
