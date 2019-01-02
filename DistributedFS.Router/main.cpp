// CSV to ASCII Table :D

#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <fstream>

#if defined(__cplusplus) && __cplusplus < 199711L
#define nullptr NULL
#endif

struct Arguments {
	char* csvfin;
	char* asciifout;
	bool header;

	bool in_stdin;
	bool out_stdout;
	bool out_stderr;

	int max_chars_per_column;
	int column_count;
	bool overflow_behavior; // true = next line, false = truncate end

	inline Arguments() :
		csvfin(nullptr),
		asciifout(nullptr),
		header(false),
		in_stdin(false),
		out_stdout(false),
		out_stderr(false),
		max_chars_per_column(0),
		column_count(0),
		overflow_behavior(false)
	{}
};

class Buffer {
	char* buf;
	size_t allocated;
public:
	inline char* operator()() { return buf; }
	inline void allocate(size_t n) { if (n == allocated) return;  if (!buf) { buf = new char[n]; allocated = n; } else { char* b = new char[n]; memcpy((void*)b, (void*)buf, allocated * sizeof(*buf)); delete[] buf; allocated = n; } }
	inline void free() { if (buf) { delete[] buf; allocated = 0; } }
	inline size_t size() { return allocated * sizeof(*buf); }
	inline size_t elements() { return allocated; }
	inline Buffer() : buf(nullptr), allocated(0) {}
	inline ~Buffer() { if (buf) delete[] buf; }
};

static Arguments Options;

void Help();
int ParseArguments(int argc, char** argv);
int ProcessCSV(std::istream& stream, std::vector<std::vector<std::string> > & table, std::vector<unsigned int>& lengths);
int BuildTable(FILE* stream, std::vector<std::vector<std::string> > & table);
int main(int argc, char** argv);


int main(int argc, char** argv) {
	int ret;
	if ((ret = ParseArguments(argc, argv)) != 0)
		return ret;

	std::vector<std::vector<std::string> > table;		// Holds the table data from the csv
	std::vector<unsigned int> max_lengths;

	std::ifstream fin(Options.csvfin);
	if (!fin.is_open()) {
		printf("Error: could not open file %s\n", Options.csvfin);
		return -1;
	}
	ProcessCSV(fin, table, max_lengths);

	return 0;
}

int ProcessCSV(std::istream& stream, std::vector<std::vector<std::string> >& table, std::vector<unsigned int>& max_lengths) {
	std::string buffer;
	while (std::getline(stream, buffer)) {
		std::vector<std::string> columns;
		size_t last_comma = 0;
		for (size_t i = 0; i < buffer.size(); i++) {
			if (buffer[i] == ',') {
				// buffer[last_comma + 1:i - 1]
				columns.push_back(buffer.substr(last_comma, i - last_comma));
				std::cout << columns.back() << std::endl;
				last_comma = i + 1;

				if (max_lengths.size() < i + 1) {
					max_lengths.push_back(columns.back().size());
				}

				max_lengths[i] = (max_lengths[i] < columns.back().size()) ? columns.back().size() : max_lengths[i];
			}
		}
		table.push_back(columns);
	}

	return 1;
}

int ParseArguments(int argc, char** argv) {
	if (argc < 2) {
		Help();
	}
	
	int current = 0;
	while (++current < argc) {
		printf("Processing argument %s\n", argv[current]);
		if (
			!strcmp(argv[current], "-i") ||
			!strcmp(argv[current], "--in-file")
			) {
			if (++current >= argc) {
				printf("Error: expected filename after %s\n", argv[current - 1]);
				Help();
				return -1;
			}
			else {
				Options.csvfin = argv[current];
			}
		}
		else if (
			!strcmp(argv[current], "-o") ||
			!strcmp(argv[current], "--in-file")
			) {
			if (++current >= argc) {
				printf("Error: expected a filename after %s\n", argv[current - 1]);
				Help();
				return -1;
			}
			else {
				Options.asciifout = argv[current];
			}
		}
		else if (!strcmp(argv[current], "--in-stdin")) {
			Options.in_stdin = true;
		}
		else if (!strcmp(argv[current], "--out-stdout")) {
			if (Options.out_stderr) {
				printf("Error: already selected --out-stderr\n (%s)", argv[current]);
				Help();
				return -1;
			}
			else {
				Options.out_stdout = true;
			}
		}
		else if (!strcmp(argv[current], "--out-stderr")) {
			if (Options.out_stdout) {
				printf("Error: already selected --out-stdout\n (%s)", argv[current]);
				Help();
				return -1;
			}
			else {
				Options.out_stderr = true;
			}
		}
		else if (
			!strcmp(argv[current], "-c") ||
			!strcmp(argv[current], "--max-column-width")
			) {
			if (++current >= argc) {
				printf("Error: expected an integer after %s\n", argv[current - 1]);
				Help();
				return -1;
			}
			else {
				Options.column_count = atoi(argv[current]);
			}
		}
		else if (
			!strcmp(argv[current], "-on") ||
			!strcmp(argv[current], "--overflow-nextline")
			) {
			Options.overflow_behavior = true;
		}
		else if (
			!strcmp(argv[current], "-ot") ||
			!strcmp(argv[current], "--overflow-truncate")
			) {
			Options.overflow_behavior = false;
		}
		else if (
			!strcmp(argv[current], "-h") ||
			!strcmp(argv[current], "--headers")
			) {
			Options.header = true;
		}
		else if (
			!strcmp(argv[current], "-?") ||
			!strcmp(argv[current], "--help")
			) {
			Help();
			return -1;
		}
	}
}

void Help() {
	printf("Help menu: csv2ascii table generator, v9000.0\n");
	printf("Options:\n");
	printf("    -i      --in-file           Input File, followed by the filename (of type CSV)\n");
	printf("    -o      --out-file          Output File, followed by the filename\n");
	printf("            --in-stdin          Input from stdin\n");
	printf("            --out-stdout        Output to stdout\n");
	printf("            --out-stderr        Output to stderr\n");
	printf("    -c      --max-column-width  Maximum width of a column\n");
	printf("    -o[n/t] --overflow=nextline / --overflow=truncate  Overflow behaviour of cells\n");
	printf("    -h      --headers           First line is headers\n");
	printf("[not supported yet]-n      --column-count      Number of columns, if lines are not split by a newline\n");
	printf("    -?      --help        Help menu\n");
}