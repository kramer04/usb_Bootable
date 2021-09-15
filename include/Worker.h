#ifndef WORKER_H
#define WORKER_H

#include <mutex>
#include <thread>
#include <glibmm-2.4/glibmm/ustring.h>

class Fenetre;
class Worker
{
private:
  // Synchronizes access to member data.
  mutable std::mutex m_Mutex;
  // Data used by both GUI thread and worker thread.
  double m_fraction_done;
  unsigned long m_sizeUsb{0}, m_sizeIso{0};
  bool m_shall_stop;
  bool m_has_stopped;
  bool m_copy_has_finished;
  //bool m_inverted_progressBar;
  std::string m_mountPointIso, m_mountPointUsb;
  //Fonctions
  unsigned long folder_size(std::string chemin);

public:
  Worker();
  void do_work(Fenetre *caller);
  //void get_data(double *fraction_done, Glib::ustring *message, bool *inverted, bool *copyFinished) const;
  void get_data(double *fraction_done, Glib::ustring *message, bool *copyFinished) const;
  void stop_work();
  bool has_stopped() const;
  Glib::ustring m_message;

};

#endif