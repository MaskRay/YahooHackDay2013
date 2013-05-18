#include <algorithm>
#include <getopt.h>
#include <iostream>
#include <map>
#include <memory>
#include <mongo/client/dbclient.h>
#include <stdio.h>
#include <string>
#include <tuple>
#include <unistd.h>
#include <vector>
using namespace std;
using namespace mongo;

#define FOR(i, a, b) for (int i = (a); i < (b); i++)
#define REP(i, n) FOR(i, 0, n)

typedef long double ld;

map<string, int> user2id, repo2id;
vector<string> id2user, id2repo;
vector<tuple<int, int, int> > contrib;
vector<int> userCommits, repoCommits;

string host = "localhost", port = "27017";

void fetchData()
{
  mongo::DBClientConnection conn;
  printf("connecting to %s:%s ...\n", host.c_str(), port.c_str());
  conn.connect(host + ":" + port);
  printf("connected.\n");

  auto getUserId = [&](string name) -> int {
    if (user2id.count(name))
      return user2id[name];
    int t = int(user2id.size());
    user2id[name] = t;
    id2user.push_back(name);
    userCommits.resize(t + 1);
    return t;
  };

  auto getRepoId = [&](string name) -> int {
    if (repo2id.count(name))
      return repo2id[name];
    int t = int(repo2id.size());
    repo2id[name] = t;
    id2repo.push_back(name);
    repoCommits.resize(t + 1);
    return t;
  };

  std::shared_ptr<DBClientCursor> cur = conn.query("codersearch.contributes");
  while (cur->more()) {
    auto p = cur->next();
    string user = p.getStringField("login"), repo = p.getStringField("repository");
    int commits = p.getIntField("commits");
    int uid = getUserId(user), rid = getRepoId(repo);
    contrib.push_back(make_tuple(rid, uid, commits));
    userCommits[uid] += commits;
    repoCommits[rid] += commits;
  }
}

void hits(int niter)
{
  ld *userV = new ld[user2id.size()], *repoV = new ld[repo2id.size()];
  fill_n(userV, user2id.size(), ld(1) / user2id.size());
  fill_n(repoV, repo2id.size(), ld(1) / repo2id.size());

  REP(iter, niter) {
    printf("iteration %d\n", iter);

    fill_n(userV, user2id.size(), ld(0));
    for (auto &e: contrib) {
      int x = get<0>(e), y = get<1>(e), z = get<2>(e);
      if (repoCommits[x])
        userV[y] += repoV[x] * ld(z) / repoCommits[x];
    }
    ld invTot = ld(1) / accumulate(userV, userV + user2id.size(), ld(0));
    REP(i, user2id.size())
      userV[i] *= invTot;

    fill_n(repoV, repo2id.size(), ld(0));
    for (auto &e: contrib) {
      int x = get<0>(e), y = get<1>(e), z = get<2>(e);
      if (userCommits[y])
        repoV[x] += userV[y] * ld(z) / userCommits[y];
    }
    invTot = ld(1) / accumulate(repoV, repoV + repo2id.size(), ld(0));
    REP(i, repo2id.size())
      repoV[i] *= invTot;
  }

  puts("---user---");
  REP(i, user2id.size())
    printf("%d %.10Lf %s\n", i, userV[i], id2user[i].c_str());

  puts("---repo---");
  REP(i, repo2id.size())
    printf("%d %.10Lf %s\n", i, repoV[i], id2repo[i].c_str());

  delete[] userV;
  delete[] repoV;
}

char *progname;
void usage()
{
  printf("Usage: %s <options>\n", progname);
  printf("Options:\n");
  printf("  --host -h STRING\n");
  printf("  --port -p NUM\n");
  printf("  -i, --niter=NUM     number of iterations, default is 30\n");
  printf("  -u, --ntopuser=NUM  show top NUM coders, default is 10\n");
  printf("  -r, --ntoprepo=NUM  show top NUM coders, default is 10\n");
}

int main(int argc, char *argv[])
{
  progname = argv[0];
  static struct option long_options[] = {
    {"host",  required_argument,  NULL, 'h'},
    {"port",  required_argument,  NULL, 'p'},
    {"niter", required_argument,  NULL, 'i'},
    {0,     0,          0,    0}
  };
  ld factor_d = 0.85;
  int niter = 30;
  int opt;

  while ((opt = getopt_long(argc, argv, "h:p:u:r:", long_options, NULL)) != -1) {
    switch (opt) {
      case 'h':
        host = optarg;
        break;
      case 'p':
        port = optarg;
        break;
      case 'i':
        niter = atoi(optarg);
        break;
      default:
        usage();
        return 1;
    }
  }

  fetchData();
  hits(niter);
  return 0;
}
