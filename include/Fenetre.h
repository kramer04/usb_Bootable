#ifndef FENETRE_H
#define FENETRE_H

#include "Worker.h"
#include <iostream>
#include <gtkmm-3.0/gtkmm/window.h>
#include <gtkmm-3.0/gtkmm/box.h>
#include <gtkmm-3.0/gtkmm/button.h>
#include <gtkmm-3.0/gtkmm/buttonbox.h>
//#include <gtkmm-3.0/gtkmm/checkbutton.h>
#include <gtkmm-3.0/gtkmm/comboboxtext.h>
#include <gtkmm-3.0/gtkmm/filechooserdialog.h>
#include <gtkmm-3.0/gtkmm/label.h>
#include <gtkmm-3.0/gtkmm/progressbar.h>
#include <gtkmm-3.0/gtkmm/stock.h>
#include <filesystem>
#include <string>
#include <glibmm-2.4/glibmm/dispatcher.h>

class Fenetre : public Gtk::Window
{
public:
    Fenetre();
    void f_lsblk();
    void ouvrir_fichier();
    void notify();
    void on_start_button_clicked();
    void update_start_stop_buttons();
    void on_quit_button_clicked();
    void active_listederoulante();
    void on_notification_from_worker_thread();
    void update_widgets();

    std::string filename, chemin_usb;

    Gtk::ProgressBar m_ProgressBar;
    Gtk::ComboBoxText listeDeroulante;

private:
    Gtk::Box boiteV;
    Gtk::ButtonBox boutonBox;
    Gtk::Button boutonQ, lance_copie, ouvrirFichier;
    Gtk::Label pourcentage;

    Glib::Dispatcher m_Dispatcher;
    std::thread *m_WorkerThread;
    Worker m_worker;

};

#endif