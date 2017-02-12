#ifndef __BTOOTH_H
#define __BTOOTH_H

class Bluetooth
{
public:
    Bluetooth();


    void scanStart();
    void scanLoop();
    void scanStop();
    
    void scan();

    void close();

    int Handle() {
      return _sock;
    }
    
private:
    int _devId;
    int _sock;
};
    

#endif // __BTOOTH_H
