#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <aos/aos.h>
#include <alsa/snd.h>
#include <alsa/pcm.h>

#define TAG "card"


// int snd_card_new(struct device *parent)
int snd_card_new(int *parent, int idx, const char *xid, struct module *module, int size, snd_card_drv_t **card_ret)
{

    return 0;
}

int snd_card_free(snd_card_drv_t *card)
{

    return 0;
}

int aos_card_attach(const char *name, card_dev_t **card)
{
    *card = (card_dev_t *)device_open(name);
    return *card > 0? 0 : -1;
}

int aos_card_lpm(const char *name ,int state)
{
    char *dev_name  = strdup(name);
    int len         = strlen(name);
    int id          = dev_name[len-1] - 0x30;
    dev_name[len-1] = 0;

    card_dev_t *card = (card_dev_t *)device_find(dev_name, id);

    device_lpm((aos_dev_t*)card, state);

    aos_free(dev_name);

    return 0;
}