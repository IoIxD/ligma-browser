#include "GUI.h"
#include "gtk/gtk.h"
#include "rlgl.h"
#include <execinfo.h>
#include <format>
#include <gobject/gsignal.h>
#include <iostream>

void segfault_sigaction(int signal, siginfo_t *si, void *arg);
void restartable_main();
void restart_callback();
void error_exit_callback();
void error_window(const char *err, bool recoverable);

int main(int argc, char *argv[]) {
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
  sigaction(SIGSYS, &sa, NULL);

  restartable_main();

  return 0;
}

void restartable_main() {
  try {
    auto gui = new Window();
    gui->main_thread();
  } catch (std::exception &e) {
    error_window(e.what(), true);
    exit(0);
  }
}

void error_window(const char *err, bool recoverable) {
  fork();
  gtk_main_quit();
  gtk_init(nullptr, nullptr);
  auto err_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(err_win), "Exception caught :(");
  gtk_widget_set_size_request(err_win, 256, 128);
  auto vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  auto label = gtk_label_new(err);
  gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
  gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 5);

  if (recoverable) {
    auto hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    auto cont = gtk_button_new_with_label("Restart");
    g_signal_connect(cont, "clicked", restart_callback, NULL);
    auto quit = gtk_button_new_with_label("Quit");

    g_signal_connect(quit, "clicked", error_exit_callback, NULL);
    gtk_box_pack_start(GTK_BOX(hbox), cont, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), quit, TRUE, TRUE, 5);
    gtk_box_pack_end(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
  }

  gtk_container_add(GTK_CONTAINER(err_win), vbox);
  gtk_widget_show_all(err_win);
  gtk_main();
}
void segfault_sigaction(int signal, siginfo_t *si, void *arg) {
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
  void *callstack[128];
  int i, frames = backtrace(callstack, 128);
  char **strs = backtrace_symbols(callstack, frames);
  for (i = 0; i < frames; ++i) {
    st += std::format("\n{}", strs[i]);
  }
  free(strs);

  std::cout << st << std::endl;
  error_window(st.c_str(), true);
  exit(0);
}

void error_exit_callback() {
  gtk_main_quit();
  exit(0);
}

void restart_callback() {
  gtk_main_quit();
  restartable_main();
}
