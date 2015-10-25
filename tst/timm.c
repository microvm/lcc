void hello(char *name) {
    char *h = "hello";
}

int main(int argc, char *argv[]) {
    if (argc < 2)
        return 1;
    hello(argv[1]);
    return 0;
}
