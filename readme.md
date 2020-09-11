![alt text](https://github.com/JamieH/gm_glsock/raw/master/images/logo.png "Logo")

#### Credits: ζeh Matt, Morten, Python1320, CapsAdmin, JamieH, yobson/jonjo

Also big thanks to Morten for a few fixes and the first initial Linux build.

http://www.facepunch.com/threads/1136961

### Known Bugs

Windows:  
    Unloading the module may cause a delay due to the io_service not being shutdown properly, blame Boost.
    Actually, disable IOCP when building boost, that fixes it. Default build comes with IOCP disabled.

Linux:  
    Boost is now statically linked, theres still some problem with libstd++.6.so, you have to wipe it from
    the bin directory in orangebox in order to load this module, unless you come up with a fix.
    
### Documentation
https://github.com/yobson1/gm_glsock/wiki
