#include "co.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <setjmp.h>
#include <string.h>
#include <unistd.h>

#define KB *1024
#define STACK_SIZE 64 KB



int count = 0;
struct co* current = NULL;

enum co_status{
    co_new = 1,
    co_running,
    co_waiting,
    co_dead,
};

static inline void
stack_switch_call(void *sp, void *entry, uintptr_t arg) {
    asm volatile (
#if __x86_64__
        "movq %0, %%rsp; movq %2, %%rdi; jmp *%1"
          :
          : "b"((uintptr_t)sp),
            "d"(entry),
            "a"(arg)
          : "memory"
#else
        "movl %0, %%esp; movl %2, 4(%0); jmp *%1"
          :
          : "b"((uintptr_t)sp - 8),
            "d"(entry),
            "a"(arg)
          : "memory"
#endif
    );
}



struct co {
    char *name;
    void (*func)(void *);
    void *arg;
    int count;

    enum co_status status;
    jmp_buf context;
    uint8_t stack[STACK_SIZE]; // 这样就可以分配栈空间了
};

struct co *co[150];


__attribute__((constructor)) void co_init(){
    struct co* new_co = (struct co*)malloc(sizeof(struct co));

    if(new_co == NULL){
        fprintf(stderr,"malloc error in main");
    }

    new_co->name = "main";
    new_co->status = co_waiting;
    new_co->count = count;

    co[count++] = new_co;
    current = new_co;
}


void coroutine_wrapper(struct co *co_ptr){
    co_ptr->func(co_ptr->arg);

    co_ptr->status = co_dead;


    if(co_ptr->status != co_dead){
        fprintf(stderr,"error in wrapper");
    }

    printf("------------");
    co_yield();
    return;
}


struct co *co_start(const char *name, void (*func)(void *), void *arg) {


    if(count >= 150){
        fprintf(stderr,"Too many coroutines created!\n");
         return NULL;
    }

    
    struct co *new_co = (struct co *)malloc(sizeof(struct co));

    if(new_co == NULL){
        fprintf(stderr,"Memory not allocated for coroutine.\n");
        return NULL;
    }

    new_co->name = " ";
    new_co->func = func;
    new_co->arg = arg;
    new_co->status = co_new;
    new_co->count = count;
    
    co[count++] = new_co;
    
    return new_co;
}


void co_wait(struct co *co_old) {
    if(co_old->status == co_dead){
            for(int i = co_old->count;i < count -1;i++){
                co[i] = co[i+1];
            }
            count--; 
            free(co_old);
            return;
       }
       else{

        
        co_yield();
        co_wait(co_old);
       }
           
 }




void co_yield() {
    int val = setjmp(current->context);
    if(current->status != co_dead)
        current->status = co_waiting;

    if(val == 0){
        //如何模拟随机选取协程?
        if(count <= 0){
            fprintf(stderr,"field in use co_yeid");
            return;
        }

        int next_co_index = 0;
        do{
            next_co_index = rand()%count;
        }while(co[next_co_index]->status == co_dead);

        current = co[next_co_index];

        if(current->status == co_new){
             current->status = co_running;
             stack_switch_call((void *)(current->stack+STACK_SIZE), (void *)coroutine_wrapper, (uintptr_t)(current));
        }else if(current->status == co_waiting){
            current->status = co_running;
            longjmp(current->context,1);
        }
    }else{
            // don't do anything
    }
    return;
}
