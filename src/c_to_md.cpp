#include <fstream>
#include <string>
#include <iostream>

using namespace std;

#define panic_if(cond, ...) if(cond) { printf(__VA_ARGS__); char i; cin >> i; exit(1); }

/// Advance fs until encountering a line that is s
/*!
	Calls fs.getline until the buffer returned matches s.
	\param[in] fs std::fstream to advance
	\param[in] s std::string to match against, max length of 1024
*/
void sseek(fstream &fs, string s) {
	char buf[1024];
	while (fs.good()) {
		fs.getline(buf, 1024);
		if (s.compare(buf) == 0)
			return;
	}
}

string start("//%BEGIN"); ///< Marker to begin copy 
string stop("//%END"); ///<  Marker to stop copy

///Copies parts of a C file into parts of a machine description file
/*!
	Takes two arguments: a C file and a md file. Both files are divided into two types of sections.
	The C file consists of sections to be ignored and sections that are copied to the md file. The latter is bounded by ::start and ::stop.
	The md file consists of sections that are maintained and sections that are overwriten with the contents of the C file. The latter is bounded by ::start and ::stop.
	While there can be any number of copied/overwriten sections the number should be the same for both files.
	A lot of the complexity in the while loop comes from my hatred of trailing newlines.
*/
int main(int argc, char *argv[]) {
	panic_if(argc < 3, "Usage: %s <input file> <output file>\n", argv[0]);

	int cnt;
	/// Open up the two files passed as arguments, and an additional temporary file.
	fstream ifs(argv[1], fstream::in);
	panic_if(!ifs, "%s could not be oppened\n", argv[1]);
	fstream ofs(argv[2], fstream::in);
	panic_if(!ifs, "%s could not be oppened\n", argv[2]);
	fstream tmpfs("tmp", fstream::in | fstream::out | fstream::trunc);
	/// Copy the contents of the md file into the temp file.
	tmpfs << ofs.rdbuf();

	/// Trucate the destination (md) file.
	ofs.close();
	ofs.open(argv[2], fstream::out | fstream::trunc);
	// i think fflush didn't work for some reason. This works.
	ofs.write("", 0);

	panic_if(!ifs || !ofs, "something went wrong\n");

	char buf[1024] = { 0 };

	tmpfs.clear();
	tmpfs.seekg(0);

	/// While either file still has contents to copy: 
	while (ifs.good() || tmpfs.good()) {
		/*!
		Move the input (C) fstream to the begining of the first section to copy
		and copy the first section to be maintained from the temp file to the destination (md) file.
		*/
		if (ifs.good())
			sseek(ifs, start);
		while (tmpfs.good()) {
			tmpfs.getline(buf, 1024);
			cnt = tmpfs.gcount();
			ofs.write(buf, cnt - 1);
			if (tmpfs.eof())
				ofs.write(&buf[cnt - 1], 1);
			else
				ofs.write("\n", 1);
			if (start.compare(buf) == 0)
				break;
		}

		/*!
		Move the temp fstream to the end of the section to be overwritten
		and copy the section to be copied from the input file to the destination file.
		*/
		if (tmpfs.good())
			sseek(tmpfs, stop);
		while (ifs.good()) {
			ifs.getline(buf, 1024);
			cnt = ifs.gcount();
			ofs.write(buf, cnt - 1);
			if (ifs.eof())
				ofs.write(&buf[cnt - 1], 1);
			if (tmpfs.good()) //there is still more to copy over
				ofs.write("\n", 1);
			if (stop.compare(buf) == 0)
				break;
		}
	}

	return 0;
}