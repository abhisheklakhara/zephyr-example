#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <inttypes.h>

#define SW0_NODE DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS_OKAY(SW0_NODE)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static struct gpio_callback button_cb_data;

/* Simulate irq_disable by disabling GPIO interrupt */
void disable_button_interrupt(void)
{
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_DISABLE);
    printk(">> Button interrupt DISABLED (irq_disable simulated)\n");
}

void enable_button_interrupt(void)
{
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    printk(">> Button interrupt ENABLED (irq_enable simulated)\n");
}

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());

    disable_button_interrupt();

    /* Simulate ISR processing delay */
    k_sleep(K_MSEC(20000)); // During this time, button won't trigger

    enable_button_interrupt();
}

int main(void)
{
    int ret;

    if (!gpio_is_ready_dt(&button)) {
        printk("Button device not ready\n");
        return 0;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);

    printk("Button setup complete. Press to test irq_disable simulation.\n");

    while (1) {
        k_msleep(1000);
    }

    return 0;
}

