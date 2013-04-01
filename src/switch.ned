package cngcontrol;

//
// TODO auto-generated module
//
module Switch
{
    
    
    parameters:
      @display("i=block/switch");
      string tableFile = "switch";  
    gates:
        input in[5];
        output out[5];
    submodules:
        conP[5]: CP {
            @display("i=block/boundedqueue");
        }
        control: MsgCntrl {
            @display("i=block/dispatch");
        }
    connections:
        control.in[i] <-- in[i] for i=0..4;
        conP[i].mc <--> control.mc[i] for i=0..4;
        conP[i].out --> out[i] for i=0..4;

}
