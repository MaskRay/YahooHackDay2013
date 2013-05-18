#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <utility>
using namespace std;

const int N = 250000;
const int MAX_QUESTION_LEN = 1000;
int tot = 1, tail = 0;
struct Node
{
  int l, f, c[256], ans;
};
Node *a = new Node[N * 2];
char answer[N];

void add(int c, int ans)
{
  int p = tail, np = tot++;
  memset(&a[np], -1, sizeof(Node));
  a[np].l = a[p].l + 1;
  a[np].ans = ans;
  tail = np;
  for (; p != -1 && a[p].c[c] == -1; p = a[p].f)
    a[p].c[c] = np;
  if (p == -1)
    a[np].f = 0;
  else if (a[a[p].c[c]].l == a[p].l + 1)
    a[np].f = a[p].c[c];
  else {
    int q = a[p].c[c], r = tot++;
    memcpy(&a[r], &a[q], sizeof(Node));
    a[r].l = a[p].l + 1;
    a[q].f = a[np].f = r;
    for (; p != -1 && a[p].c[c] == q; p = a[p].f)
      a[p].c[c] = r;
  }
}

pair<int, const char *> query(const char *q)
{
  int p = 0, x = 0, opt = 0, ans = 0;
  int32_t c = 0;
  for (; *q; q++) {
    if (a[p].c[*q] != -1) {
      p = a[p].c[*q];
      x++;
    } else {
      while (p != -1 && a[p].c[*q] == -1)
        p = a[p].f;
      if (p != -1) {
        x = a[p].l + 1;
        p = a[p].c[*q];
      } else {
        x = 0;
        p = 0;
      }
    }
    if (x > opt) {
      opt = x;
      ans = a[p].ans;
    }
  }
  return make_pair(opt, answer + ans);
}

int main()
{
  const int M = 10000;
  static char question[M + 1];
  int n;
  scanf("%d", &n);
  int nans = 0;
  memset(&a[0], -1, sizeof(Node));
  while (scanf("%s%s", question, answer + nans) == 2) {
    for (char *q = question; ; q++) {
      add(*q, nans);
      if (! *q) break;
    }
    nans += int(strlen(answer + nans)) + 1;
  }

  struct sockaddr_in servAddr, cliAddr;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    return perror(""), 1;

  bzero(&servAddr, sizeof servAddr);
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = INADDR_ANY;
  servAddr.sin_port = htons(5000);

  int one = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);
  if (bind(sockfd, (struct sockaddr *)&servAddr, sizeof servAddr) < 0)
    return perror(""), 1;

  char buf[MAX_QUESTION_LEN + 1];
  listen(sockfd, 1);
  for(;;) {
    socklen_t cliLen = sizeof cliAddr;
    int client = accept(sockfd, (struct sockaddr *)&cliAddr, &cliLen);
    if (client < 0)
      return perror(""), 1;
    int n = read(client, buf, MAX_QUESTION_LEN);
    if (n <= 0) {
      close(client);
      continue;
    }
    buf[n] = '\0';
    auto res = query(buf);
    write(client, res.second, strlen(res.second));
    close(client);
  }
}
