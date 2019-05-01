//////////////////////////////////////////////////////////////////////
////                                                              ////
////  uart_test.v                                                 ////
////                                                              ////
////                                                              ////
////  This file is part of the "UART 16550 compatible" project    ////
////  http://www.opencores.org/cores/uart16550/                   ////
////                                                              ////
////  Documentation related to this project:                      ////
////  - http://www.opencores.org/cores/uart16550/                 ////
////                                                              ////
////  Projects compatibility:                                     ////
////  - WISHBONE                                                  ////
////  RS232 Protocol                                              ////
////  16550D uart (mostly supported)                              ////
////                                                              ////
////  Overview (main Features):                                   ////
////  UART core test bench                                        ////
////                                                              ////
////  Known problems (limits):                                    ////
////  A very simple test bench. Creates two UARTS and sends       ////
////  data on to the other.                                       ////
////                                                              ////
////  To Do:                                                      ////
////  More complete testing should be done!!!                     ////
////                                                              ////
////  Author(s):                                                  ////
////      - gorban@opencores.org                                  ////
////      - Jacob Gorban                                          ////
////                                                              ////
////  Created:        2001/05/12                                  ////
////  Last Updated:   2001/05/17                                  ////
////                  (See log for the revision history)          ////
////                                                              ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2000 Jacob Gorban, gorban@opencores.org        ////
////                                                              ////
//// This source file may be used and distributed without         ////
//// restriction provided that this copyright statement is not    ////
//// removed from the file and that any derivative work contains  ////
//// the original copyright notice and the associated disclaimer. ////
////                                                              ////
//// This source file is free software; you can redistribute it   ////
//// and/or modify it under the terms of the GNU Lesser General   ////
//// Public License as published by the Free Software Foundation; ////
//// either version 2.1 of the License, or (at your option) any   ////
//// later version.                                               ////
////                                                              ////
//// This source is distributed in the hope that it will be       ////
//// useful, but WITHOUT ANY WARRANTY; without even the implied   ////
//// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      ////
//// PURPOSE.  See the GNU Lesser General Public License for more ////
//// details.                                                     ////
////                                                              ////
//// You should have received a copy of the GNU Lesser General    ////
//// Public License along with this source; if not, download it   ////
//// from http://www.opencores.org/lgpl.shtml                     ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
//
// CVS Revision History
//
// $Log: uart_test.v,v $
// Revision 1.3  2001/05/31 20:08:01  gorban
// FIFO changes and other corrections.
//
// Revision 1.2  2001/05/17 18:34:18  gorban
// First 'stable' release. Should be sythesizable now. Also added new header.
//
// Revision 1.0  2001-05-17 21:27:12+02  jacob
// Initial revision
//
//
//`define DATA_BUS_WIDTH_8
`include "timescale.v"
`define TEST_NAME_STRING "uart-cdn"
module uart_test ();

`include "uart_defines.v"

reg                         clkr;
reg                         wb_rst_ir;
wire [`UART_ADDR_WIDTH-1:0] wb1_adr_i;
wire [31:0]                 wb1_dat_i;
wire [31:0]                 wb1_dat_o;
wire  [3:0]                 wb1_sel_i;
wire                        stx1_o;
reg                         srx1_ir;
wire                        int1_o;
wire                        int2_o;

integer e;

uart_top  uart1(
  clk, 
  
  // Wishbone signals
  wb_rst_i, wb1_adr_i, wb1_dat_i, wb1_dat_o, wb1_we_i, wb1_stb_i, wb1_cyc_i, wb1_ack_o,  wb1_sel_i,
  int1_o, // interrupt request

  // UART signals
  // serial input/output
  stx1_o, srx1_i,

  // modem signals
  rts1_o, cts1_i, dtr1_o, dsr1_i, ri1_i, dcd1_i
  );


// All the signals and regs named with a 1 are receiver fifo signals

wire [`UART_ADDR_WIDTH-1:0]  wb2_adr_i;
wire [31:0]                  wb2_dat_i;
wire [31:0]                  wb2_dat_o;
wire  [3:0]                  wb2_sel_i;
wire                         stx2_o;
reg                          srx2_ir;

uart_top  uart2(
  clk, 
  
  // Wishbone signals
  wb_rst_i, wb2_adr_i, wb2_dat_i, wb2_dat_o, wb2_we_i, wb2_stb_i, wb2_cyc_i, wb2_ack_o, wb2_sel_i,  
  int2_o, // interrupt request

  // UART signals
  // serial input/output
  stx2_o, srx2_i,

  // modem signals
  rts2_o, cts2_i, dtr2_o, dsr2_i, ri2_i, dcd2_i
  );

assign clk       = clkr;
assign wb_rst_i  = wb_rst_ir;
assign srx1_i    = srx1_ir;
assign cts1_i    = 1; //cts_ir;
assign dsr1_i    = 1; //dsr_ir;
assign ri1_i     = 1; //ri_ir;
assign dcd1_i    = 1; //dcd_ir;

assign srx2_i    = srx2_ir;
assign cts2_i    = 1; //cts1_ir;
assign dsr2_i    = 1; //dsr1_ir;
assign ri2_i     = 1; //ri1_ir;
assign dcd2_i    = 1; //dcd1_ir;

reg [31:0] dat_o;
/////////// CONNECT THE UARTS
always @(stx2_o)
begin
  srx1_ir = stx2_o;  
end

always @(stx1_o)
begin
  srx2_ir = stx1_o;
end

initial
begin
  clkr = 0;
end

wb_mast wbm1(// Outputs
        .adr                 (wb1_adr_i),
        .dout                (wb1_dat_i),
        .cyc                 (wb1_cyc_i),
        .stb                 (wb1_stb_i),
        .sel                 (wb1_sel_i),
        .we                  (wb1_we_i),
        // Inputs
        .clk                 (clk),
        .rst                 (wb1_rst_i),
        .din                 (wb1_dat_o),
        .ack                 (wb1_ack_o),
        .err                 (1'b0),
        .rty                 (1'b0));

wb_mast wbm2(// Outputs
         .adr                (wb2_adr_i),
         .dout               (wb2_dat_i),
         .cyc                (wb2_cyc_i),
         .stb                (wb2_stb_i),
         .sel                (wb2_sel_i),
         .we                 (wb2_we_i),
         // Inputs
         .clk                (clk),
         .rst                (wb_rst_i),
         .din                (wb2_dat_o),
         .ack                (wb2_ack_o),
         .err                (1'b0),
         .rty                (1'b0));

// The test sequence
initial
begin
  $display("* VCD in %s\n", {`TEST_NAME_STRING,".vcd"});
  $dumpfile({`TEST_NAME_STRING,".vcd"});
	$dumpvars(0);
  #1 wb_rst_ir = 1;
  #10 wb_rst_ir = 0;
end

task sendbyte1;
  input [7:0] byte;
  begin
    $display("%m : %t : sending : %h", $time, byte);
    wbm1.wb_wr1(0, 4'b1, {24'b0, byte});
    @(posedge clk);
    @(posedge clk);
  end
endtask

task sendbyte2;
  input [7:0] byte;
  begin
    $display("%m : %t : sending : %h", $time, byte);
    wbm2.wb_wr1(0, 4'b1, {24'b0, byte});
    @(posedge clk);
    @(posedge clk);
  end
endtask

task recvbyte1;
  begin
    e = 800;
    while (e > 0)
    begin
      @(posedge clk)
      if (uart1.regs.enable) e = e - 1;
    end
    wbm1.wb_rd1(0, 4'b1, dat_o);

  end
endtask
    

initial
begin
  #11;
  // Initalize lcr 
  wbm1.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011011, 24'b0});
  @(posedge clk);
  @(posedge clk);

  //write to lcr. set bit 7 to 1 (DLL and DLM accessible)
  wbm1.wb_wr1(`UART_REG_LC, 4'b1000, {8'b10011011, 24'b0});
  // set dl to divide by 2
  wbm1.wb_wr1(`UART_REG_DL1,4'b0001, 32'd2);
  @(posedge clk);
  @(posedge clk);

  // set dl to divide by 3
  wbm1.wb_wr1(`UART_REG_DL1,4'b0001, 32'd3);
  @(posedge clk);
  @(posedge clk);

  // set dl to divide by 2
  wbm1.wb_wr1(`UART_REG_DL1,4'b0001, 32'd2);
  @(posedge clk);
  @(posedge clk);

  // restore normal regiters by resetting bit 7 in lcr
  wbm1.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011011, 24'b0}); //00011011
  @(posedge clk);
  @(posedge clk);

  // Set bits 0-3 of the modem control resigster
  wbm1.wb_wr1(`UART_REG_MC, 4'b0001, {24'b0, 8'hFF});
  @(posedge clk);
  @(posedge clk);
  // Reset bits 0-3 of the modem control resigster
  wbm1.wb_wr1(`UART_REG_MC, 4'b0001, {24'b0, 8'h00});
  @(posedge clk);
  @(posedge clk);

  // Clear Rx and Tx FIFO
  wbm1.wb_wr1(`UART_REG_FC, 4'b0100, {8'b0, 8'h06, 16'h0000});
  @(posedge clk);
  @(posedge clk);
  // Change interrupt level
  wbm1.wb_wr1(`UART_REG_FC, 4'b0100, {8'b0, 8'h80, 16'h0000});
  @(posedge clk);
  @(posedge clk);
  // Change interrupt level back
  wbm1.wb_wr1(`UART_REG_FC, 4'b0100, {8'b0, 8'h00, 16'h0000});
  @(posedge clk);
  @(posedge clk);

  // Enable all interrupts
  wbm1.wb_wr1(`UART_REG_IE, 4'b0010, {16'd0, 8'hFF, 8'd0}); 
  @(posedge clk);
  @(posedge clk);
  // Disable all interrupts
  wbm1.wb_wr1(`UART_REG_IE, 4'b0010, {16'd0, 8'h00, 8'd0}); 
  @(posedge clk);
  @(posedge clk);

  // Set scratch register
  wbm1.wb_wr1(`UART_REG_SR, 4'b1000, {8'hFF, 24'd0}); 
  @(posedge clk);
  @(posedge clk);
  // Reset scratch register
  wbm1.wb_wr1(`UART_REG_SR, 4'b1000, {8'h00, 24'd0}); 
  @(posedge clk);
  @(posedge clk);

  // Read the LSR
  wbm1.wb_rd1(`UART_REG_LS, 4'b0010, dat_o);
  @(posedge clk);
  // Read the IIR
  wbm1.wb_rd1(`UART_REG_II, 4'b0100, dat_o);
  @(posedge clk);
  // Read the LSR
  wbm1.wb_rd1(`UART_REG_LS, 4'b0010, dat_o);
  @(posedge clk);

  // Change word size to 7-bits
  wbm1.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011010, 24'b0});
  @(posedge clk);
  @(posedge clk);

  // Change word size to 8-bits
  wbm1.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011011, 24'b0});

  fork
  begin
    sendbyte1(8'b10000001);
    sendbyte1(8'b01000010);
    sendbyte1(8'b11000011);
    sendbyte1(8'b00100100);
    sendbyte1(8'b10100101);
    sendbyte1(8'b01100110);
    sendbyte1(8'b11100111);
    sendbyte1(8'b00011000);
    sendbyte1(8'b10000001);
    sendbyte1(8'b01000010);
    sendbyte1(8'b11000011);
    sendbyte1(8'b00100100);
    sendbyte1(8'b10100101);
    sendbyte1(8'b01100110);
    sendbyte1(8'b11100111);
    sendbyte1(8'b00011000);

    wait (uart1.regs.tstate==0 && uart1.regs.transmitter.tf_count==0);
//    disable check;
  end
//  begin: check
//  end
  join


  // Now receiving
  // enable interrupts
  wbm1.wb_wr1(`UART_REG_IE, 4'b0010, {16'b0, 8'b00001111, 8'b0});
  @(posedge clk);

  // disable interupts
  wbm1.wb_wr1(`UART_REG_IE, 4'b0010, {16'b0, 8'b00000000, 8'b0});
  @(posedge clk);

  // enable interrupts
  wbm1.wb_wr1(`UART_REG_IE, 4'b0010, {16'b0, 8'b00001111, 8'b0});


  wait(uart1.regs.receiver.rf_count == 16);
 

  e = 6400;
  while (e > 0)
  begin
    @(posedge clk)
    if (uart1.regs.enable) e = e - 1;
  end

  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm1.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  $display("%m : Finish");

  $finish;
end

/*
always @(int1_o) begin
  if (int1_o)
    $display("INT1_O high (%g)", $time);
  else
    $display("INT1_O low (%g)", $time);
end
    
always @(int1_o)
begin
  if (int1_o) begin
    wbm2.wb_rd1(2,4'b0100, dat_o);
    $display("IIR : %h", dat_o);
    wbm2.wb_rd1(5,4'b0010, dat_o);
    $display("LSR : %h", dat_o);
    wbm2.wb_rd1(0, 4'b1, dat_o);
    $display("%m : %t : Data out: %h", $time, dat_o);
  end
end

always @(int2_o) begin
  if (int2_o)
    $display("INT2_O high (%g)", $time);
  else
    $display("INT2_O low (%g)", $time);
end
    
always @(int2_o)
begin
  if (int2_o) begin
    wbm1.wb_rd1(2,4'b0100, dat_o);
    $display("IIR : %h", dat_o);
    wbm1.wb_rd1(5,4'b0010, dat_o);
    $display("LSR : %h", dat_o);
    wbm1.wb_rd1(0, 4'b1, dat_o);
    $display("%m : %t : Data out: %h", $time, dat_o);
  end
end
*/

// receiver side
initial
begin
  #11;
  //Init LCR
  wbm2.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011011, 24'b0});
  @(posedge clk);
  @(posedge clk);

  //write to lcr. set bit 7
  wbm2.wb_wr1(`UART_REG_LC, 4'b1000, {8'b10011011, 24'b0});
  // set dl to divide by 2
  wbm2.wb_wr1(`UART_REG_DL1, 4'b1, 32'd2);
  @(posedge clk);
  @(posedge clk);

  // set dl to divide by 3
  wbm2.wb_wr1(`UART_REG_DL1,4'b0001, 32'd3);
  @(posedge clk);
  @(posedge clk);

  // set dl to divide by 2
  wbm2.wb_wr1(`UART_REG_DL1,4'b0001, 32'd2);
  @(posedge clk);
  @(posedge clk);

  // restore normal registers
  wbm2.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011011, 24'b0});
  @(posedge clk);
  @(posedge clk);

  // Set bits 0-3 of the modem control resigster
  wbm2.wb_wr1(`UART_REG_MC, 4'b0001, {24'b0, 8'hFF});
  @(posedge clk);
  @(posedge clk);
  // Reset bits 0-3 of the modem control resigster
  wbm2.wb_wr1(`UART_REG_MC, 4'b0001, {24'b0, 8'h00});
  @(posedge clk);
  @(posedge clk);

  // Clear Rx and Tx FIFO
  wbm2.wb_wr1(`UART_REG_FC, 4'b0100, {8'b0, 8'h06, 16'h0000});
  @(posedge clk);
  @(posedge clk);
  // Change interrupt level
  wbm2.wb_wr1(`UART_REG_FC, 4'b0100, {8'b0, 8'h80, 16'h0000});
  @(posedge clk);
  @(posedge clk);
  // Change interrupt level back
  wbm2.wb_wr1(`UART_REG_FC, 4'b0100, {8'b0, 8'h00, 16'h0000});
  @(posedge clk);
  @(posedge clk);

  // Enable all interrupts
  wbm2.wb_wr1(`UART_REG_IE, 4'b0010, {16'd0, 8'hFF, 8'd0}); 
  @(posedge clk);
  @(posedge clk);
  // Disable all interrupts
  wbm2.wb_wr1(`UART_REG_IE, 4'b0010, {16'd0, 8'h00, 8'd0}); 
  @(posedge clk);
  @(posedge clk);

  // Set scratch register
  wbm2.wb_wr1(`UART_REG_SR, 4'b1000, {8'hFF, 24'd0}); 
  @(posedge clk);
  @(posedge clk);
  // Reset scratch register
  wbm2.wb_wr1(`UART_REG_SR, 4'b1000, {8'h00, 24'd0}); 
  @(posedge clk);
  @(posedge clk);

  // Read the LSR
  wbm2.wb_rd1(`UART_REG_LS, 4'b0010, dat_o);
  @(posedge clk);
  // Read the IIR
  wbm2.wb_rd1(`UART_REG_II, 4'b0100, dat_o);
  @(posedge clk);
  // Read the LSR
  wbm2.wb_rd1(`UART_REG_LS, 4'b0010, dat_o);
  @(posedge clk);

  // Change word size to 7-bits
  wbm2.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011010, 24'b0});
  @(posedge clk);
  @(posedge clk);

  // Change word size to 8-bits
  wbm2.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011011, 24'b0});

  // enable interrupts
  wbm2.wb_wr1(`UART_REG_IE, 4'b0010, {16'b0, 8'b00001111, 8'b0});
  @(posedge clk);

  // disable interrupts
  wbm2.wb_wr1(`UART_REG_IE, 4'b0010, {16'b0, 8'b00000000, 8'b0});
  @(posedge clk);

  // enable interrupts
  wbm2.wb_wr1(`UART_REG_IE, 4'b0010, {16'b0, 8'b00001111, 8'b0});


  wait(uart2.regs.receiver.rf_count == 16);

  e = 6400;
  while (e > 0)
  begin
    @(posedge clk)
    if (uart2.regs.enable) e = e - 1;
  end

  wbm2.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm2.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm2.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm2.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm2.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm2.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm2.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm2.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm2.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm2.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm2.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm2.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm2.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm2.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm2.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  @(posedge clk);

  wbm2.wb_rd1(0, 4'b1, dat_o);
  $display("%m : %t : Data out: %h", $time, dat_o);
  $display("%m : Finish");

  // NOW SENDING
  fork
  begin
    sendbyte2(8'b10000001);
    sendbyte2(8'b01000010);
    sendbyte2(8'b11000011);
    sendbyte2(8'b00100100);
    sendbyte2(8'b10100101);
    sendbyte2(8'b01100110);
    sendbyte2(8'b11100111);
    sendbyte2(8'b00011000);
    sendbyte2(8'b10000001);
    sendbyte2(8'b01000010);
    sendbyte2(8'b11000011);
    sendbyte2(8'b00100100);
    sendbyte2(8'b10100101);
    sendbyte2(8'b01100110);
    sendbyte2(8'b11100111);
    sendbyte2(8'b00011000);

    wait (uart2.regs.tstate==0 && uart2.regs.transmitter.tf_count==0);
//    disable check;
  end
//  begin: check
//  end
  join
end

//always @(uart_rcv.regs.rstate)
//begin
//  $display($time,": Receiver state changed to: ", uart_rcv.regs.rstate);
//end

initial
  begin
    `ifdef DATA_BUS_WIDTH_8
$display("DATA BUS IS 8");
`else
$display("DATA BUS IS 32");
`endif
    $display("%d %d", `UART_ADDR_WIDTH, `UART_DATA_WIDTH);

  end


always
begin
  #5 clkr = ~clk;
end

endmodule
