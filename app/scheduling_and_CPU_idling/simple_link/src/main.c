#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define STACK_SIZE 512
#define THREAD_PRIORITY 5

void thread_func1(void *arg1, void *arg2, void *arg3)
{
    while (1) {
        printk("Thread 1 is running\n");
        k_sleep(K_MSEC(5000));
    }
}

void thread_func2(void *arg1, void *arg2, void *arg3)
{
    while (1) {
        printk("Thread 2 is running\n");
        k_sleep(K_MSEC(5000));
    }
}

void thread_func3(void *arg1, void *arg2, void *arg3)
{
    while (1) {
        printk("Thread 3 is running\n");
        k_sleep(K_MSEC(5000));
    }
}

K_THREAD_STACK_DEFINE(thread1_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread2_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread3_stack, STACK_SIZE);

struct k_thread thread1_data;
struct k_thread thread2_data;
struct k_thread thread3_data;

int  main(void)
{
    printk("Starting with CONFIG_SCHED_SIMPLE...\n");

    k_thread_create(&thread1_data, thread1_stack, STACK_SIZE,
                    thread_func1, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&thread2_data, thread2_stack, STACK_SIZE,
                    thread_func2, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);

    k_thread_create(&thread3_data, thread3_stack, STACK_SIZE,
                    thread_func3, NULL, NULL, NULL,
                    THREAD_PRIORITY, 0, K_NO_WAIT);
return 0;
}

