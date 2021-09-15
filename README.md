![blabla](https://user-images.githubusercontent.com/29018157/128608073-5bd0bbdf-a191-47aa-9a5e-a65d1e1a13dc.png)

# usb_Bootable
# Make usb bootable for Windows or Linux OS.

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

### To launch program
```
sudo ./main
```
![Capture d’écran de 2021-09-15 22-26-04](https://user-images.githubusercontent.com/29018157/133504896-af38865a-e178-4be4-a177-e514492887b3.png)
![Capture d’écran de 2021-09-15 22-26-38](https://user-images.githubusercontent.com/29018157/133504909-62fc4c0e-43e2-4ce7-92b5-da132b05f5aa.png)
![Capture d’écran de 2021-09-15 22-27-54](https://user-images.githubusercontent.com/29018157/133504920-787d0b27-4a24-4b93-8e2d-b6588b29d01c.png)

