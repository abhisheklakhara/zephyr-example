#include <stdio.h>
#include <zephyr/kernel.h>


#define STACK_SIZE 512
#define PRIORITY 3  // Same priority for both threads

void thread1(void *a, void *b, void *c)
{
    while (1) {
        printk(" Thread 1 running\n");
       // k_yield();  // Yield to give Thread 2 a chance
    }
}

void thread2(void *a, void *b, void *c)
{
    while (1) {
        printk(" Thread 2 running\n");
        //k_yield();  // Yield to give Thread 1 a chance
    }

}

K_THREAD_STACK_DEFINE(stack1, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack2, STACK_SIZE);
struct k_thread t1, t2;

void main(void)
{
    printk("Demo of k_yield()\n");

    k_thread_create(&t1, stack1, STACK_SIZE,
                    thread1, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&t2, stack2, STACK_SIZE,
                    thread2, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);
}

