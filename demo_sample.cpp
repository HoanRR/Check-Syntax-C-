
// ===== Global declarations =====
int X = 10;
int X = 20;          // [REDECL] trùng tên trong cùng scope

// Kiểu, const, pointer (hiện Parser nuốt được; Semantics chưa check sâu kiểu)
const char *greet = "Hello \"world\"!\n";
char ch = '\n';
int hexNum = 0x1F;
int octNum = 075;
double sci = 3.14e2; // 314.0

// Hàm hợp lệ để đối chiếu
int add(int a, int b) {
    int s = a + b;
    if (s >= 0) return s;
    else return -s;
}

// Hàm void trả về biểu thức -> lỗi E-RETVOID (đã có trong Semantics)
void badVoidReturn() {
    int t = 1;
    return t;        // [RETVOID]
}

// Hàm non-void nhưng return rỗng -> lỗi E-RETEMPTY
int badNonVoidReturn() {
    int y = 0;
    if (y > 0) {
        return 1;
    } else {
        y = y + 1   // [SYNTAX] thiếu ';' để test phục hồi lỗi
    }
    return;          // [RETEMPTY]
}

// Gọi hàm + biểu thức đủ loại
int main() {
    // ----- Undeclared identifier -----
    z = 5;           // [UNDECL] 'z' chưa khai báo (useIdent trong parsePrimary)

    // ----- If / While / Logical / Relational / Shift -----
    int a = 1, b = 2, c = 0;
    if ( (a && b) || !c ) {
        int a = 100;  // Hợp lệ do scope mới (không phải redeclare cùng scope)
        a += 1;
    }

    while (b < 10) {
        b << 1;      // shift-left
        b++;          // postfix ++
        a -= 3;
    }
    LOG(5);
    // ----- Gọi hàm hợp lệ -----
    int s1 = add(3, 4);

    // ----- Gọi hàm chưa khai báo (undeclared function) -----
    int s2 = foo(1, 2);   // [UNDECL] 'foo' chưa khai báo

    // ----- Chuỗi, char test thêm -----
    const char *s = "Line1\nLine2\t\"quote\"";
    char c1 = 'A';
    char c2 = '\t';

    // ----- Toán tử bằng nhau/khác -----
    int eq = (a == b);
    int ne = (a != b);

    // ----- Gán hợp chất -----
    int k = 5;
    k *= 2;
    k /= 3;
    k %= 2;

    return s1 + s2 + k;   // s2 dùng foo() sẽ khiến trình biên dịch thật lỗi, nhưng ta chỉ cần checker báo undeclared
}
