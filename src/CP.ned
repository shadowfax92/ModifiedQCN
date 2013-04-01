package cngcontrol;

//
// TODO auto-generated module
//
simple CP
{
     parameters:
        @display("i=block/switch");
        @signal[QLen](type="double");
        @statistic[qlen](title="queue size"; source="QLen"; record=vector,stats,histogram; interpolationmode=linear);
        @signal[LossSg](type="double");
        @statistic[tLoss](title="Time Loss"; source="LossSg";record=stats,histogram; interpolationmode=none);
        @signal[fbCnt](type="int");
        @statistic[fbCnt](title="fb counter"; source="fbCnt";record=vector,stats,histogram; interpolationmode=none);
        volatile double Q_eq = 200.0;//Need to check this
        volatile double tToMarkRnd;
        volatile double interval;
        double MaxQlen = 300.0; //KB
        bool QCN_ON = default(true);  
    gates:
        inout mc;//Message control (mc)
        output out;
}
