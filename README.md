# Mini C Compiler IDE

Một môi trường phát triển tích hợp (IDE) gọn nhẹ dành cho ngôn ngữ C, được xây dựng từ đầu bằng **C++** và **Qt6 Framework**. Dự án tập trung vào việc mô phỏng các giai đoạn cốt lõi của một trình biên dịch: Phân tích từ vựng (Lexing), Phân tích cú pháp (Parsing), Phân tích ngữ nghĩa (Semantic Analysis) và hỗ trợ gợi ý code thông minh (Intelligent Auto-completion).

## 🚀 Tính năng nổi bật

### 1. Trình biên dịch Front-end (Compiler Front-end)

- **Lexer:** Tokenize mã nguồn, xử lý các kiểu dữ liệu số (Hex, Octal, Float), chuỗi ký tự và comment (`//`, `/* */`).
- **Parser:** Sử dụng kỹ thuật **Recursive Descent Parsing** (Phân tích đệ quy xuống) dựa trên văn phạm EBNF. Hỗ trợ các cấu trúc:
  - Khai báo hàm, biến.
  - Cấu trúc điều khiển: `if`, `else`, `while`, `for`.
  - Biểu thức toán học và logic phức tạp.
- **Semantic Analysis:**
  - Quản lý Symbol Table với Scope (phạm vi biến) lồng nhau.
  - Phát hiện lỗi: Khai báo lại biến (Redeclaration), biến chưa khai báo, sai kiểu trả về của hàm (`void` vs có giá trị).
- **Preprocessor:** Xử lý chỉ thị `#include` để nhận diện các hàm thư viện chuẩn (`stdio.h`, `math.h`, v.v.).

### 2. Algorithmic Intelligence (Điểm nhấn)

Tính năng gợi ý code (Auto-completion) không chỉ dựa trên tiền tố (Prefix matching) đơn thuần mà còn xử lý **Fuzzy Matching** (Khớp gần đúng) để hỗ trợ người dùng khi gõ sai:

- **Cấu trúc dữ liệu:** Sử dụng **Trie** để lưu trữ từ khóa và định danh.
- **Thuật toán tìm kiếm:** Triển khai thuật toán **A\* Search (A-Star)** kết hợp với hàm heuristic khoảng cách chỉnh sửa (Edit Distance).
  - _Ví dụ:_ Gõ `prnt` có thể gợi ý `printf`.
  - Hệ thống tính toán chi phí (Cost) dựa trên thay thế, chèn, xóa ký tự để đưa ra gợi ý phù hợp nhất.

### 3. Giao diện người dùng (UI)

- Code Editor tích hợp số dòng và Syntax Highlighting (tô màu cú pháp) sử dụng Regular Expressions.
- Báo lỗi thời gian thực (Real-time error reporting) với cơ chế highlight dòng lỗi trực tiếp trong editor.

## 🛠 Cài đặt & Hướng dẫn Build

Dự án yêu cầu **Qt6** và trình biên dịch hỗ trợ **C++17**.

### Yêu cầu tiên quyết

- CMake (>= 3.16)
- Qt 6 (Components: Widgets)
- MinGW (GCC) hoặc MSVC

### Các bước Build

1.  Clone repository:
    ```bash
    git clone [https://github.com/username/Check-Syntax-C-.git](https://github.com/username/Check-Syntax-C-.git)
    cd Check-Syntax-C-
    ```
2.  Tạo thư mục build và chạy CMake:
    ```bash
    mkdir build
    cd build
    cmake .. -DCMAKE_PREFIX_PATH="path/to/Qt/6.x.x/mingw_64"
    cmake --build .
    ```

## 📂 Cấu trúc dự án

- **lexer/**: Bộ phân tích từ vựng (Tokenization).
- **parser/**: Bộ phân tích cú pháp (EBNF Grammar & Recursive Descent logic).
- **symboltable/**: Quản lý bảng ký hiệu và kiểm tra kiểu.
- **preprocessor/**: Xử lý các chỉ thị tiền xử lý.
- **UI/**: Giao diện đồ họa (MainWindow, CodeEditor, Highlighter).
- **Diagnostic/**: Quản lý và báo cáo lỗi.
- **trie.cpp/h**: Cài đặt thuật toán Trie và A\* Search.

## 📝 Grammar (EBNF)

Ngữ pháp của ngôn ngữ C con (subset) được định nghĩa chi tiết trong file [parser/EBND.txt](parser/EBND.txt).

---

_Dự án được thực hiện bởi [Tên của bạn] - Sinh viên chuyên ngành Khoa học Dữ liệu & AI._
