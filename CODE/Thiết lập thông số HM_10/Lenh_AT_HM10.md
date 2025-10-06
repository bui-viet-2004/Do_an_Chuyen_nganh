# Danh sách lệnh AT của Module HM-10 (Bluetooth BLE)

## I. Lệnh AT cơ bản

AT: Kiểm tra kết nối → OK

AT+RESET: Khởi động lại module → OK+RESET

AT+VERSION?: Xem phiên bản firmware → OK+VERSION:xxxx

AT+ADDR?: Xem địa chỉ MAC Bluetooth → OK+ADDR:xxxx

AT+HELP: Danh sách lệnh hỗ trợ → Danh sách lệnh

## II. Lệnh cấu hình cơ bản

AT+NAME?: Xem tên thiết bị → OK+NAME:HMSoft

AT+NAME=MyHM10: Đặt lại tên thiết bị → OK+Set:MyHM10

AT+ROLE?: Xem chế độ hoạt động → OK+ROLE:0 (0=Peripheral, 1=Central)

AT+BAUD?: Xem tốc độ baud UART → OK+BAUD:4

AT+BAUD4: Đặt baud rate 9600 bps → OK+Set:4

## III. Lệnh cấu hình kết nối

AT+CONN?: Kiểm tra tình trạng kết nối → OK+CONN:x

AT+CONNxxxx: Kết nối đến địa chỉ MAC khác → OK+CONN

AT+DISC: Ngắt kết nối hiện tại → OK+LOST

AT+IMME1: Chỉ kết nối khi gửi lệnh AT+CONN → OK+Set:1

AT+IMME0: Tự động kết nối khi bật nguồn → OK+Set:0

## IV. Cấu hình mã PIN & bảo mật

AT+PASS?: Xem mã PIN → OK+PASS:000000

AT+PASS123456: Đổi mã PIN → OK+Set:123456

AT+TYPE?: Xem chế độ kết nối an toàn → OK+TYPE:0

## V. Lệnh nâng cao

AT+PIOx: Điều khiển chân PIO (GPIO) → OK+PIOx

AT+SLEEP: Chế độ tiết kiệm năng lượng → OK+SLEEP

AT+START: Bắt đầu truyền dữ liệu → OK+START

AT+EXIT: Thoát chế độ AT → OK+EXIT
