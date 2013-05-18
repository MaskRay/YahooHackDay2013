/*
 * $File: pagerank.cc
 * $Date: Thu May 16 07:45:05 2013 +0000
 * $Author: Xinyu Zhou <zxytim[at]gmail[dot]com>
 */

#include <unistd.h>
#include <getopt.h>
#include <cstdio>
#include <map>
#include <memory>
#include <cstring>
#include <string>
#include <iostream>
#include <mongo/client/dbclient.h>
#include <vector>
#include <algorithm>

using namespace std;
using namespace mongo;

typedef long double ld;

map<string, int> name2id;
vector<string> id2name;


string host = "localhost",
	   port = "27017";

string output;

struct Vertex {
	vector<int> to;
	int ind, outd;
	ld pagerank;
	int id;
	Vertex(int id) : id(id), ind(0), outd(0), pagerank(0) {}
	ld contrib() const {
		return pagerank / outd;
	}
};
struct Edge {
	int from, to;
	Edge(int from, int to) : from(from), to(to) {
	}
};
vector<Vertex> vtx;
vector<Edge> edge;

int nvtx = 0;

void fetch_data() {
	auto get_id = [&](string name) -> int {
		if (name2id.count(name))
			return name2id[name];
		else {
			name2id[name] = nvtx ++;
			id2name.push_back(name);
			return nvtx - 1;
		}

	}; 
	
	try {
		mongo::DBClientConnection conn;
		printf("connecting to %s:%s ...\n", host.c_str(), port.c_str());
		conn.connect(host + ":" + port);
		printf("connected.\n");

		printf("fetching data ...\n");
		int tot = conn.count("codersearch.follows");
		printf("follow relation count: %d\n", tot);
		std::shared_ptr<DBClientCursor> cusor = conn.query("codersearch.follows");
		int cnt = 0, nreport = max(1, tot / 10);

		while (cusor->more()) {
			auto p = cusor->next();
			string from = p.getStringField("from"),
				   to = p.getStringField("to");

			int id_from = get_id(from),
				id_to = get_id(to);

			while (id_from >= vtx.size() || id_to >= vtx.size())
				vtx.push_back(Vertex(vtx.size()));

			vtx[id_from].to.push_back(id_to);
			edge.push_back(Edge(id_from, id_to));

			cnt ++;
			if (cnt % nreport == 0)
				printf("progress[%.1lf%%]: %d/%d\n", cnt / double(tot) * 100, cnt, tot);
		}

	} catch (const mongo::DBException &e) {
		std::cout << "caught exception: " << e.what() << std::endl;
		exit(1);
	}
}

int ntop = 10;

void page_rank(ld factor_d, int niter) {
	ld *pr_next = new ld[nvtx]; // temp pagerank
	for (auto &e: edge) {
		vtx[e.from].outd ++;
		vtx[e.to].ind ++;
	}
	for (auto &v: vtx) {
		v.pagerank = ld(1.0) / nvtx;
	}

	for (int iter_id = 1; iter_id <= niter; iter_id ++) {
		printf("iteration %d ...\n", iter_id);
		ld deadend_val = 0;
		for (int i = 0; i < nvtx; i ++) {
			pr_next[i] = (1.0 - factor_d) / nvtx;
			if (vtx[i].outd == 0)
				deadend_val += vtx[i].pagerank / nvtx;
		}
		for (auto &e: edge)
			pr_next[e.to] += factor_d * vtx[e.from].contrib();
		for (int i = 0; i < nvtx; i ++)
			vtx[i].pagerank = pr_next[i] + deadend_val;
	}

	delete [] pr_next;

	printf("sorting ...\n");
	sort(vtx.begin(), vtx.end(), [](const Vertex &a, const Vertex &b) -> bool { return a.pagerank > b.pagerank; } );

	if (output != "") {
		FILE *f = fopen(output.c_str(), "w");
		for (int i = 0; i < nvtx; i ++) {
			fprintf(f, "%d %.10Lf %s\n", i, vtx[i].pagerank, id2name[vtx[i].id].c_str());
		}
		fclose(f);
	}

	if (ntop > 0) {
		printf("top %d coders:\n", ntop);
		for (int i = 0; i < ntop; i ++) {
			printf("%3d %Lf %s\n", i + 1, vtx[i].pagerank, id2name[vtx[i].id].c_str());
		}
	}
}


char *progname;
void print_usage() {
	printf("Usage: %s <options>\n", progname);
	printf("Options:\n");
	printf("    --host -h STRING    specify the host of mongodb to connect to, \n");
	printf("                        default is `localhost'\n");
	printf("    --port -p NUM       specify the port to connect to,\n");
	printf("                        default is 27017\n");
	printf("    --factord -d FLOAT  set the factor d stated in PageRank page in Wikipedia,\n");
	printf("                        default is 0.85\n");
	printf("    --niter -i NUM      number of iterations, default is 30\n");
	printf("    --ntop -t NUM       show top NUM coders, default is 10\n");
	printf("    --output -o STRING  output result to file\n");
	printf("    --help              show this help and quit\n");
}

int main(int argc, char *argv[]) {
	progname = argv[0];
	static struct option long_options[] = {
		{"host",	required_argument,	NULL,	'h'},
		{"port",	required_argument,	NULL,	'p'},
		{"factord", required_argument,	NULL,	'd'},
		{"niter",	required_argument,	NULL,	'i'},
		{"ntop",	required_argument,	NULL,	't'},
		{"help",	no_argument,		NULL,	0},
		{"output",	required_argument,	NULL,	'o'},
		{0,			0,					0,		0}
	};
	ld factor_d = 0.85;
	int niter = 30;
	int opt;

	while ((opt = getopt_long(argc, argv, "h:p:d:i:t:o:", long_options, NULL)) != -1) {
		switch (opt) {
			case '0':
				print_usage();
				return 0;
			case 'h':
				host = optarg;
				break;
			case 'p':
				port = optarg;
				break;
			case 'd':
				factor_d = atof(optarg);
				if (factor_d <= 0 || factor_d > 1) {
					fprintf(stderr, "factor d must be in range (0, 1]\n");
					exit(1);
				}
				break;
			case 'i':
				niter = atoi(optarg);
				if (niter < 1) {
					fprintf(stderr, "niter must greater than 0\n");
					exit(1);
				}
				break;
			case 't':
				ntop = atoi(optarg);
				if (ntop < 0) {
					fprintf(stderr, "ntop must be non-negative\n");
					exit(1);
				}
				break;
			case 'o':
				output = optarg;
				break;
			default:
				print_usage();
				return 1;
		}
	}
	fetch_data();
	page_rank(factor_d, niter);
	return 0;
}

/**
 * vim: syntax=cpp11 foldmethod=marker
 */

