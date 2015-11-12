#include <fstream>
#include <string>
#include <iostream>

using namespace std;

#define panic_if(cond, ...) if(cond) { printf(__VA_ARGS__); char i; cin >> i; exit(1); }

void sseek(fstream &fs, string s) {
	char buf[1024];
	while (fs.good()) {
		fs.getline(buf, 1024);
		if (s.compare(buf) == 0)
			return;
	}
}

string ts("//%TOP_START"), te("//%TOP_END"), bs("//%BOT_START"), be("//%BOT_END");

int main(int argc, char *argv[]) {
	panic_if(argc < 3, "Usage: %s <input file> <output file>\n", argv[0]);

	int cnt;
	fstream ifs(argv[1], fstream::in);
	panic_if(!ifs, "%s could not be oppened\n", argv[1]);
	fstream ofs(argv[2], fstream::in);
	panic_if(!ifs, "%s could not be oppened\n", argv[2]);
	fstream tmpfs("tmp", fstream::in | fstream::out | fstream::trunc);
	tmpfs << ofs.rdbuf();
	ofs.close();
	ofs.open(argv[2], fstream::out | fstream::trunc);
	ofs.write("", 0);
	panic_if(!ifs || !ofs, "something went wrong\n");
	char buf[1024] = { 0 };
	sseek(ifs, ts);
	tmpfs.clear();
	tmpfs.seekg(0);
	while (tmpfs.good()) {
		tmpfs.getline(buf, 1024);
		cnt = tmpfs.gcount();
		ofs.write(buf, cnt - 1);
		ofs.write("\n", 1);
		if (ts.compare(buf) == 0)
			break;
	}

	sseek(tmpfs, te);
	while (ifs.good()) {
		ifs.getline(buf, 1024);
		cnt = ifs.gcount();
		ofs.write(buf, cnt - 1);
		ofs.write("\n", 1);
		if (te.compare(buf) == 0)
			break;
	}

	sseek(ifs, bs);
	while (tmpfs.good()) {
		tmpfs.getline(buf, 1024);
		cnt = tmpfs.gcount();
		ofs.write(buf, cnt - 1);
		ofs.write("\n", 1);
		if (bs.compare(buf) == 0)
			break;
	}

	sseek(tmpfs, be);
	while (ifs.good()) {
		ifs.getline(buf, 1024);
		cnt = ifs.gcount();
		ofs.write(buf, cnt - 1);
		if (be.compare(buf) == 0)
			break;
		if (ifs.eof())
			ofs.write(&buf[cnt - 1], 1);
		else
			ofs.write("\n", 1);
	}
	return 0;
}