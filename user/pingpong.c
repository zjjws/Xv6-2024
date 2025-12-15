#include "kernel/types.h"
#include "user/user.h"

int 
main(int argc, char* argv[]) {
  char buf[] = {'o'};

  int p2s[2], s2p[2];
  pipe(p2s);  //parent to son
  pipe(s2p);  //son to parent

  if (fork() == 0) {
    close(p2s[1]);
    close(s2p[0]);
    if (read(p2s[0], buf, sizeof buf)) {
      printf("%d: received ping\n", getpid());
    }
    close(p2s[0]);
    write(s2p[1], buf, sizeof(buf));
    close(s2p[1]);
    exit(0);
  } else {
    close(p2s[0]);
    close(s2p[1]);
    write(p2s[1], buf, sizeof(buf));
    close(p2s[1]);
    wait(0);
    if (read(s2p[1], buf, sizeof(buf))) {
      printf("%d: received pong\n", getpid());
    }
    close(s2p[0]);
    exit(0);
  }
}