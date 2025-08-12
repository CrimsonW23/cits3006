# MalRick

## Note for GitHub: Only use these files on your own machines. Using these files without consent of the victim is ILLEGAL.
### For a better GUI, I've uploaded this project to my GitHub at https://github.com/CrimsonW23/cits3006/tree/main/LabQuiz1

## How to run
1. Find out your "hacker" client's IP
   - <b>Linux:</b> `ifconfig`
   - <b>Windows:</b> `ipconfig`
2. In `victim.c`, edit the IP in line 26 to your hacker's IP.
3. Compile the `hacker.c` and `victim.c` codes.
   - <b>Linux:</b> Open up a terminal and run `make`
   - <b>Windows:</b> Run the commands below from a Linux machine:
     ```
     x86_64-w64-mingw32-g++ -o hacker.exe hacker.cpp -lws2_32
     x86_64-w64-mingw32-g++ -o victim.exe victim.cpp -lws2_32
     ```
4. In your hacker client, open up a terminal and navigate to the folder where you saved the codes.
5. Run the `hacker` executable
   - <b>Linux:</b> `./hacker`
   - <b>Windows:</b> `hacker.exe`
6. Repeat steps 4-5 in your victim client with `victim`
7. Enjoy

## Description
A simple "malware" function that prints any `.txt` files and the directory's content to the hacker's client. It also downloads the "malware" executable files from the Git repository, and runs them to show that the victim has been hacked. As a final touch, it launches a browser and plays "Never Gonna Give You Up" by Rick Astley.

## Other Files
### cross_malware.cpp
The "malware" file provided in the labs. Modified by me to be suitable for both Windows and Linux.
### linux_malware & win_malware.exe
The compiled versions of cross_malware.cpp for their respective platforms. Uploaded in the GitHub repository for easy downloading.
### Makefile
The same makefile provided in the labs.
