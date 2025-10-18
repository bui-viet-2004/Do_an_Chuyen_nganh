Tổng hợp đầy đủ các lệnh AT của module HM-10 (Bluetooth BLE)

I. Lệnh cơ bản

AT: Kiểm tra kết nối → OK

AT+RESET: Khởi động lại module → OK+RESET

AT+VERSION?: Xem phiên bản firmware → OK+VERSION:HMSoftVxxxx

AT+ADDR?: Xem địa chỉ MAC → OK+ADDR:20CD39A9C1B7

AT+HELP: Liệt kê danh sách các lệnh AT khả dụng → Danh sách lệnh

AT+STATE?: Kiểm tra trạng thái kết nối Bluetooth → OK+STATE:x

II. Lệnh cấu hình cơ bản

AT+NAME?: Xem tên thiết bị → OK+NAME:HMSoft

AT+NAME=MyHM10: Đổi tên thiết bị → OK+Set:MyHM10

AT+ROLE?: Xem chế độ hoạt động (Peripheral/Central) → OK+ROLE:0

AT+ROLE0: Đặt chế độ Peripheral (Slave) → OK+Set:0

AT+ROLE1: Đặt chế độ Central (Master) → OK+Set:1

AT+BAUD?: Xem tốc độ baud UART → OK+BAUD:4

AT+BAUDx: Đặt tốc độ baud (0--8) → OK+Set:x

Bảng Baud Rate

  Giá trị   Tốc độ (bps)
  --------- --------------
  0         9600
  1         19200
  2         38400
  3         57600
  4         115200
  5         4800
  6         2400
  7         1200
  8         230400

III. Lệnh điều khiển kết nối

AT+CONN?: Kiểm tra trạng thái kết nối → OK+CONN:x

AT+CONN`<addr>`{=html}: Kết nối tới thiết bị có địa chỉ MAC cụ thể →
OK+CONN

AT+DISC: Ngắt kết nối hiện tại → OK+LOST

AT+IMME?: Kiểm tra chế độ tự kết nối khi khởi động → OK+IMME:0

AT+IMME1: Bật chế độ chỉ kết nối khi gửi AT+CONN → OK+Set:1

AT+IMME0: Tự động kết nối khi bật nguồn → OK+Set:0

IV. Cấu hình mật khẩu & bảo mật

AT+PASS?: Xem mã PIN hiện tại → OK+PASS:000000

AT+PASS123456: Đặt mã PIN mới → OK+Set:123456

AT+TYPE?: Xem chế độ xác thực → OK+TYPE:0

AT+TYPE0: Không yêu cầu xác thực (mặc định) → OK+Set:0

AT+TYPE1: Yêu cầu xác thực đơn giản → OK+Set:1

AT+TYPE2: Yêu cầu nhập PIN khi ghép đôi → OK+Set:2

V. Lệnh điều khiển & nâng cao

AT+PIOx: Điều khiển chân PIO (GPIO) → OK+PIOx

AT+SLEEP: Chuyển sang chế độ tiết kiệm năng lượng → OK+SLEEP

AT+START: Bắt đầu chế độ truyền dữ liệu → OK+START

AT+EXIT: Thoát chế độ AT, chuyển sang Data Mode → OK+EXIT

AT+RSSI?: Xem cường độ tín hiệu kết nối (nếu có) → OK+RSSI:-60

VI. Kiểm tra nhanh

AT → OK\
AT+NAME? → OK+NAME:HMSoft\
AT+BAUD? → OK+BAUD:4\
AT+VERSION? → OK+VERSION:HMSoftV540\
AT+PASS? → OK+PASS:123456\
AT+TYPE? → OK+TYPE:2
