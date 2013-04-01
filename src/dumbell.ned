package cngcontrol;

network Dumbell
{
    parameters:
        int W = default(2);
        double Q_LENGTH =  default(10);//Que Length
        int Q_EQ_STABLE_PERCENT =  default(20);//Q_QE calculated from Q_LENGTH and Q_EQ_STABLE_PERSENT
        double BC_LIMIT =  default(150);
        double TIMER_PERIOD =  default(10);//mSec
        double R_AI =  default(5);//Mbps;
        double R_HAI =  default(50);//Mbps
        int FAST_RECOVERY_TH =  default(5);//stages
        double MIN_RATE =  default(10);//Mbps
        double MIN_DEC_FACTOR =  default(0.5);
        double MAX_TX =  default(1000);//Mbps
        
        
    types:
        channel Channel extends ned.DatarateChannel
        {
            datarate= 100Mbps;
                //datarate = 10Gbps;
                //delay = 10us;
                //ber = 1e-10;
        }
    submodules:
        host[8]: Host {
            @display("i=device/pc");
        }
        switch[2]: Switch {
            @display("i=,gold");
        }
        
    connections:

       for j=0..sizeof(switch)-1, for i=0..(sizeof(host)/2-1){
        switch[j].in[i] <-- Channel <-- host[i+j*(sizeof(host)/2)].out; 
        host[i+j*(sizeof(host)/2)].in <-- Channel <-- switch[j].out[i];
       }
       switch[0].in++ <-- Channel <-- switch[1].out++;
       switch[1].in++ <-- Channel <-- switch[0].out++;
}

