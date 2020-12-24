//////////////////////////////////////////////////////////////////////
///                                                               //// 
/// ORPSoC ML501 testbench                                        ////
///                                                               ////
/// Instantiate ORPSoC, monitors, provide stimulus                ////
///                                                               ////
/// Julius Baxter, julius@opencores.org                           ////
///                                                               ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2009, 2010 Authors and OPENCORES.ORG           ////
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

`include "orpsoc-defines.v"
`include "orpsoc-testbench-defines.v"
`include "test-defines.v"
`include "timescale.v"

module orpsoc_testbench;

	// Clock and Reset signal registers
	reg clk   = 0;
	reg rst_n = 1; // Active LOW

	// Index for dumping arrayed registers (i.e. memories) to VCD
	integer z;

	// Program Filename to Load
	integer     k;
	reg [31:0]  program_index;
	reg [31:0]  num_tests;
	reg [31:0]  test_num;
	reg [1023:0] memory_file;
	 
	// Define Clock
	always
		#((`BOARD_CLOCK_PERIOD)/2) clk <= ~clk;

	wire clk_n;
	wire clk_p;
	assign clk_p = clk;
	assign clk_n = ~clk;

	// Include design parameters file
	`include "orpsoc-params.v"

	// 
	// I2C pullup bus
	// 
	tri1 i2c_scl, i2c_sda;
	 
	// 
	// JTAG Connections
	// 
`ifdef JTAG_DEBUG
	wire tdo_pad_o;
	wire tck_pad_i;
	wire tms_pad_i;
	wire tdi_pad_i;
`endif

	// 
	// UART Connections
	// 
`ifdef UART0
	wire uart0_stx_pad_o;
	wire uart0_srx_pad_i;
`endif

	// 
	// GPIO Connections
	// 
`ifdef GPIO0
	wire [gpio0_io_width-1:0] gpio0_io;
`endif
	
	// 
	// SPI Connections
	// 
`ifdef SPI0
	wire                     spi0_mosi_o;
	wire                     spi0_miso_i;
	wire                     spi0_sck_o;
	wire                     spi0_hold_n_o;
	wire                     spi0_w_n_o;   
	wire [spi0_ss_width-1:0] spi0_ss_o;
`endif
	
	// 
	// Ethernet Connections
	// 
`ifdef ETH0
	wire       mtx_clk_o;		
	wire [3:0] ethphy_mii_tx_d;	
	wire       ethphy_mii_tx_en;	
	wire       ethphy_mii_tx_err;	
	wire       mrx_clk_o;		
	wire [3:0] mrxd_o;			
	wire       mrxdv_o;		
	wire       mrxerr_o;		
	wire       mcoll_o;		
	wire       mcrs_o;
	wire       ethphy_rst_n;
	wire       eth0_mdc_pad_o;
	wire       eth0_md_pad_io;
`endif
	
	// 
	// SSRAM Connections
	// 
`ifdef XILINX_SSRAM
	wire        sram_clk;
	wire        sram_clk_fb;
	wire        sram_adv_ld_n;
	wire [3:0]  sram_bw;
	wire 		sram_cen;
	wire [21:1] sram_flash_addr;
	wire [31:0] sram_flash_data;
	wire        sram_flash_oe_n;
	wire        sram_flash_we_n;
	wire        sram_mode;
`endif

	// 
	// Instantiate DUT 
	// 
	orpsoc_top dut(
`ifdef XILINX_SSRAM
		.sram_clk        (sram_clk),
		.sram_flash_addr (sram_flash_addr),
		.sram_cen        (sram_cen),
		.sram_flash_oe_n (sram_flash_oe_n),
		.sram_flash_we_n (sram_flash_we_n),
		.sram_bw         (sram_bw),
		.sram_adv_ld_n   (sram_adv_ld_n),
		.sram_mode       (sram_mode),
		.sram_clk_fb     (sram_clk_fb),
		.sram_flash_data (sram_flash_data),
`endif
`ifdef UART0      
		.uart0_stx_pad_o           (uart0_stx_pad_o),
		.uart0_srx_pad_i           (uart0_srx_pad_i),
		.uart0_stx_expheader_pad_o (uart0_stx_pad_o),
		.uart0_srx_expheader_pad_i (uart0_srx_pad_i),
`endif
`ifdef SPI0
		.spi0_mosi_o (spi0_mosi_o),
		.spi0_ss_o   (spi0_ss_o),
`endif
`ifdef I2C0
		.i2c0_sda_io (i2c_sda),
		.i2c0_scl_io (i2c_scl),
`endif
`ifdef I2C1
		.i2c1_sda_io (i2c_sda),
		.i2c1_scl_io (i2c_scl),
`endif
`ifdef GPIO0
		.gpio0_io (gpio0_io),
`endif
		.sys_clk_in_p (clk_p),
		.sys_clk_in_n (clk_n),
		.rst_n_pad_i  (rst_n)      
	);

	//
	// Instantiate OR1200 monitor
	//
 or1200_monitor monitor();
`include "or1200_defines.v"
`include "or1200_monitor_defines.v"
`include "test-defines.v"
	 integer    r3;

	//
	// Get GPR
	//
	task get_gpr;
		input	[4:0] gpr_no;
		output [31:0] gpr;
		integer       j;
		begin

`ifdef OR1200_RFRAM_GENERIC
			for(j = 0; j < 32; j = j + 1) begin
				gpr[j] = `OR1200_TOP.`CPU_cpu.`CPU_rf.rf_a.mem[gpr_no*32+j];
			end
`else
			//gpr = `OR1200_TOP.`CPU_cpu.`CPU_rf.rf_a.mem[gpr_no];
			gpr = `OR1200_TOP.`CPU_cpu.`CPU_rf.rf_a.get_gpr(gpr_no);
`endif


		end
	endtask

	//
	// Hooks for:
	// - displaying registers
	// - end of simulation
	// - access to SPRs
	//
	always @(posedge `CPU_CORE_CLK)
		if (!`OR1200_TOP.`CPU_cpu.`CPU_ctrl.wb_freeze) begin
			if (((`OR1200_TOP.`CPU_cpu.`CPU_ctrl.wb_insn[31:26] != `OR1200_OR32_NOP) | !`OR1200_TOP.`CPU_cpu.`CPU_ctrl.wb_insn[16])
				& !(`OR1200_TOP.`CPU_cpu.`CPU_except.except_flushpipe & `OR1200_TOP.`CPU_cpu.`CPU_except.ex_dslot))
			;
		else
			// small hack to stop simulation (l.nop 1):
			if (`OR1200_TOP.`CPU_cpu.`CPU_ctrl.wb_insn == 32'h1500_0001) begin
				get_gpr(3, r3);
				$display("%t: l.nop exit (%h)", $time, r3);
				$finish;
			end
			// debug if test (l.nop 10)
			if (`OR1200_TOP.`CPU_cpu.`CPU_ctrl.wb_insn == 32'h1500_000a) begin
				$display("%t: l.nop dbg_if_test", $time);
			end
			// simulation reports (l.nop 2)
			if (`OR1200_TOP.`CPU_cpu.`CPU_ctrl.wb_insn == 32'h1500_0002) begin 
				get_gpr(3, r3);
				$display("%t: l.nop report (0x%h)", $time, r3);
			end
			// simulation printfs (l.nop 3)
			if (`OR1200_TOP.`CPU_cpu.`CPU_ctrl.wb_insn == 32'h1500_0003) begin 
				get_gpr(3, r3);
				$display("%t: l.nop printf (%h)", $time, r3);
			end
			// simulation putc (l.nop 4)
			if (`OR1200_TOP.`CPU_cpu.`CPU_ctrl.wb_insn == 32'h1500_0004) begin 
				get_gpr(3, r3);
				$write("%c", r3);
				$display("%t: l.nop putc (%c)", $time, r3);
			end
		end

	// 
	// Notify on Cache Enables
	// 
	`define CPU_ic_top or1200_ic_top
	`define CPU_dc_top or1200_dc_top
	
	wire ic_en = orpsoc_testbench.dut.or1200_top0.or1200_ic_top.ic_en;
	wire dc_en = orpsoc_testbench.dut.or1200_top0.or1200_dc_top.dc_en;
	
	always @(posedge ic_en)
		$display("Or1200 IC enabled at %t", $time);
	
	always @(posedge dc_en)
		$display("Or1200 DC enabled at %t", $time);


	// 
	// Debugging Interface(s)
	// 
`ifdef JTAG_DEBUG   
`ifdef VPI_DEBUG
	
	vpi_debug_module vpi_dbg(
		.tms(tms_pad_i), 
		.tck(tck_pad_i), 
		.tdi(tdi_pad_i), 
		.tdo(tdo_pad_o)
	);

	`else   

		// If no VPI debugging, tie off JTAG inputs
		assign tdi_pad_i = 1;
		assign tck_pad_i = 0;
		assign tms_pad_i = 1;

`endif // !`ifdef VPI_DEBUG_ENABLE
`endif //  `ifdef JTAG_DEBUG
	 
	// 
	// SPI Interface
	// 
`ifdef SPI0

	// STARTUP_VIRTEX5 module routes these out on the board.
	// So for now just connect directly to the internals here.
	assign spi0_sck_o      = dut.spi0_sck_o;
	assign dut.spi0_miso_i = spi0_miso_i;
	 
	 // SPI flash memory - M25P16 compatible SPI protocol
	 // 2MB flash on ML501
	 AT26DFxxx #(.MEMSIZE(2048*1024)) spi0_flash(
	 	// Outputs
		.SO	(spi0_miso_i),
		
		// Inputs
		.CSB (spi0_ss_o),
		.SCK (spi0_sck_o),
		.SI	 (spi0_mosi_o),
		.WPB (1'b1)
	);

`endif //  `ifdef SPI0

	// 
	// Ethernet Interface
	// 
`ifdef ETH0
	 
	/* TX/RXes packets and checks them, enabled when ethernet MAC is */
	`include "eth_stim.v"

	eth_phy eth_phy0(
		
		// Outputs
		.mtx_clk_o   (mtx_clk_o),
		.mrx_clk_o   (mrx_clk_o),
		.mrxd_o      (mrxd_o[3:0]),
		.mrxdv_o     (mrxdv_o),
		.mrxerr_o    (mrxerr_o),
		.mcoll_o     (mcoll_o),
		.mcrs_o	     (mcrs_o),
		.link_o      (),
		.speed_o     (), 
		.duplex_o    (),
		.smii_clk_i  (1'b0),
		.smii_sync_i (1'b0),
		.smii_rx_o   (),
		
		// Inouts
		.md_io (eth0_md_pad_io),
		
		// Inputs
 	`ifndef ETH0_PHY_RST

		// If no reset out from the design, hook up to the board's active low rst
		.m_rst_n_i (rst_n),
		
		`else

		.m_rst_n_i (ethphy_rst_n),
		
	`endif

		.mtxd_i	  (ethphy_mii_tx_d[3:0]),
		.mtxen_i  (ethphy_mii_tx_en),
		.mtxerr_i (ethphy_mii_tx_err),
		.mdc_i    (eth0_mdc_pad_o));

`endif //  `ifdef ETH0

	// 
	// SSRAM Interface
	// 
`ifdef XILINX_SSRAM
	wire [18:0] sram_a;
	wire [3:0]  dqp;   
	wire        sram_ce1b; 
	wire        sram_ce2;
	wire        sram_ce3b;

	assign sram_a[18:0] = sram_flash_addr[19:1];   
	assign sram_ce1b = 1'b0;
	assign sram_ce2 = 1'b1;   
	assign sram_ce3b = 1'b0;   
	assign sram_clk_fb = sram_clk;   

	cy7c1354 ssram0(
		// Inouts
		// This model puts each parity bit after each byte, but the ML501's part
		// doesn't, so we wire up the data bus like so.
		.d ({dqp[3],sram_flash_data[31:24],
			 dqp[2],sram_flash_data[23:16],
			 dqp[1],sram_flash_data[15:8],
			 dqp[0],sram_flash_data[7:0]}),

		// Inputs
		.clk	(sram_clk),
		.we_b	(sram_flash_we_n),
		.adv_lb	(sram_adv_ld_n),
		.ce1b	(sram_ce1b),
		.ce2	(sram_ce2),
		.ce3b	(sram_ce3b),
		.oeb	(sram_flash_oe_n),
		.cenb	(sram_cen),
		.mode	(sram_mode),
		.bws	(sram_bw),
		.a		(sram_a));
`endif

	// 
	// DDR2 Interface
	// 
`ifdef XILINX_DDR2
	`ifndef GATE_SIM
		defparam dut.xilinx_ddr2_0.xilinx_ddr2_if0.ddr2_mig0.SIM_ONLY = 1;
	`endif

	always @( * ) begin
		ddr2_ck_sdram     <=  #(TPROP_PCB_CTRL) ddr2_ck_fpga;
		ddr2_ck_n_sdram   <=  #(TPROP_PCB_CTRL) ddr2_ck_n_fpga;
		ddr2_a_sdram      <=  #(TPROP_PCB_CTRL) ddr2_a_fpga;
		ddr2_ba_sdram     <=  #(TPROP_PCB_CTRL) ddr2_ba_fpga;
		ddr2_ras_n_sdram  <=  #(TPROP_PCB_CTRL) ddr2_ras_n_fpga;
		ddr2_cas_n_sdram  <=  #(TPROP_PCB_CTRL) ddr2_cas_n_fpga;
		ddr2_we_n_sdram   <=  #(TPROP_PCB_CTRL) ddr2_we_n_fpga;
		ddr2_cs_n_sdram   <=  #(TPROP_PCB_CTRL) ddr2_cs_n_fpga;
		ddr2_cke_sdram    <=  #(TPROP_PCB_CTRL) ddr2_cke_fpga;
		ddr2_odt_sdram    <=  #(TPROP_PCB_CTRL) ddr2_odt_fpga;
		ddr2_dm_sdram_tmp <=  #(TPROP_PCB_DATA) ddr2_dm_fpga; //DM signal generation
	end // always @ ( * )
	 
	// Model delays on bi-directional BUS
	genvar dqwd;
	generate
		for (dqwd = 0;dqwd < DQ_WIDTH;dqwd = dqwd+1) begin : dq_delay
			wiredelay #(.Delay_g (TPROP_PCB_DATA), .Delay_rd (TPROP_PCB_DATA_RD)) u_delay_dq(
				.A     (ddr2_dq_fpga[dqwd]),
				.B     (ddr2_dq_sdram[dqwd]),
				.reset (rst_n));
		end
	endgenerate
	 
	genvar dqswd;
	generate
		for (dqswd = 0;dqswd < DQS_WIDTH;dqswd = dqswd+1) begin : dqs_delay
			wiredelay #(.Delay_g (TPROP_DQS), .Delay_rd (TPROP_DQS_RD)) u_delay_dqs(
				.A     (ddr2_dqs_fpga[dqswd]),
				.B     (ddr2_dqs_sdram[dqswd]),
				.reset (rst_n));
	 
			wiredelay #(.Delay_g (TPROP_DQS), .Delay_rd (TPROP_DQS_RD)) u_delay_dqs_n(
				.A     (ddr2_dqs_n_fpga[dqswd]),
				.B     (ddr2_dqs_n_sdram[dqswd]),
				.reset (rst_n));
		end
	endgenerate
	 
	assign ddr2_dm_sdram = ddr2_dm_sdram_tmp;
	parameter NUM_PROGRAM_WORDS = 67108864;
	integer   program_word_ptr;
	integer   k;
	integer   burst_address;
	reg [31:0] program_array [0:NUM_PROGRAM_WORDS-1]; // 1M words = 4MB
	reg [8*16-1:0] ddr2_ram_mem_line; //8*16-bits= 8 shorts (half-words)
	genvar i, j;
	generate
		// if the data width is multiple of 16
		for(j = 0; j < CS_NUM; j = j+1) begin : gen_cs // Loop of 1
			for(i = 0; i < DQS_WIDTH/2; i = i+1) begin : gen // Loop of 4 (DQS_WIDTH=8)
				initial begin

			// `ifdef PRELOAD_RAM
				`include "ddr2_model_preload.v"
			//`endif
				end
			
			ddr2_model u_mem0(
				 .ck      (ddr2_ck_sdram[CLK_WIDTH*i/DQS_WIDTH]),
				 .ck_n    (ddr2_ck_n_sdram[CLK_WIDTH*i/DQS_WIDTH]),
				 .cke     (ddr2_cke_sdram[j]),
				 .cs_n    (ddr2_cs_n_sdram[CS_WIDTH*i/DQS_WIDTH]),
				 .ras_n   (ddr2_ras_n_sdram),
				 .cas_n   (ddr2_cas_n_sdram),
				 .we_n    (ddr2_we_n_sdram),
				 .dm_rdqs (ddr2_dm_sdram[(2*(i+1))-1 : i*2]),
				 .ba      (ddr2_ba_sdram),
				 .addr    (ddr2_a_sdram),
				 .dq      (ddr2_dq_sdram[(16*(i+1))-1 : i*16]),
				 .dqs     (ddr2_dqs_sdram[(2*(i+1))-1 : i*2]),
				 .dqs_n   (ddr2_dqs_n_sdram[(2*(i+1))-1 : i*2]),
				 .rdqs_n  (),
				 .odt     (ddr2_odt_sdram[ODT_WIDTH*i/DQS_WIDTH]));
			end
		end
	endgenerate
	 
`endif

	// 
	// VCD definintion
	// 
`ifdef VCD

	reg vcd_go = 0;

	always @(vcd_go) begin

		// Delay by x insns
		`ifdef VCD_DELAY_INSNS
			#10; // Delay until after the value becomes valid
			while (monitor.insns < `VCD_DELAY_INSNS)
				@(posedge clk);
	 	`endif	

		`ifdef SIMULATOR_MODELSIM
			
			// Modelsim can GZip VCDs on the fly if given in the suffix
			`define VCD_SUFFIX ".vcd.gz"
	 	`else
			`define VCD_SUFFIX ".vcd"
		`endif

		// Set VCD file path
		$display("* VCD in %s\n", `VCD_FILENAME);
		$dumpfile(`VCD_FILENAME);
		
		`ifndef VCD_DEPTH
			`define VCD_DEPTH 0
	 	`endif 
		
		// Set variables to dump to VCD
		$dumpvars(`VCD_DEPTH, `VCD_PATH);

		// Dump all arrayed variables
        for(z = 0; z < 32; z++) begin
        	$dumpvars(0, orpsoc_testbench.dut.or1200_top0.or1200_cpu.or1200_rf.rf_a.mem[z]);
        	$dumpvars(0, orpsoc_testbench.dut.or1200_top0.or1200_cpu.or1200_rf.rf_b.mem[z]);
        end
        // for(z = 0; z < 16; z++) begin
            // $dumpvars(0, orpsoc_testbench.dut.uart16550_0.regs.receiver.fifo_rx.rfifo.ram[z]);
            // $dumpvars(0, orpsoc_testbench.dut.uart16550_0.regs.transmitter.fifo_tx.tfifo.ram[z]);
            // $dumpvars(0, orpsoc_testbench.dut.uart16550_0.regs.receiver.fifo_rx.fifo[z]);
        // end
	end

`endif // ifdef VCD
	 
	initial begin

		$display("Starting simulation...");	
	
	`ifdef VCD
		vcd_go = 1;
	`endif

	end

	// 
	// Check for program finish signal and start next program
	// 
	integer i;
	initial begin
		
		// Set testing signals
		orpsoc_testbench.dut.or1200_dbg_stb_i_reg = 0;

		// Get cmd line arg for number of tests to run
		test_num = 0;
        if (! $value$plusargs("num_tests=%d", num_tests)) begin
        	$display("ERROR: please specify +num_tests=<value> to start.");
            $finish;
        end

        // Get cmd line arg for program index to run
        if (! $value$plusargs("program_index=%d", program_index)) begin
        	$display("Program index not specified... setting to 0.");
            program_index = 0;
        end

		#1;
		monitor.program_exit = 1;
	end
	always @(posedge monitor.program_exit) begin

		$display("--------------------------------------------------------------------------------");

		// Check if time to switch program
		if (test_num == num_tests) begin
			$display("Simulation Complete (time: %t)", $time);
			test_num = 0;
			program_index++;
		end

		// Check if all simulations complete
		if (program_index == 8) begin
			#1;
			orpsoc_testbench.dut.or1200_dbg_stb_i_reg = 1;
			#100;
			orpsoc_testbench.dut.or1200_dbg_stb_i_reg = 0;
			#100;
			$display("All Program Simulations Complete (time: %t)", $time);
			$finish;
		end

		// Reset DUT
		$display("Putting DUT into Reset (time: %t)...", $time);
		repeat (32) @(negedge clk)
			rst_n <= 1;
		repeat (32) @(negedge clk)
			rst_n <= 0;
		$display();

		// Select program to load
		if (program_index == 0) begin
			$display("Loading simulation program (time: %t): or1k-dsx.vmem", $time);
			memory_file = "../../third_party/or1200/simulation/vmem/or1k-dsx.vmem";
			// $display("Loading simulation program (time: %t): or1k-custom.vmem", $time);
			// memory_file = "../../third_party/or1200/simulation/vmem/or1k-custom.vmem";
		end
		else if (program_index == 1) begin
			$display("Loading simulation program (time: %t): or1k-insnfetcherror.vmem", $time);
			memory_file = "../../third_party/or1200/simulation/vmem/or1k-insnfetcherror.vmem";
		end
		else if (program_index == 2) begin
			$display("Loading simulation program (time: %t): or1k-lsu.vmem", $time);
			memory_file = "../../third_party/or1200/simulation/vmem/or1k-lsu.vmem";
		end
		else if (program_index == 3) begin
			$display("Loading simulation program (time: %t): or1k-lwjr.vmem", $time);
			memory_file = "../../third_party/or1200/simulation/vmem/or1k-lwjr.vmem";
		end
		else if (program_index == 4) begin
			$display("Loading simulation program (time: %t): or1k-ov.vmem", $time);
			memory_file = "../../third_party/or1200/simulation/vmem/or1k-ov.vmem";
		end
		else if (program_index == 5) begin
			$display("Loading simulation program (time: %t): or1k-shortjump.vmem", $time);
			memory_file = "../../third_party/or1200/simulation/vmem/or1k-shortjump.vmem";
		end
		else if (program_index == 6) begin
			$display("Loading simulation program (time: %t): or1k-ticksyscall.vmem", $time);
			memory_file = "../../third_party/or1200/simulation/vmem/or1k-ticksyscall.vmem";
		end
		else if (program_index == 7) begin
			$display("Loading simulation program (time: %t): or1k-custom.vmem", $time);
			memory_file = "../../third_party/or1200/simulation/vmem/or1k-custom.vmem";
		end
		else begin
			$display("ERROR: unkown program index (time: %t)", $time);
			$finish;
		end

		// 0-out memory on processor reset
		for(k = 0; k < orpsoc_testbench.dut.ram_wb0.ram_wb_b3_0.mem_words; k++) begin
			orpsoc_testbench.dut.ram_wb0.ram_wb_b3_0.mem[k] = 1'b0;
		end

		// Load program into memory
		$readmemh(memory_file, orpsoc_testbench.dut.ram_wb0.ram_wb_b3_0.mem);

		// print memory stats
		$display("Memory Address Width (Bits): %12d", orpsoc_testbench.dut.ram_wb0.ram_wb_b3_0.mem_adr_width);
		$display("Memory Size (Num Words):     %12d", orpsoc_testbench.dut.ram_wb0.ram_wb_b3_0.mem_words);
		$display("Memory Size (Num Bytes):     %12d", orpsoc_testbench.dut.ram_wb0.ram_wb_b3_0.mem_size_bytes);
		$display();
		
		// Clear Program Exit Flag
		monitor.program_exit = 0;
			
		// Complete DUT Reset
		repeat (32) @(negedge clk)
			rst_n <= 1;
		$display("DUT Reset Complete (time: %t).", $time);
		$display();

		// Increment Test Case Counter
		test_num++;

		// // Check if simulation done
		// if (test_num == num_tests) begin
		// 	$display("Simulation Complete (time: %t)", $time);
		// 	$finish;
		// end
		// else begin

		// 	// Reset DUT
		// 	$display("Putting DUT into Reset (time: %t)...", $time);
		// 	repeat (32) @(negedge clk)
		// 		rst_n <= 1;
		// 	repeat (32) @(negedge clk)
		// 		rst_n <= 0;
		// 	$display();

		// 	if (program_index == 0) begin
		// 		$display("Loading simulation program (time: %t): helloworld.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/helloworld.vmem";
		// 	end
		// 	else if (program_index == 1) begin
		// 		$display("Loading simulation program (time: %t): or1k-alignillegalinsn.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-alignillegalinsn.vmem";
		// 	end
		// 	else if (program_index == 2) begin
		// 		$display("Loading simulation program (time: %t): or1k-backtoback_jmp.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-backtoback_jmp.vmem";
		// 	end
		// 	else if (program_index == 3) begin
		// 		$display("Loading simulation program (time: %t): or1k-basic.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-basic.vmem";
		// 	end
		// 	else if (program_index == 4) begin
		// 		$display("Loading simulation program (time: %t): or1k-cmov.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-cmov.vmem";
		// 	end
		// 	else if (program_index == 5) begin
		// 		$display("Loading simulation program (time: %t): or1k-cy.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-cy.vmem";
		// 	end
		// 	else if (program_index == 6) begin
		// 		$display("Loading simulation program (time: %t): or1k-dsxinsn.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-dsxinsn.vmem";
		// 	end
		// 	else if (program_index == 7) begin
		// 		$display("Loading simulation program (time: %t): or1k-dsx.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-dsx.vmem";
		// 	end
		// 	else if (program_index == 8) begin
		// 		$display("Loading simulation program (time: %t): or1k-ext.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-ext.vmem";
		// 	end
		// 	else if (program_index == 9) begin
		// 		$display("Loading simulation program (time: %t): or1k-ffl1.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-ffl1.vmem";
		// 	end
		// 	else if (program_index == 10) begin
		// 		$display("Loading simulation program (time: %t): or1k-icache.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-icache.vmem";
		// 	end
		// 	else if (program_index == 11) begin
		// 		$display("Loading simulation program (time: %t): or1k-illegalinsndelayslot.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-illegalinsndelayslot.vmem";
		// 	end
		// 	else if (program_index == 12) begin
		// 		$display("Loading simulation program (time: %t): or1k-illegalinsn.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-illegalinsn.vmem";
		// 	end
		// 	else if (program_index == 13) begin
		// 		$display("Loading simulation program (time: %t): or1k-insnfetchalign.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-insnfetchalign.vmem";
		// 	end
		// 	else if (program_index == 14) begin
		// 		$display("Loading simulation program (time: %t): or1k-insnfetcherror.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-insnfetcherror.vmem";
		// 	end
		// 	else if (program_index == 15) begin
		// 		$display("Loading simulation program (time: %t): or1k-intloop.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-intloop.vmem";
		// 	end
		// 	else if (program_index == 16) begin
		// 		$display("Loading simulation program (time: %t): or1k-intmulticycle.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-intmulticycle.vmem";
		// 	end
		// 	else if (program_index == 17) begin
		// 		$display("Loading simulation program (time: %t): or1k-intsyscall.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-intsyscall.vmem";
		// 	end
		// 	else if (program_index == 18) begin
		// 		$display("Loading simulation program (time: %t): or1k-inttickloop.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-inttickloop.vmem";
		// 	end
		// 	else if (program_index == 19) begin
		// 		$display("Loading simulation program (time: %t): or1k-jmp.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-jmp.vmem";
		// 	end
		// 	else if (program_index == 20) begin
		// 		$display("Loading simulation program (time: %t): or1k-jr.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-jr.vmem";
		// 	end
		// 	else if (program_index == 21) begin
		// 		$display("Loading simulation program (time: %t): or1k-lsualigndelayslot.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-lsualigndelayslot.vmem";
		// 	end
		// 	else if (program_index == 22) begin
		// 		$display("Loading simulation program (time: %t): or1k-lsualign.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-lsualign.vmem";
		// 	end
		// 	else if (program_index == 23) begin
		// 		$display("Loading simulation program (time: %t): or1k-lsuerrordelayslot.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-lsuerrordelayslot.vmem";
		// 	end
		// 	else if (program_index == 24) begin
		// 		$display("Loading simulation program (time: %t): or1k-lsuerror.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-lsuerror.vmem";
		// 	end
		// 	else if (program_index == 25) begin
		// 		$display("Loading simulation program (time: %t): or1k-lsu.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-lsu.vmem";
		// 	end
		// 	else if (program_index == 26) begin
		// 		$display("Loading simulation program (time: %t): or1k-lwjr.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-lwjr.vmem";
		// 	end
		// 	else if (program_index == 27) begin
		// 		$display("Loading simulation program (time: %t): or1k-mmu.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-mmu.vmem";
		// 	end
		// 	else if (program_index == 28) begin
		// 		$display("Loading simulation program (time: %t): or1k-mul.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-mul.vmem";
		// 	end
		// 	else if (program_index == 29) begin
		// 		$display("Loading simulation program (time: %t): or1k-mul-basic.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-mul-basic.vmem";
		// 	end
		// 	else if (program_index == 30) begin
		// 		$display("Loading simulation program (time: %t): or1k-msync.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-msync.vmem";
		// 	end
		// 	else if (program_index == 31) begin
		// 		$display("Loading simulation program (time: %t): or1k-newlibirq.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-newlibirq.vmem";
		// 	end
		// 	else if (program_index == 32) begin
		// 		$display("Loading simulation program (time: %t): or1k-ovcy.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-ovcy.vmem";
		// 	end
		// 	else if (program_index == 33) begin
		// 		$display("Loading simulation program (time: %t): or1k-ov.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-ov.vmem";
		// 	end
		// 	else if (program_index == 34) begin
		// 		$display("Loading simulation program (time: %t): or1k-regjmp.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-regjmp.vmem";
		// 	end
		// 	else if (program_index == 35) begin
		// 		$display("Loading simulation program (time: %t): or1k-rfe.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-rfe.vmem";
		// 	end
		// 	else if (program_index == 36) begin
		// 		$display("Loading simulation program (time: %t): or1k-sfbf.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-sfbf.vmem";
		// 	end
		// 	else if (program_index == 37) begin
		// 		$display("Loading simulation program (time: %t): or1k-sf.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-sf.vmem";
		// 	end
		// 	else if (program_index == 38) begin
		// 		$display("Loading simulation program (time: %t): or1k-shiftopts.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-shiftopts.vmem";
		// 	end
		// 	else if (program_index == 39) begin
		// 		$display("Loading simulation program (time: %t): or1k-shortbranch.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-shortbranch.vmem";
		// 	end
		// 	else if (program_index == 40) begin
		// 		$display("Loading simulation program (time: %t): or1k-shortjump.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-shortjump.vmem";
		// 	end
		// 	else if (program_index == 41) begin
		// 		$display("Loading simulation program (time: %t): or1k-simple.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-simple.vmem";
		// 	end
		// 	else if (program_index == 42) begin
		// 		$display("Loading simulation program (time: %t): or1k-systemcall.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-systemcall.vmem";
		// 	end
		// 	else if (program_index == 43) begin
		// 		$display("Loading simulation program (time: %t): or1k-tickloop.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-tickloop.vmem";
		// 	end
		// 	else if (program_index == 44) begin
		// 		$display("Loading simulation program (time: %t): or1k-tickrfforward.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-tickrfforward.vmem";
		// 	end
		// 	else if (program_index == 45) begin
		// 		$display("Loading simulation program (time: %t): or1k-ticksyscall.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-ticksyscall.vmem";
		// 	end
		// 	else if (program_index == 46) begin
		// 		$display("Loading simulation program (time: %t): or1k-timer.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-timer.vmem";
		// 	end
		// 	else if (program_index == 47) begin
		// 		$display("Loading simulation program (time: %t): or1k-trapdelayslot.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-trapdelayslot.vmem";
		// 	end
		// 	else if (program_index == 48) begin
		// 		$display("Loading simulation program (time: %t): or1k-trap.vmem", $time);
		// 		memory_file = "../../third_party/or1200/simulation/vmem/or1k-trap.vmem";
		// 	end
		// 	else begin
		// 		$display("ERROR: unkown program index (time: %t)", $time);
		// 		$finish;
		// 	end

		// 	// // Set simulation program filename
		// 	// if (program_index == 0) begin
		// 	// 	$display("Loading simulation program (time: %t): helloworld.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/helloworld.vmem";
		// 	// end
		// 	// else if (program_index == 1) begin
		// 	// 	$display("Loading simulation program (time: %t): aes.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/aes.vmem";
		// 	// end
		// 	// else if (program_index == 2) begin
		// 	// 	$display("Loading simulation program (time: %t): basicmath.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/basicmath.vmem";
		// 	// end
		// 	// else if (program_index == 3) begin
		// 	// 	$display("Loading simulation program (time: %t): blowfish.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/blowfish.vmem";
		// 	// end
		// 	// else if (program_index == 4) begin
		// 	// 	$display("Loading simulation program (time: %t): crc.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/crc.vmem";
		// 	// end
		// 	// else if (program_index == 5) begin
		// 	// 	$display("Loading simulation program (time: %t): dijkstra.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/dijkstra.vmem";
		// 	// end
		// 	// else if (program_index == 6) begin
		// 	// 	$display("Loading simulation program (time: %t): fft.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/fft.vmem";
		// 	// end
		// 	// else if (program_index == 7) begin
		// 	// 	$display("Loading simulation program (time: %t): limits.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/limits.vmem";
		// 	// end
		// 	// else if (program_index == 8) begin
		// 	// 	$display("Loading simulation program (time: %t): lzfx.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/lzfx.vmem";
		// 	// end
		// 	// else if (program_index == 9) begin
		// 	// 	$display("Loading simulation program (time: %t): qsort.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/qsort.vmem";
		// 	// end
		// 	// else if (program_index == 10) begin
		// 	// 	$display("Loading simulation program (time: %t): randmath.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/randmath.vmem";
		// 	// end
		// 	// else if (program_index == 11) begin
		// 	// 	$display("Loading simulation program (time: %t): rc4.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/rc4.vmem";
		// 	// end
		// 	// else if (program_index == 12) begin
		// 	// 	$display("Loading simulation program (time: %t): rsa.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/rsa.vmem";
		// 	// end
		// 	// else if (program_index == 13) begin
		// 	// 	$display("Loading simulation program (time: %t): sha.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/sha.vmem";
		// 	// end
		// 	// else if (program_index == 14) begin
		// 	// 	$display("Loading simulation program (time: %t): stringsearch.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/stringsearch.vmem";
		// 	// end
		// 	// else if (program_index == 15) begin
		// 	// 	$display("Loading simulation program (time: %t): susan.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/susan.vmem";
		// 	// end
		// 	// else if (program_index == 16) begin
		// 	// 	// $display("Loading simulation program (time: %t): syscall.vmem", $time);
		// 	// 	// memory_file = "../../third_party/or1200/simulation/vmem/syscall.vmem";
		// 	// 	$display("Loading simulation program (time: %t): or1k-systemcall.vmem", $time);
		// 	// 	memory_file = "../../third_party/or1200/simulation/vmem/or1k-systemcall.vmem";
		// 	// end
		// 	// else begin
		// 	// 	$display("ERROR: unkown program index (time: %t)", $time);
		// 	// 	$finish;
		// 	// end

		// 	// 0-out memory on processor reset
		// 	for(k = 0; k < orpsoc_testbench.dut.ram_wb0.ram_wb_b3_0.mem_words; k++) begin
		// 		orpsoc_testbench.dut.ram_wb0.ram_wb_b3_0.mem[k] = 1'b0;
		// 	end

		// 	// Load program into memory
		// 	$readmemh(memory_file, orpsoc_testbench.dut.ram_wb0.ram_wb_b3_0.mem);

		// 	// print memory stats
		// 	$display("Memory Address Width (Bits): %12d", orpsoc_testbench.dut.ram_wb0.ram_wb_b3_0.mem_adr_width);
		// 	$display("Memory Size (Num Words):     %12d", orpsoc_testbench.dut.ram_wb0.ram_wb_b3_0.mem_words);
		// 	$display("Memory Size (Num Bytes):     %12d", orpsoc_testbench.dut.ram_wb0.ram_wb_b3_0.mem_size_bytes);
		// 	$display();
			
		// 	// Clear Program Exit Flag
		// 	monitor.program_exit = 0;
			
		// 	// Complete DUT Reset
		// 	repeat (32) @(negedge clk)
		// 		rst_n <= 1;
		// 	$display("DUT Reset Complete (time: %t).", $time);
		// 	$display();

		// 	// Increment Test Case Counter
		// 	test_num++;
		// end
	end

	// 
	// End simulation after specified amount of TIME
	// 
`ifdef END_TIME
	initial begin
		#(`END_TIME);     
		$display("* Finish simulation due to END_TIME being set at %t", $time);    
		$finish;
	end
`endif

	// 
	// End simulation after specified amount of INSTRUCTIONS
	// 
`ifdef END_INSNS
	initial begin
		#10
		while (monitor.insns < `END_INSNS)
			@(posedge clk);      
		$display("* Finish simulation due to END_INSNS count (%d) reached at %t", `END_INSNS, $time);
		$finish;
	end
`endif     
	
	// 
	// UART decoder (for enable STDIO)
	// 
`ifdef UART0   
	
	// 115200 baud = period 8.68uS
	uart_decoder #(.uart_baudrate_period_ns(8680)) uart0_decoder (
		.clk(clk),
		.uart_tx(uart0_stx_pad_o));
	 
	// Loopback UART lines
	assign uart0_srx_pad_i = uart0_stx_pad_o;

`endif // `ifdef UART0
	 
endmodule // orpsoc_testbench
