//
// TODO generated message class
//
packet Eth_pck 
{
    unsigned char macDest[6];
    unsigned char macSrc[6];
    unsigned short length;
     // the next variables are considered payload should not be considered as header
    int type; // 0 - Genereal , 1- Request, 2 - Reply. used for RTT calculations.
    unsigned int msgNumber;
}
