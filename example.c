const int c = 9;

int main() {
	const int b = 0xa;
	int a = 0;
	for (int i = 0; i < 10; ++i) {
		a += 3;
	}
	return a + b + c;
}
