#include "Fenetre.h"
//#include <filesystem>
#include <gtkmm-3.0/gtkmm/application.h>

int main(int argc, char *argv [])
{
  if (std::filesystem::exists("out.txt"))
  {
    std::filesystem::remove("out.txt");
  }
  auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
  Fenetre fenetre;

  return app->run(fenetre);
}