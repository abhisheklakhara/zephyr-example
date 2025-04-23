#include <stdio.h>
#include <zephyr/kernel.h>


#define STACK_SIZE 512

#define HIGH_PRIORITY 2
#define LOW_PRIORITY 5

void high_priority_thread(void *a, void *b, void *c)
{
    while (1) {
        printk(" High-priority thread is running\n");
        k_sleep(K_MSEC(1000));
    }
}

void low_priority_thread(void *a, void *b, void *c)
{
    while (1) {
        printk(" Locking scheduler in low-priority thread\n");
        k_sched_lock();  

        for (int i = 0; i < 5; i++) {
            printk("Working in critical section (%d)\n", i);
            k_sleep(K_MSEC(200)); 
        }

        k_sched_unlock();  
	printk(" Scheduler unlocked\n");

        k_sleep(K_MSEC(1000));  
    }
}

K_THREAD_STACK_DEFINE(high_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(low_stack, STACK_SIZE);
struct k_thread high_thread_data;
struct k_thread low_thread_data;

int main(void)
{
    printk(" Demo: Scheduler Locking\n");

    k_thread_create(&low_thread_data, low_stack, STACK_SIZE,
                    low_priority_thread, NULL, NULL, NULL,
                    LOW_PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&high_thread_data, high_stack, STACK_SIZE,
                    high_priority_thread, NULL, NULL, NULL,
                    HIGH_PRIORITY, 0, K_NO_WAIT);
return 0;
}

