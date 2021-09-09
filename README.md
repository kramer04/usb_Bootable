![blabla](https://user-images.githubusercontent.com/29018157/128608073-5bd0bbdf-a191-47aa-9a5e-a65d1e1a13dc.png)

# usb_Bootable
# Make usb bootable for Windows and Linux OS.

### To use on Ubuntu


## Install library gtkmm-3.0
```
sudo apt install libgtkmm-3.0-dev
```
![French doc about gtkmm](https://doc.ubuntu-fr.org/gtkmm)

## How to compile
### Open a terminal where you have unzip files

```
g++ -g main.cpp src/*.cpp -lstdc++fs -pthread -I include -o main `pkg-config gtkmm-3.0 --cflags --libs` -Wall -Wextra -O2
```

## Use an usb key with one partition. Type lsblk on terminal. You do have this :
<pre>sdf      8:80   1  14,4G  0 disk 
└─sdf1   8:81   1  14,4G  0 part </pre>
Otherwise use Gparted to create one partition

### To launch programm
```
sudo ./main
```
![1](https://user-images.githubusercontent.com/29018157/132638028-46ff8a90-dd24-4344-867c-4b2817363ea9.png)
![2](https://user-images.githubusercontent.com/29018157/132638040-f33a269e-9b11-463a-9e12-e62071b3f846.png)

