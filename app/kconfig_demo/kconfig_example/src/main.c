#include <zephyr/random/random.h>

//settings
static const int32_t sleep_ms = 1000;

int main(void)
{
	uint32_t rnd;

	while(1)
	{
		rnd = sys_rand32_get();
		printf("Random value: %u\n", rnd);
		k_msleep(sleep_ms);
	}
	return 0;
	
}
