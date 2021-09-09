#include "Worker.h"
#include "Fenetre.h"
#include <chrono>
#include <sstream>

Worker::Worker() :
  m_Mutex(),
  m_fraction_done(0.0),
  m_sizeUsb(0),
  m_sizeIso(0),
  m_shall_stop(false),
  m_has_stopped(false),
  m_copy_has_finished(false),
  m_inverted_progressBar(false),
  m_message()
{}

void Worker::get_data(double *fraction_done, Glib::ustring *message, bool *inverted, bool *copyFinished) const
{
  std::lock_guard<std::mutex> lock(m_Mutex);

  if (fraction_done)
    *fraction_done = m_fraction_done;
  if (message)
    *message = m_message;

  if (m_copy_has_finished)
  {
    *copyFinished = m_copy_has_finished;
    *inverted = m_inverted_progressBar;
  }
}
void Worker::do_work(Fenetre *caller)
{
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_fraction_done = 0.0;
    m_message = "";
    m_has_stopped = false;
  }

  std::thread t1([this]() {

    auto const copyOption = std::filesystem::copy_options::recursive | std::filesystem::copy_options::skip_symlinks;
    std::filesystem::copy("/mnt/iso", "/mnt/usb", copyOption);
    {
      std::lock_guard<std::mutex> lock(m_Mutex);
      m_copy_has_finished = true;
    }

  });

  std::thread t2([this, caller]() {
    {
      std::lock_guard<std::mutex> lock(m_Mutex);
      m_sizeIso = folder_size("/mnt/iso");
    }
    while (m_fraction_done < 1)
    {
      {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_sizeUsb = folder_size("/mnt/usb");
        m_fraction_done = static_cast<double>(m_sizeUsb) / m_sizeIso;
        std::ostringstream ostr;
        ostr << (m_fraction_done * 100.0) << " %";
        if (m_copy_has_finished)
        {
          m_fraction_done = 1;
          ostr << " Copie terminée. Synchronization en cours";
          std::cout << "Copie terminée. Synchronization en cours" << std::endl;
        }
        m_message = ostr.str();

      }
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
      caller->notify();
    }
  });

  if (t1.joinable())
    t1.join();
  if (t2.joinable())
    t2.join();

  caller->notify();

  std::thread t3([]() {
    int status = system("bash ./unmountpoint.sh");
    status != -1 ? std::cout << "." << std::endl : std::cout << "Erreur exécution commande system" << std::endl;
  });
  std::thread t4([this, caller]() {
    {
      std::lock_guard<std::mutex> lock(m_Mutex);
      m_fraction_done = 0;
    }
    while (std::filesystem::exists("/mnt/usb"))
    {
      /*
      {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_fraction_done += 0.01;
        if (m_fraction_done > 1)
        {
          m_fraction_done = 0;
          m_inverted_progressBar = !m_inverted_progressBar;
        }
      }
      */
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      caller->notify();
    }
    {
      std::lock_guard<std::mutex> lock(m_Mutex);
      std::ostringstream ostr;
      ostr << "Copie sur la clé USB terminée";
      m_message = ostr.str();
    }
  });
  if (t3.joinable())
    t3.join();
  if (t4.joinable())
    t4.join();
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_shall_stop = false;
    m_has_stopped = true;
  }
  caller->notify();
}
void Worker::stop_work()
{
  std::lock_guard<std::mutex> lock(m_Mutex);
  m_shall_stop = true;
}
bool Worker::has_stopped() const
{
  std::lock_guard<std::mutex> lock(m_Mutex);
  return m_has_stopped;
}

unsigned long Worker::folder_size(std::string chemin)
{
  unsigned long dossier_size = 0;
  for (std::filesystem::recursive_directory_iterator it(chemin); it != std::filesystem::recursive_directory_iterator(); it++)
  {
    if (!std::filesystem::is_directory(*it))
    {
      dossier_size += std::filesystem::file_size(*it);
    }
  }
  return dossier_size;
}
/*
  std::thread t1([this, caller]() {
    while (m_fraction_done <= 1)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
      {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_fraction_done += 0.01;
      }
      caller->notify();std::cout << "m_fraction_done = " << m_fraction_done << "\n";
    }
  });

  std::thread t2([this, caller]() {
    unsigned long sizeUsb{0}, sizeIso{0};
    do
    {
      {
        std::lock_guard<std::mutex> lock(m_Mutex);
        std::ostringstream ostr;
        ostr << (m_fraction_done * 100.0) << "% done";
        m_message = ostr.str();
        m_fraction_done += 0.01;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
      caller->notify();std::cout << "m_fraction_done = " << m_fraction_done << "\n";
    } while (m_fraction_done <= 1);
  });
*/