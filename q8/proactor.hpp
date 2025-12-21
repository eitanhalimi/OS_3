// q8/proactor.hpp
#pragma once
#include <pthread.h>

typedef void * (* proactorFunc) (int sockfd); 
 
// starts new proactor and returns proactor thread id.  
pthread_t startProactor (int sockfd, proactorFunc threadFunc);  
// stops proactor by threadid 
int stopProactor(pthread_t tid);