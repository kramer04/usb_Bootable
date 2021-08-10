![blabla](https://user-images.githubusercontent.com/29018157/128608073-5bd0bbdf-a191-47aa-9a5e-a65d1e1a13dc.png)

# usb_Bootable
# Make usb bootable for Windows, Linux OS or Raspberry OS

### To use on Ubuntu


## Install library glibmm-3.0

`sudo apt-get install libglibmm-3.0-dev`

## How to compile
### Open a terminal where you have unzip files

```
g++ -g main.cpp src/*.cpp -lstdc++fs -pthread -I include -o main `pkg-config gtkmm-3.0 --cflags --libs` -Wall -Wextra -O2
```


### To launch programm

`sudo ./main`

![capture_wait](https://user-images.githubusercontent.com/29018157/128824495-9eb37701-2161-4af4-a73e-605e386910c2.png)

![copie_terminee](https://user-images.githubusercontent.com/29018157/128824573-018917eb-03aa-4547-bf1f-bed7e0b37cfa.png)
