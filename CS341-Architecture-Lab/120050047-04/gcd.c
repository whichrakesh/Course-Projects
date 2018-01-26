// int gcd(int x, int y) {
//   if(x < y) {
//     int temp = x;
//     x = y;
//     y = temp;
//   }
//   do {
//     int remainder = x%y;
//     x = y;
//     y = remainder;
//   } while(y != 0);
//   return x;
// } // End gcd()

int gcd(int x, int y) {
  for(;;) {
    if(x == y) return x;
    if(x > y) { x = x-y; }
    else { y = y-x; }
  }
}

int result;

int main(int argc, char *argv[]) {
  int x = 48;
  int y = 15;
  result = gcd(x, y);
  syscall_print_int(result);
  syscall_print_newline();
  return 0;
} // End main()
