#ifndef UI_H
#define UI_H
#include <stdint.h>
#include "app_types.h"
void ui_invalidate(void);                                  /* bat buoc ve lai toan bo lan toi */
void ui_render(const SharedData_t *d, uint8_t blink);      /* chi ve phan thay doi */
#endif
