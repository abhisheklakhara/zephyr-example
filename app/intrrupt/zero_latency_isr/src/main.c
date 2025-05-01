#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/irq.h>

void simulated_normal_isr(void)
{
    printk("[Normal ISR] Trying to execute...\n");

    if (k_is_in_isr()) {
        printk("[Normal ISR] Running in ISR context.\n");
    } else {
        printk("[Normal ISR] Running in thread context (delayed due to irq_lock).\n");
    }
}

void simulated_zero_latency_isr(void)
{
    printk(">>> [Zero Latency ISR] Executing immediately regardless of irq_lock!\n");
}

void thread_func(void *p1, void *p2, void *p3)
{
    printk("Thread started. Simulating irq_lock...\n");

    unsigned int key = irq_lock();  // Simulate entering critical section
    printk("irq_lock acquired. Kernel in critical section.\n");

    // Simulated ISR that obeys irq_lock
    printk("Simulating normal interrupt call (should be delayed)...\n");
    simulated_normal_isr();

    // Simulated ISR that bypasses irq_lock
    printk("Simulating zero-latency interrupt call...\n");
    simulated_zero_latency_isr();

    irq_unlock(key);
    printk("irq_lock released. Kernel out of critical section.\n");

    // End thread
    k_thread_suspend(k_current_get());
}

K_THREAD_DEFINE(sim_thread, 1024, thread_func, NULL, NULL, NULL, 5, 0, 0);

void main(void)
{
    printk("Main thread started. Waiting for simulation thread...\n");
}

