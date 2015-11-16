void hello(char *name, int a) {
	char *h = "hello";
	name++;
}

int main(int argc, char *argv[]) {
	if (argc < 2)
		return 1;
	hello(argv[1], argc + 1);
	return 0;
}
