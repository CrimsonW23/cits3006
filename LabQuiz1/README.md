# RickMalware

### For a better GUI, I've uploaded this project to my GitHub at https://github.com/CrimsonW23/cits3006/tree/main/LabQuiz1

## Linux
1. Find out your "hacker" client's IP using `ifconfig`.
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
7. Repeat in your victim client with `victim`
8. Enjoy
