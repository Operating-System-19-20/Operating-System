﻿-Tạo và nạp module:
+ tạo một thư mục gồm 4 file: Makefile, Kbuild, random.c //module , test.c //process
+ thực hiện các lệnh sau để nạp module
	cd path(chứa 4 file)
	sudo make
	sudo insmod random.ko
+ build file proccess
	sudo gcc test.c -o test
	sudo ./test
+ hoặc sử dụng lệnh để kết nối với driver
	int ret, fd, receive;
	fd = open("/dev/randomnumber", O_RDWR); // <0? lỗi
	ret = read(fd,&receive,1) //<0? lỗi giá trị trả về của module được lưu trong receive
+ xóa module
	sudo rmmod random
-Các bước thực hiện:
+ tạo một thư mục gồm 2 file: Hook.c, Makefile
+ thực hiện các bước sau để Hook
	cd path (chứa 2 file)
	sudo make
	sudo insmod hook.ko
+ kiểm tra việc Hook thành công chưa
	dmesg
