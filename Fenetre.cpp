#include "Fenetre.h"

std::string getPopen(std::string cmd)
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

Fenetre::Fenetre() : filename(), chemin_usb(), m_ProgressBar(),
boiteV(Gtk::ORIENTATION_VERTICAL, 5), boutonBox(Gtk::ORIENTATION_HORIZONTAL),
boutonQ(Gtk::Stock::QUIT), lance_copie("Lance la copie"), ouvrirFichier("Fichier iso/img"),
m_Dispatcher(), m_WorkerThread(nullptr)
{
  set_title("Création d'une clé usb bootable");
  set_border_width(5);
  set_default_size(320, 150);
  set_position(Gtk::WIN_POS_CENTER);
  set_icon_from_file("icons8-cle-usb-81.png");

  add(boiteV);

  pourcentage.set_justify(Gtk::JUSTIFY_CENTER);

  boiteV.pack_start(m_ProgressBar, Gtk::PACK_SHRINK);
  boiteV.pack_start(pourcentage, Gtk::PACK_EXPAND_WIDGET);
  boiteV.pack_start(listeDeroulante, Gtk::PACK_SHRINK);
  boiteV.pack_start(boutonBox, Gtk::PACK_SHRINK);
  boutonBox.pack_start(ouvrirFichier, Gtk::PACK_SHRINK);
  boutonBox.pack_start(lance_copie, Gtk::PACK_SHRINK);
  boutonBox.pack_start(boutonQ, Gtk::PACK_SHRINK);
  boutonBox.set_border_width(5);
  boutonBox.set_spacing(5);
  boutonBox.set_layout(Gtk::BUTTONBOX_CENTER);

  //boutons sur off
  lance_copie.set_sensitive(false);
  ouvrirFichier.set_sensitive(false);

  //clique
  boutonQ.signal_clicked().connect([this] (){ on_quit_button_clicked(); });
  listeDeroulante.signal_changed().connect([this] (){ active_listederoulante(); });
  ouvrirFichier.signal_clicked().connect([this] (){ ouvrir_fichier(); });
  lance_copie.signal_clicked().connect([this] (){ on_start_button_clicked(); });

  //dispatcher
  m_Dispatcher.connect(sigc::mem_fun(*this, &Fenetre::on_notification_from_worker_thread));

  //liste déroulante
  f_lsblk();

  show_all_children();
}

void Fenetre::notify()
{
  m_Dispatcher.emit();
}
void Fenetre::update_widgets()
{
  double fraction_done;
  m_worker.get_data(&fraction_done);
  m_ProgressBar.set_fraction(fraction_done);
  fraction_done *= 100;
  int precision {3};
  if (fraction_done == 100)
  {
    precision = 4;
  }
  else if (fraction_done < 10)
  {
    precision = 2;
  }

  //fraction_done == 100 ? precision = 4 : precision = 3;
  //std::string trimmedString = std::to_string(doubleVal).substr(0, std::to_string(doubleVal).find(".") + precisionVal + 1);
  pourcentage.set_text(std::to_string(fraction_done).substr(0, std::to_string(fraction_done).find(".") + precision) + " %");
}
void Fenetre::on_notification_from_worker_thread()
{

  if (m_WorkerThread && m_worker.has_stopped())
  {
    // Work is done.
    if (m_WorkerThread->joinable())
      m_WorkerThread->join();
    delete m_WorkerThread;
    m_WorkerThread = nullptr;
    update_start_stop_buttons();
  }
  update_widgets();
}

void Fenetre::active_listederoulante()
{
  ouvrirFichier.set_sensitive(true);
  if (listeDeroulante.get_active_row_number() == 0)
  {
    ouvrirFichier.set_sensitive(false);
  }
  chemin_usb = listeDeroulante.get_active_text();
}
void ::Fenetre::on_start_button_clicked()
{

  if (m_WorkerThread)
  {
    std::cout << "Ne peut lancer deux threads en même temps" << std::endl;
  }
  else
  {
    //lance un nouveau thread
    m_WorkerThread = new std::thread(
      [this]{
      m_worker.do_work(this);
    });
  }
  update_start_stop_buttons();
}
void Fenetre::update_start_stop_buttons()
{
  const bool thread_is_running = m_WorkerThread != nullptr;

  lance_copie.set_sensitive(!thread_is_running);
  boutonQ.set_sensitive(!thread_is_running);
  ouvrirFichier.set_sensitive(!thread_is_running);
}
void Fenetre::f_lsblk()
{
  std::string lsblk {R"(lsblk -dPo tran,label,model,size,path | grep usb | awk '/sd/' | sed 's/"//g')"};
  std::string env_p {getPopen(lsblk)};
  //std::vector<std::string> flux{};
  std::istringstream str(env_p);
  std::string ligne {""};

  if (env_p != "")
  {
    listeDeroulante.append("Choisir une clé USB");
    while (std::getline(str, ligne))
    {
      //std::getline(str >> std::ws, ligne);
      //flux.push_back(ligne);
      listeDeroulante.append(ligne);
    }
  }
  else
  {
    listeDeroulante.append("Pas de clé USB");
  }
  listeDeroulante.set_active(0);
}
//quit
void Fenetre::on_quit_button_clicked()
{
  if (m_WorkerThread)
  {
    // Ordonner au thread de travail de s'arrêter et d'attendre qu'il s'arrête.
    m_worker.stop_work();
    if (m_WorkerThread->joinable())
      m_WorkerThread->join();
  }
  hide();
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
        filename = dialog.get_filename();
        std::cout << "Fichier sélectionné : " << filename << std::endl;
        std::cout << "Taille de filename = " << std::filesystem::file_size(filename) << std::endl;
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
  if (filename != "")
  {
    lance_copie.set_sensitive(true);
  }
  else
  {
    lance_copie.set_sensitive(false);
  }
}