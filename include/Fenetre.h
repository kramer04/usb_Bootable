#ifndef FENETRE_H
#define FENETRE_H

#include "Worker.h"
#include <iostream>
#include <glibmm-2.4/glibmm/dispatcher.h>
#include <gtkmm-3.0/gtkmm/window.h>
#include <gtkmm-3.0/gtkmm/box.h>
#include <gtkmm-3.0/gtkmm/buttonbox.h>
#include <gtkmm-3.0/gtkmm/button.h>
#include <gtkmm-3.0/gtkmm/comboboxtext.h>
#include <gtkmm-3.0/gtkmm/progressbar.h>
#include <gtkmm-3.0/gtkmm/label.h>
#include <gtkmm-3.0/gtkmm/stock.h>
#include <gtkmm-3.0/gtkmm/filechooserdialog.h>
#include <filesystem>
#include <string>
#include <cstdlib>

class Fenetre :public Gtk::Window
{
  private:
  Gtk::Box m_boxH, m_boxV;
  Gtk::ComboBoxText m_listeDeroulante;
  Gtk::ButtonBox m_buttonBox;
  Gtk::Button m_demarrer, m_isofile, m_Q;
  Gtk::ProgressBar m_progressBar;
  Gtk::Label m_label;
  Glib::Dispatcher m_dispatcher;

  std::string m_filename;

  // Signal handlers.
  void on_start_button_clicked();
  void on_quit_button_clicked();
  void update_widget();
  void update_start_stop_buttons();
  void ouvrir_fichier();
  // Dispatcher handler.
  void on_notification_from_worker_thread();

  //création de la liste déroulante
  void lsblk();
  //popen : récupère le résultat d'une commande dans le terminal
  std::string getPopen(std::string cmd);
  //liste déroulante
  void active_listederoulante();

  Worker m_Worker;
  bool m_stateLabel;
  std::thread *m_WorkerThread;

  std::string m_mountPointIso, m_mountPointUsb;

  public:
  Fenetre();
  void notify();
  // Functions
  void create_mount_point();
  void delete_unmount_point();
};

#endif