#include <stdio.h>
#include <zephyr/kernel.h>

#define STACK_SIZE 512

void high_priority_thread(void *a, void *b, void *c)
{
    while (1) {
        printk(" High Priority Thread Running\n");
        k_sleep(K_MSEC(2000));
    }
}

void medium_priority_thread(void *a, void *b, void *c)
{
    while (1) {
        printk(" Medium Priority Thread Running\n");
        k_sleep(K_MSEC(2000));
    }
}

void low_priority_thread(void *a, void *b, void *c)
{
    while (1) {
        printk(" Low Priority Thread Running\n\n");
        k_sleep(K_MSEC(2000));
    }
}

K_THREAD_STACK_DEFINE(high_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(med_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(low_stack, STACK_SIZE);

struct k_thread high_thread_data;
struct k_thread med_thread_data;
struct k_thread low_thread_data;

int main(void)
{
    printk("Starting Red-Black Tree Scheduler Demo (CONFIG_SCHED_SCALABLE)\n");

    k_thread_create(&low_thread_data, low_stack, STACK_SIZE,
                    low_priority_thread, NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);   // Lowest priority

    k_thread_create(&med_thread_data, med_stack, STACK_SIZE,
                    medium_priority_thread, NULL, NULL, NULL,
                    3, 0, K_NO_WAIT);   // Medium priority

    k_thread_create(&high_thread_data, high_stack, STACK_SIZE,
                    high_priority_thread, NULL, NULL, NULL,
                    1, 0, K_NO_WAIT);   // Highest priority
return 0;
}

