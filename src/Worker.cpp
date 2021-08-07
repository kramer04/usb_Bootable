#include "Worker.h"
#include "Fenetre.h"
#include <cstdlib>
#include <fstream>
#include <algorithm>

long double folder_size(std::string chemin)
{
  //unsigned long long f_size = 0;
  long double dossier_size = 0.0;
  for (std::filesystem::recursive_directory_iterator it(chemin); it != std::filesystem::recursive_directory_iterator(); it++)
  {
    if (!std::filesystem::is_directory(*it))
    {
      dossier_size += std::filesystem::file_size(*it);
    }
  }
  return dossier_size;
}

Worker::Worker() : m_Mutex(), m_has_stopped(false), source_size(0), m_fraction_done(0.0)
{}
void Worker::get_data(double *fraction_done)
{

  std::lock_guard<std::mutex> lock(m_Mutex);
  if (fraction_done)
    *fraction_done = m_fraction_done;
}
void Worker::do_work(Fenetre *caller)
{

  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_has_stopped = false;
    std::string source {caller->filename}, cible {};
    std::string::iterator ptr;
    for (ptr = caller->chemin_usb.end() - 8; ptr != caller->chemin_usb.end(); ptr++)
    {
      cible += *ptr;
    }
    cible = "pathToUsb=" + cible;
    char *c = new char [cible.size() + 1];
    strcpy(c, cible.c_str());
    putenv(c);

    cible = "";
    for (ptr = caller->chemin_usb.end() - 8; ptr != caller->chemin_usb.end(); ptr++)
    {
      cible += *ptr;
    }
    cible = "umountUsb=" + cible + "?*";
    char *c1 = new char [cible.size() + 1];
    strcpy(c1, cible.c_str());
    putenv(c1);

    source = "source=" + source;
    char *d = new char [source.size() + 1];
    strcpy(d, source.c_str());
    putenv(d);

    //source_size = folder_size(caller->filename);
    //source_size = caller->filename.size();
    source_size = std::filesystem::file_size(caller->filename);
    std::cout << "source size = " << source_size << std::endl;
    std::cout << "Source = " << source << std::endl;
    std::cout << "Cible = " << cible << std::endl;
    //system("echo $source;echo $pathToUsb;echo $umountUsb");
    //system("dd if=$source of=$pathToUsb bs=4M status=progress && sync");
  } //fin mutex

  std::thread t1([] (){
    std::cout << "Lance le thread t1" << std::endl;
    //if (system("sudo mountpoint -q $umountUsb && umount $umountUsb") != -1)
    /*
    if (system("sudo umount $umountUsb") != -1)
      {
      std::cout << "Démonte la clé usb" << std::endl;
      }
*/
    std::system("sudo umount $umountUsb");

    std::cout << "Démonte la clé usb" << std::endl;

    std::cout << "Lance la copie" << std::endl;

    //system(R"(while read line;do export line; done < <(sudo dd if=$source of=$pathToUsb bs=4M conv=fdatasync status=progress 2>&1 | stdbuf -o1 tr '\r' '\n' | stdbuf -o1 cut -d' ' -f1 | sed -u 's/[a-z]*//g')");

    //https://stackoverflow.com/questions/16618071/can-i-export-a-variable-to-the-environment-from-a-bash-script-without-sourcing-i
    //system("./start.sh");
    std::system("dd if=$source of=$pathToUsb bs=4M conv=fdatasync status=progress 2>&1 | stdbuf -o1 tr '\r' '\n' | stdbuf -o1 cut -d' ' -f1 | sed -u 's/[a-z]*//g' > out.txt");
    //std::system("unset t_std;sudo eval \"$((echo dd if=$source of=$pathToUsb bs=4M conv=fdatasync status=progress 2>&1 | stdbuf -o1 tr '\r' '\n' | stdbuf -o1 cut -d' ' -f1 | sed -u 's/[a-z]*//g') \\ > >t_std=$(cat);typeset -p t_std)\"");
    std::cout << "Fin de la copie" << std::endl;
  });

  std::thread t2([this, caller] (){
    std::cout << "Lance le thread t2" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //int i {0};
    //double taille {0.0};
    do
    {
      //char *ligne = getenv("echo $t_std");
      //std::stringstream strl{out()};
      ////strl << ligne;
      //std::string l{strl.str()};
      //l = strl.str();
      //std::cout << "LIGNE = " << i << " " << l << std::endl;
      //i++;
      //f_size_usb = std::atoll(l.c_str());
      f_size_usb = out();
      //std::cout << "f_size_usb = " << f_size_usb << std::endl;
      m_fraction_done = static_cast<float>(f_size_usb) / source_size;
      //std::cout << "m_fraction_done = " << m_fraction_done << std::endl;
      //m_fraction_done += 0.01;
      //taille += 0.1;
      //m_fraction_done = taille / source_size;
      //out();
      caller->notify();
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
    } while (m_fraction_done < 1);
  });

  if (t1.joinable())
  {
    t1.join();
  }
  if (t2.joinable())
  {
    t2.join();
  }
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_has_stopped = true;
  }
  caller->notify();
}
long long Worker::out()
{
  const std::string filename {"out.txt"};
  std::string lastline;
  std::fstream fs;
  fs.open(filename.c_str(), std::fstream::in);
  if (fs.is_open())
  {
    //chope le dernier caractère avant EOF
    fs.seekg(-2, std::ios_base::end);
    bool keepLooping = true;
    while (keepLooping)
    {
      char ch;
      fs.get(ch);
      if ((int)fs.tellg() <= 1)
      {
        fs.seekg(0);
        keepLooping = false;
      }
      else if (ch == '\n')
      {
        keepLooping = false;
      }
      else
      {
        fs.seekg(-2, std::ios_base::cur);
      }
    }
    getline(fs, lastline);
    //std::cout << lastline << std::endl;
    fs.close();
  }
  else
  {
    std::cout << "Ne trouve pas le dernier caractère de la ligne" << std::endl;
  }
  //auto it {std::remove(std::begin(lastline), std::end(lastline), ' ')};
  //lastline.erase(it, std::end(lastline));
  lastline.erase(std::remove_if(lastline.begin(), lastline.end(), [] (char c){return(c == ' ' || c == '\r' || c == '\n'); }), lastline.end());
  std::cout << "lastline = " << lastline << std::endl;
  long sizeUsb {0};
  if (lastline == "")
  {
    sizeUsb = 0;
  }
  else { sizeUsb = std::stol(lastline); };
  return sizeUsb;
}
void Worker::stop_work()
{
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    std::cout << "FIN" << std::endl;
  }
}
bool Worker::has_stopped() const
{
  std::lock_guard<std::mutex> lock(m_Mutex);
  return m_has_stopped;
}