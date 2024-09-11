#include <execinfo.h>
#include <raylib.h>
#include <signal.h>
#include <string>
#include "RaylibWindow.hpp"
#include "cef_app.h"
#include "rlgl.h"

void segfault_sigaction(int signal, siginfo_t* si, void* arg) {
  std::string st = "Ligma browser crashed with \"";
  switch (signal) {
    case SIGILL:
      st += "Illegal instruction";
      break;
    case SIGBUS:
      st += "Bus error";
      break;
    case SIGFPE:
      st += "Floating-point exception";
      break;
    case SIGKILL:
      st += "Forced-process termination ";
      break;
    case SIGSEGV:
      st += "Invalid memory reference";
      break;
    case SIGPIPE:
      st += "Write to pipe with no readers ";
      break;
    case SIGSTKFLT:
      st += "Coprocessor stack error ";
      break;
    case SIGXCPU:
      st += "CPU time limit exceeded ";
      break;
    case SIGXFSZ:
      st += "File size limit exceeded";
      break;
    case SIGVTALRM:
      st += "Virtual timer clock";
      break;
    case SIGPROF:
      st += "Profile timer clock";
      break;
    case SIGPWR:
      st += "Power supply failure";
      break;
    case SIGSYS:
      st += "Bad system call";
      break;
  }
  st += "\"\n\nStacktrace:";
  void* callstack[128];
  int i, frames = backtrace(callstack, 128);
  char** strs = backtrace_symbols(callstack, frames);
  for (i = 0; i < frames; ++i) {
    st += std::format("\n{}", strs[i]);
  }
  free(strs);
  std::print(std::cout, "{}", st);
  exit(0);
}

int main(int argc, char* argv[]) {
  struct sigaction sa;
  memset(&sa, 0, sizeof(struct sigaction));
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = segfault_sigaction;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGSEGV, &sa, NULL);
  sigaction(SIGILL, &sa, NULL);
  sigaction(SIGBUS, &sa, NULL);
  sigaction(SIGFPE, &sa, NULL);
  sigaction(SIGKILL, &sa, NULL);
  sigaction(SIGSEGV, &sa, NULL);
  sigaction(SIGPIPE, &sa, NULL);
  sigaction(SIGSTKFLT, &sa, NULL);
  sigaction(SIGXCPU, &sa, NULL);
  sigaction(SIGXFSZ, &sa, NULL);
  sigaction(SIGVTALRM, &sa, NULL);
  sigaction(SIGPROF, &sa, NULL);
  sigaction(SIGPWR, &sa, NULL);

  RaylibWindow* window = new RaylibWindow(argc, argv);

  while (!WindowShouldClose()) {
    window->resizeTranslation();
    window->keyTranslation();
    window->mouseTranslation();
    window->resizeTranslation();
    window->render();
  }

  CefShutdown();
  return 0;
}
