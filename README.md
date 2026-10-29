# BTL_He_Thong_Nhung
He thong nha thong minh da nhiem tren FreeRTOS - Hoc phan He thong nhung, Nhom 05.

## Nhanh
- main: ban on dinh, CAM commit truc tiep
- develop: nhanh tich hop chinh
- feature/<ten-task>: nhanh lam viec ca nhan
- integration/<phien-ban>: ghep he thong truoc khi release

## Bat dau lam viec
git checkout develop
git pull origin develop
git checkout -b feature/<ten-task>

## Ky luat
- Chi 1 nguoi generate code CubeMX, khong de len FreeRTOSConfig.h
- Trong ISR chi dung ham ...FromISR(); trong task dung vTaskDelay(), khong dung HAL_Delay()
- Khong sua main.c o nhanh task
- Commit message: feat(task-xxx), fix(rtos), refactor(...)
- Khong commit build/, Debug/, Release/
