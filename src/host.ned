package cngcontrol;

//
// TODO auto-generated module
//
simple Host
{
    parameters:
        @signal[RTT](type="double");
        @statistic[Rtt](title="RTT measurment"; source="RTT"; record=vector,stats,histogram; interpolationmode=linear);
        @signal[dataRate](type="double");
        @statistic[cRate](title="dataRate"; source="dataRate"; record=vector,stats; interpolationmode=sample-hold);
        @display("i=device/pc");
        int hostNum = default(8);
        volatile int dataLength;
        int destChoice = default(0);
        volatile double expThr;
        int consecutive= default(1); // how many messages in a row
        // QCN parameters for HOST
        bool USING_TIMER;
    gates:
        input in;
        output out;
}
