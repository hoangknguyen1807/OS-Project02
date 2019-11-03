# Đồ án 2, môn học Hệ điều hành
## Phần 1: Mục tiêu hiểu về Linux kernel module và hệ thống quản lý file và device trong linux, giao tiếp giữa tiến trình ở user space và kernel space.
- Viết một module dùng để tạo ra số ngẫu nhiên.
- Module này sẽ tạo một character device để cho phép các tiền trình ở user space có thể open và read các số ngẫu nhiên
## Phần 2: Chương trình hook vào một system call:
- syscall open =>ghi vào dmesg tên tiến trình mở file và tên file được mở
- syscall write => ghi vào dmesg tên tiến trình, tên file bị ghi và số byte được ghi