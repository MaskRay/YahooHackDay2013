#include <algorithm>
#include <getopt.h>
#include <iostream>
#include <map>
#include <memory>
#include <mongo/client/dbclient.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>
using namespace std;
using namespace mongo;

#define FOR(i, a, b) for (int i = (a); i < (b); i++)
#define REP(i, n) FOR(i, 0, n)

typedef long double ld;

bool proficiency = false;

map<string, string> repo2lang;
map<string, int> user2id, repo2id;
map<pair<string, string>, int> userlang2id;
vector<string> id2user, id2repo;
vector<pair<string, string>> id2userlang;
vector<pair<pair<int, int>, int> > contrib;
vector<int> userCommits, repoCommits;

vector<ld> userV, repoV;

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

  auto getUserLangId = [&](string name, string lang) -> int {
    pair<string, string> nl(name, lang);
    if (userlang2id.count(nl))
      return userlang2id[nl];
    int t = int(userlang2id.size());
    userlang2id[nl] = t;
    id2userlang.push_back(nl);
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

  if (proficiency) {
    std::shared_ptr<DBClientCursor> cur = conn.query("codersearch.repositories");
    while (cur->more()) {
      auto p = cur->next();
      string repo = p.getStringField("full_name"), lang = p.getStringField("language");
      if (lang != "")
        repo2lang[repo] = lang;
    }
  }

  std::shared_ptr<DBClientCursor> cur = conn.query("codersearch.contributes");
  while (cur->more()) {
    auto p = cur->next();
    string user = p.getStringField("login"), repo = p.getStringField("repository"), lang;
    if (proficiency) {
      if (! repo2lang.count(repo)) continue;
      lang = repo2lang[repo];
    }
    int commits = p.getIntField("commits");
    int uid = ! proficiency ? getUserId(user) : getUserLangId(user, lang);
    int rid = getRepoId(repo);
    contrib.push_back(make_pair(make_pair(rid, uid), commits));
    userCommits[uid] += commits;
    repoCommits[rid] += commits;
  }
}

void hits(int niter, int nuser, int nrepo)
{
  userV.assign(nuser, ld(1) / nuser);
  repoV.assign(nrepo, ld(1) / nrepo);

  REP(iter, niter) {
    printf("iteration %d\n", iter);

    fill(userV.begin(), userV.end(), ld(0));
    for (auto &e: contrib) {
      int x = e.first.first, y = e.first.second, z = e.second;
      if (repoCommits[x])
        userV[y] += repoV[x] * ld(z) / repoCommits[x];
    }
    ld invTot = ld(1) / accumulate(userV.begin(), userV.end(), ld(0));
    REP(i, nuser)
      userV[i] *= invTot;

    fill(repoV.begin(), repoV.end(), ld(0));
    for (auto &e: contrib) {
      int x = e.first.first, y = e.first.second, z = e.second;
      if (userCommits[y])
        repoV[x] += userV[y] * ld(z) / userCommits[y];
    }
    invTot = ld(1) / accumulate(repoV.begin(), repoV.end(), ld(0));
    REP(i, nrepo)
      repoV[i] *= invTot;
  }
}

char *progname;
void usage()
{
  printf("Usage: %s <options>\n", progname);
  printf("Options:\n");
  printf("  --host -h STRING\n");
  printf("  --port -p NUM\n");
  printf("  -i, --niter=NUM     number of iterations, default is 30\n");
  printf("  -l, --language-proficiency\n");
}

int main(int argc, char *argv[])
{
  progname = argv[0];
  static struct option long_options[] = {
    {"host",  required_argument,  NULL, 'h'},
    {"port",  required_argument,  NULL, 'p'},
    {"niter", required_argument,  NULL, 'i'},
    {"proficiency", required_argument,  NULL, 'p'},
    {0,     0,          0,    0}
  };
  ld factor_d = 0.85;
  int niter = 30;
  int opt;

  while ((opt = getopt_long(argc, argv, "h:p:i:l", long_options, NULL)) != -1) {
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
      case 'l':
        proficiency = true;
        break;
      default:
        usage();
        return 1;
    }
  }

  fetchData();
  if (! proficiency)
    hits(niter, user2id.size(), repo2id.size());
  else
    hits(niter, userlang2id.size(), repo2id.size());

  if (! proficiency) {
    puts("---user---");
    REP(i, user2id.size())
      printf("%d %s %.10Lf\n", i, id2user[i].c_str(), userV[i]);
  } else {
    puts("---user lang---");
    REP(i, userlang2id.size())
      printf("%d %s %s %.10Lf\n", i, id2userlang[i].first.c_str(), id2userlang[i].second.c_str(), userV[i]);
  }

  puts("---repo---");
  REP(i, repo2id.size())
    printf("%d %.10Lf %s\n", i, repoV[i], id2repo[i].c_str());

  return 0;
}
