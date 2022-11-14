module avalon_bridge_mod (
    // Inputs
    clk,
    reset,

    // Outputs
    clk_o,
    reset_o,

    // Inputs from Avalon Switch Fabric
    avalon_rd_readdata,
    avalon_wr_waitrequest,
    avalon_rd_readdatavalid,
    avalon_rd_waitrequest,

    // Inputs from the master peripheral
    wr_address,
    rd_address,
    rd_read,
    wr_write,
    wr_write_data,
    wr_burstcount,
    rd_burstcount,

    // Bidirectionals

    // Outputs
    // Output to Avalon Switch Fabric
    avalon_wr_address,
    avalon_rd_address,
    avalon_rd_read,
    avalon_wr_write,
    avalon_wr_writedata,
    avalon_wr_burstcount,
    avalon_rd_burstcount,

    // Outputs to master peripheral
    wr_waitrequest,
    rd_waitrequest,
    rd_readdatavalid,
    rd_read_data
);


/*****************************************************************************
 *                           Parameter Declarations                          *
 *****************************************************************************/

parameter   AW  = 17;   // Address width
parameter   DW  = 16;   // Data width

/*****************************************************************************
 *                             Port Declarations                             *
 *****************************************************************************/
// Inputs
input                       clk;
input                       reset;

input                       avalon_wr_waitrequest;
input                       avalon_rd_waitrequest;
input                       avalon_rd_readdatavalid;
input           [(DW-1): 0] avalon_rd_readdata;

input           [(AW-1): 0] wr_address;
input           [(AW-1): 0] rd_address;
input                       wr_write;
input                       rd_read;
input           [(DW-1): 0] wr_write_data;
input           [5:0]       wr_burstcount;
input           [5:0]       rd_burstcount;

// Bidirectionals

// Outputs
output                  clk_o;
output                  reset_o;

output      [(AW-1): 0] avalon_wr_address;
output      [(AW-1): 0] avalon_rd_address;
output                  avalon_rd_read;
output                  avalon_wr_write;
output      [(DW-1): 0] avalon_wr_writedata;
output      [5:0]       avalon_wr_burstcount;
output      [5:0]       avalon_rd_burstcount;

output                  wr_waitrequest;
output                  rd_waitrequest;
output                  rd_readdatavalid;
output      [(DW-1): 0] rd_read_data;

/*****************************************************************************
 *                           Constant Declarations                           *
 *****************************************************************************/

/*****************************************************************************
 *                 Internal Wires and Registers Declarations                 *
 *****************************************************************************/

// Internal Wires

// Internal Registers

// State Machine Registers

/*****************************************************************************
 *                         Finite State Machine(s)                           *
 *****************************************************************************/


/*****************************************************************************
 *                             Sequential Logic                              *
 *****************************************************************************/

/*****************************************************************************
 *                            Combinational Logic                            *
 *****************************************************************************/
assign clk_o                = clk;
assign reset_o              = reset;
assign avalon_wr_address    = wr_address;
assign avalon_rd_address    = rd_address;
assign avalon_wr_write      = wr_write;
assign avalon_rd_read       = rd_read;
assign avalon_wr_writedata  = wr_write_data;
assign avalon_wr_burstcount = wr_burstcount;
assign avalon_rd_burstcount = rd_burstcount;
assign rd_read_data         = avalon_rd_readdata;
assign wr_waitrequest       = avalon_wr_waitrequest;
assign rd_waitrequest       = avalon_rd_waitrequest;
assign rd_readdatavalid     = avalon_rd_readdatavalid;

/*****************************************************************************
 *                              Internal Modules                             *
 *****************************************************************************/

endmodule

