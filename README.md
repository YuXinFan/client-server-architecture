Travis Status.
-------------
[![Build Status](https://travis-ci.com/sht-cs133/hw4-c-s-architecture-YuXinFan.svg?token=appyqywAiysphxXppp9y&branch=master)](https://travis-ci.com/sht-cs133/hw4-c-s-architecture-YuXinFan)

## Notices

This starter code template has several minor changes to the released instructions:
1. You have to implement operator `<` and `==` for [JobID](include/JobID.hpp);
2. We add one additional API `getResult(const JobID &i)` for [Socket](include/Socket.hpp)
   class, please read the inline comments in header file;
3. The `watchDog` method in [Server](include/ServerThread.hpp) class is made public,
   to enable it be tested later;
   
Please note that implementing `watchDog` is **an optional bonus**, and relevant
tests for `watchDog` are disabled by default. If you accomplished this bonus, please:
1. Briefly describe your design and implementation in **another markdown file**
   and put it in project root;
2. Edit travis configuration file [.travis.yml](.travis.yml), comment out `--gtest_filter="-*.test_watch_dog"`
   in line 9, then all tests will be activated.

> Happy coding~
