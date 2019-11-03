# Random Number Generator Character Device Driver
Mã nguồn trong file **chardevdrv.c** dùng để biên dịch ra module là một character device driver,
có chức năng cho phép các tiến trình ở user space mở và đọc một số ngẫu
nhiên từ file thiết bị của device này.
# Hướng dẫn sử dụng
1. Mở Terminal, gõ lệnh `make` , file chardevdrv.ko sẽ được tạo ra
2. Gõ lệnh `modinfo chardevdrv.ko` để xem thông tin của module
3. Gõ lệnh `sudo insmod chardevdrv.ko` để cài đặt module driver này
4. Gõ lệnh `lsmod | grep chardevdrv` để xem module đã được cài đặt thành công chưa
5. Để đọc một số ngẫu nhiên từ device này, ta dùng lệnh sau :
```
sudo dd if=/dev/randnum bs=4 count=1 | hexdump -C
```
>Lệnh trên đọc 4 byte từ file thiết bị `/dev/randnum` và xuất ra màn hình Terminal dưới dạng kí tự hexa (dùng lệnh `hexdump`). Chạy lệnh này nhiều lần, mỗi lần sẽ cho ra các kết quả ngẫu nhiên khác nhau.
6. Để gỡ module này, gõ lệnh `sudo rmmod chardevdrv`
7. Để dọn sạch các file được tạo ra trong thư mục trong quá trình biên dịch, gõ `make clean`
