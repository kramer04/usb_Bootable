#include "Fenetre.h"

Fenetre::Fenetre() :
  m_boxH(Gtk::ORIENTATION_HORIZONTAL, 5),
  m_boxV(Gtk::ORIENTATION_VERTICAL, 5),
  m_listeDeroulante(),
  m_buttonBox(Gtk::ORIENTATION_HORIZONTAL),
  m_demarrer(Gtk::Stock::EXECUTE), m_isofile("Fichier ISO"), m_Q(Gtk::Stock::QUIT),
  m_progressBar(),
  m_label("Démarrage"),
  m_dispatcher(),
  m_Worker(),
  m_stateLabel(false),
  m_WorkerThread(nullptr),
  m_mountPointIso("/mnt/iso"), m_mountPointUsb("/mnt/usb")

{
  set_title("Créer une clé USB bootable");
  set_border_width(5);
  set_default_size(300, 150);
  set_position(Gtk::WIN_POS_CENTER);
  set_icon_from_file("icons8-cle-usb-81.png");

  m_progressBar.set_text("Barre de progression");
  m_progressBar.set_show_text();
  m_demarrer.set_sensitive(false);
  m_isofile.set_sensitive(false);

  m_buttonBox.pack_start(m_demarrer, Gtk::PACK_SHRINK);
  m_buttonBox.pack_start(m_isofile, Gtk::PACK_SHRINK);
  m_buttonBox.pack_end(m_Q, Gtk::PACK_SHRINK);
  m_boxH.pack_start(m_buttonBox);
  m_boxV.pack_start(m_progressBar);
  m_boxV.pack_start(m_listeDeroulante);
  m_boxV.pack_start(m_label);
  m_boxV.pack_start(m_boxH);

  m_demarrer.signal_clicked().connect([this]() {on_start_button_clicked();});
  m_Q.signal_clicked().connect([this]() {on_quit_button_clicked();});
  m_isofile.signal_clicked().connect([this]() {ouvrir_fichier();});
  m_listeDeroulante.signal_changed().connect([this] {active_listederoulante();});
  m_dispatcher.connect(sigc::mem_fun(*this, &Fenetre::on_notification_from_worker_thread));

  add(m_boxV);
  lsblk();
  //update_start_stop_buttons();
  show_all_children();
}
void Fenetre::notify()
{
  m_dispatcher.emit();
}
void Fenetre::on_start_button_clicked()
{
  create_mount_point();
  if (m_WorkerThread)
  {
    std::cout << "Can't start a worker thread while another one is running." << std::endl;
  }
  else
  {
    // Start a new worker thread.
    m_WorkerThread = new std::thread(
      [this] {
        m_Worker.do_work(this);
      });
  }
  update_start_stop_buttons();
}
void Fenetre::update_start_stop_buttons()
{
  const bool thread_is_running = m_WorkerThread != nullptr;
  m_demarrer.set_sensitive(!thread_is_running);
  m_isofile.set_sensitive(!thread_is_running);
  m_Q.set_sensitive(!thread_is_running);
}
void Fenetre::update_widget()
{
  double fraction_done;
  Glib::ustring message_from_worker_thread;
  bool inverted, copyFinished;
  m_Worker.get_data(&fraction_done, &message_from_worker_thread, &inverted, &copyFinished);

  if (copyFinished == false)
  {
    m_progressBar.set_fraction(fraction_done);
    m_label.set_text(message_from_worker_thread);
  }
  else
  {
    m_label.set_text(message_from_worker_thread);
    //m_progressBar.set_inverted(inverted);
    m_progressBar.set_pulse_step(0.4);
    m_progressBar.pulse();
    //m_progressBar.set_fraction(fraction_done);
  }
}
void Fenetre::on_notification_from_worker_thread()
{
  if (m_WorkerThread && m_Worker.has_stopped())
  {
    // Work is done.
    if (m_WorkerThread->joinable())
      m_WorkerThread->join();
    delete m_WorkerThread;
    m_WorkerThread = nullptr;
    update_start_stop_buttons();
  }
  update_widget();
}
void Fenetre::on_quit_button_clicked()
{
  if (m_WorkerThread)
  {
    // Order the worker thread to stop and wait for it to stop.
    m_Worker.stop_work();
    if (m_WorkerThread->joinable())
      m_WorkerThread->join();
  }
  hide();
}
//Popen
std::string Fenetre::getPopen(std::string cmd)
{

  std::string data;
  FILE *stream;
  const int max_buffer = cmd.size();
  char buffer [max_buffer];

  stream = popen(cmd.c_str(), "r");

  if (stream)
  {

    while (!feof(stream))
      if (fgets(buffer, max_buffer, stream) != NULL)
        data.append(buffer);
    pclose(stream);
  }
  return data;
}
// Liste déroulante
void Fenetre::lsblk()
{
  std::string lsblk{R"(lsblk -dPo tran,label,model,size,path | grep usb | awk '/sd/' | sed 's/"//g')"};
  std::string env_p{getPopen(lsblk)};
  std::istringstream str(env_p);
  std::string ligne{""};

  if (env_p == "")
  {
    m_listeDeroulante.append("Pas de clé USB");
    m_listeDeroulante.set_active(0);
  }
  else
  {
    m_listeDeroulante.append("Choisir une clé USB");
    while (std::getline(str, ligne))
    {
      //std::getline(str >> std::ws, ligne);
      //flux.push_back(ligne);
      m_listeDeroulante.append(ligne);
    }
    m_listeDeroulante.set_active(0);
  }
}
void Fenetre::active_listederoulante()
{
  m_listeDeroulante.get_active_row_number() > 0 ? m_isofile.set_sensitive(true) : m_isofile.set_sensitive(false);
}
//fichier iso
void Fenetre::ouvrir_fichier()
{
  Gtk::FileChooserDialog dialog("Choisir un fichier", Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);

  //Add response buttons the the dialog:
  dialog.add_button("_Annuler", Gtk::RESPONSE_CANCEL);
  dialog.add_button("_Ouvrir", Gtk::RESPONSE_OK);

  //Add filters, so that only certain file types can be selected:

  auto filter_iso = Gtk::FileFilter::create();
  filter_iso->set_name("iso files");
  filter_iso->add_mime_type("application/x-cd-image");
  dialog.add_filter(filter_iso);

  auto filter_img = Gtk::FileFilter::create();
  filter_img->set_name("img files");
  filter_img->add_mime_type("application/x-raw-disk-image");
  dialog.add_filter(filter_img);

  auto filter_text = Gtk::FileFilter::create();
  filter_text->set_name("Text files");
  filter_text->add_mime_type("text/plain");
  dialog.add_filter(filter_text);

  auto filter_cpp = Gtk::FileFilter::create();
  filter_cpp->set_name("C/C++ files");
  filter_cpp->add_mime_type("text/x-c");
  filter_cpp->add_mime_type("text/x-c++");
  filter_cpp->add_mime_type("text/x-c-header");
  dialog.add_filter(filter_cpp);

  auto filter_any = Gtk::FileFilter::create();
  filter_any->set_name("Any files");
  filter_any->add_pattern("*");
  dialog.add_filter(filter_any);

  //Show the dialog and wait for a user response:
  int result = dialog.run();

  //Handle the response:
  switch (result)
  {
    case (Gtk::RESPONSE_OK):
    {
      //Notice that this is a std::string, not a Glib::ustring.
      m_filename = dialog.get_filename();
      std::cout << "Fichier sélectionné : " << m_filename << std::endl;
      std::cout << "Taille de m_filename = " << std::filesystem::file_size(m_filename) << std::endl;
      break;
    }
    case (Gtk::RESPONSE_CANCEL):
    {
      std::cout << "Clic sur annuler" << std::endl;
      break;
    }
    default:
    {
      std::cout << "Mauvais clic" << std::endl;
      break;
    }
  }

  if (m_filename != "")
  {
    m_demarrer.set_sensitive(true);
  }
  else
  {
    m_demarrer.set_sensitive(false);
  }
}
void Fenetre::create_mount_point()
{
  // Point de montage de l'iso
  std::string iso{m_filename};std::cout << m_filename << "\n";
  iso = "iso=" + iso;
  char *c = new char [iso.size() + 1];
  strcpy(c, iso.c_str());
  putenv(c);

  // Point de montage de la clé USB
  std::string usb{m_listeDeroulante.get_active_text()};
  usb = "usb=" + usb;
  char *d = new char [usb.size() + 1];
  strcpy(d, usb.c_str());
  putenv(d);

  int status = system(R"(bash ./mountpoint.sh "$iso" "$usb")");
  status != -1 ? std::cout << "." << std::endl : std::cout << "Erreur exécution commande system" << std::endl;

  delete c;
  c = nullptr;
  delete d;
  d = nullptr;

}
void Fenetre::delete_unmount_point()
{
  if (std::filesystem::exists(m_mountPointIso) || std::filesystem::exists(m_mountPointUsb))
  {
    int status = system("bash ./unmountpoint.sh");std::cout << status << "\n";
    status != -1 ? std::cout << "." << std::endl : std::cout << "Erreur exécution commande system" << std::endl;
  }
}