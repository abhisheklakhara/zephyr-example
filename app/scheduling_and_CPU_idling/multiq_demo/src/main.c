#include <stdio.h>
#include <zephyr/kernel.h>


#define STACK_SIZE 512

// Thread functions
void high_prio(void *a, void *b, void *c)
{
    while (1) {
        printk("High Priority Thread Running (prio 1)\n");
        k_sleep(K_MSEC(2000));
    }
}

void medium_prio(void *a, void *b, void *c)
{
    while (1) {
        printk("Medium Priority Thread Running (prio 3)\n");
        k_sleep(K_MSEC(2000));
    }
}

void low_prio(void *a, void *b, void *c)
{
    while (1) {
        printk("Low Priority Thread Running (prio 5)\n\n");
        k_sleep(K_MSEC(2000));
    }
}

// Allocate stacks and thread objects
K_THREAD_STACK_DEFINE(stack_hi, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_med, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_low, STACK_SIZE);

struct k_thread t_hi;
struct k_thread t_med;
struct k_thread t_low;

int  main(void)
{
    printk("Starting Multi-Queue Scheduler Demo (CONFIG_SCHED_MULTIQ)\n");

    // Lower priority = higher importance
    k_thread_create(&t_low, stack_low, STACK_SIZE,
                    low_prio, NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);  // Lower priority (less important)

    k_thread_create(&t_med, stack_med, STACK_SIZE,
                    medium_prio, NULL, NULL, NULL,
                    3, 0, K_NO_WAIT);  // Medium priority

    k_thread_create(&t_hi, stack_hi, STACK_SIZE,
                    high_prio, NULL, NULL, NULL,
                    1, 0, K_NO_WAIT);  // Highest priority
return 0;
}

