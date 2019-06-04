//--------------------------------------------------------------------------------
// Global Defines
//--------------------------------------------------------------------------------

//  Defines
`define CLOCK_PERIOD       10 // in ns
`define RECEIVE_FIFO_SIZE  16 // in bytes
`define TRANSMIT_FIFO_SIZE 16 // in bytes
`define LFSR_SEED          0
//`define DATA_BUS_WIDTH_8

// Define data bus width
`ifdef DATA_BUS_WIDTH_8
    `define DATA_BUS_WIDTH 8
`else
    `define DATA_BUS_WIDTH 32
`endif

//--------------------------------------------------------------------------------
// Global Includes
//--------------------------------------------------------------------------------
// Set timscale
`include "timescale.v"

//--------------------------------------------------------------------------------
// Test Bench Module
//--------------------------------------------------------------------------------
module uart_test ();
    
    //--------------------------------------------------------------------------------
    // Includes
    //--------------------------------------------------------------------------------
    `include "uart_defines.v"

    //--------------------------------------------------------------------------------
    // Signal Definitions
    //--------------------------------------------------------------------------------

    // Test bench driving signals
    reg clk; // Clock (drives each UART block and Wishbone interfaces)
    reg rst; // Reset (ties into Wishbone interface)

    // Wishbone/UART 1 Signals (receiver fifo)
    wire [`UART_ADDR_WIDTH-1:0] wb1_adr_i;
    wire [31:0]                 wb1_dat_i;
    wire [31:0]                 wb1_dat_o;
    wire  [3:0]                 wb1_sel_i;
    wire                        stx1_o;
    reg                         srx1_ir;
    wire                        int1_o;

    // Wishbone/UART 2 Signals (?? transmit fifo ??)
    wire [`UART_ADDR_WIDTH-1:0] wb2_adr_i;
    wire [31:0]                 wb2_dat_i;
    wire [31:0]                 wb2_dat_o;
    wire [3:0]                  wb2_sel_i;
    wire                        stx2_o;
    reg                         srx2_ir;
    wire                        int2_o;

    // Book-keeping signals and values
    // WB Data Outputs
    reg [31:0] data1_o;
    reg [31:0] data2_o;

    // LFSR Signals
    // Input LFSR
    wire [`DATA_BUS_WIDTH - 1:0] random_byte_o;
    wire                         lfsr_done_o;
    reg                          lfsr_enable_i;
    reg                          lfsr_load_seed_i;
    // Compare LFSR
    wire [`DATA_BUS_WIDTH - 1:0] comp_random_byte_o;
    wire                         comp_lfsr_done_o;
    reg                          comp_lfsr_enable_i;
    reg                          comp_lfsr_load_seed_i;

    // Other
    integer i;
    integer e;
    integer num_tests;
    reg uart2_initialized;

    //--------------------------------------------------------------------------------
    // DUT Module Instantiations
    //--------------------------------------------------------------------------------

    // Instantiations
    // Instantiate UART Module 1 (Main DUT)
    uart_top  uart1(
        
        // Clock
        .wb_clk_i (clk), 
        
        // Wishbone signals
        .wb_rst_i (rst), 
        .wb_adr_i (wb1_adr_i), 
        .wb_dat_i (wb1_dat_i), 
        .wb_dat_o (wb1_dat_o), 
        .wb_we_i  (wb1_we_i), 
        .wb_stb_i (wb1_stb_i), 
        .wb_cyc_i (wb1_cyc_i), 
        .wb_ack_o (wb1_ack_o),  
        .wb_sel_i (wb1_sel_i),

        // interrupt request
        .int_o (int1_o), 

        // UART signals
        // serial input/output
        .stx_pad_o (stx1_o), 
        .srx_pad_i (srx1_ir),

        // modem signals
        .rts_pad_o (rts1_o), 
        .cts_pad_i (1'b1), 
        .dtr_pad_o (dtr1_o), 
        .dsr_pad_i (1'b1), 
        .ri_pad_i  (1'b1), 
        .dcd_pad_i (1'b1)
    );

    // Instantiate UART Module 2 (Helper DUT)
    uart_top  uart2(

        // Clock
        .wb_clk_i (clk), 
        
        // Wishbone signals
        .wb_rst_i (rst), 
        .wb_adr_i (wb2_adr_i), 
        .wb_dat_i (wb2_dat_i), 
        .wb_dat_o (wb2_dat_o), 
        .wb_we_i  (wb2_we_i), 
        .wb_stb_i (wb2_stb_i), 
        .wb_cyc_i (wb2_cyc_i), 
        .wb_ack_o (wb2_ack_o), 
        .wb_sel_i (wb2_sel_i),

        // interrupt request
        .int_o (int2_o), 

        // UART signals
        // serial input/output
        .stx_pad_o (stx2_o), 
        .srx_pad_i (srx2_ir),

        // modem signals
        .rts_pad_o (rts2_o), 
        .cts_pad_i (1'b1), 
        .dtr_pad_o (dtr2_o), 
        .dsr_pad_i (1'b1), 
        .ri_pad_i  (1'b1), 
        .dcd_pad_i (1'b1)
    );

    // Instantiate Wishbone Master Module 1
    wb_mast wbm1(
        
        // Outputs
        .adr  (wb1_adr_i),
        .dout (wb1_dat_i),
        .cyc  (wb1_cyc_i),
        .stb  (wb1_stb_i),
        .sel  (wb1_sel_i),
        .we   (wb1_we_i),
        
        // Inputs
        .clk (clk),
        .rst (rst),
        .din (wb1_dat_o),
        .ack (wb1_ack_o),
        .err (1'b0),
        .rty (1'b0)
    );

    // Instantiate Wishbone Master Module 2
    wb_mast wbm2(
        // Outputs
        .adr  (wb2_adr_i),
        .dout (wb2_dat_i),
        .cyc  (wb2_cyc_i),
        .stb  (wb2_stb_i),
        .sel  (wb2_sel_i),
        .we   (wb2_we_i),

        // Inputs
        .clk (clk),
        .rst (rst),
        .din (wb2_dat_o),
        .ack (wb2_ack_o),
        .err (1'b0),
        .rty (1'b0)
    );

    //--------------------------------------------------------------------------------
    // Other Module Instantiations
    //--------------------------------------------------------------------------------
    
    // Instantiate 8-bit LFSR for generating random bytes to transmit
    lfsr #(.NUM_BITS(8)) input_lfsr (
        .i_Clk(clk),
        .i_Enable(lfsr_enable_i),
        .i_Seed_DV(lfsr_load_seed_i),
        .i_Seed_Data(`LFSR_SEED),
        .o_LFSR_Data(random_byte_o),
        .o_LFSR_Done(lfsr_done_o)
    );

    // Instantiate 8-bit LFSR for comparing against
    lfsr #(.NUM_BITS(8)) compare_lfsr (
        .i_Clk(clk),
        .i_Enable(comp_lfsr_enable_i),
        .i_Seed_DV(comp_lfsr_load_seed_i),
        .i_Seed_Data(`LFSR_SEED),
        .o_LFSR_Data(comp_random_byte_o),
        .o_LFSR_Done(comp_lfsr_done_o)
    );

    //--------------------------------------------------------------------------------
    // Sub-Module Connections
    //--------------------------------------------------------------------------------
    // Connect the UARTS (latching the TX/RX bits)
    always @(stx2_o) begin
        srx1_ir = stx2_o;  
    end
    always @(stx1_o) begin
        srx2_ir = stx1_o;
    end

    //--------------------------------------------------------------------------------
    // Load Command Line Arg(s)
    //--------------------------------------------------------------------------------

    // Get arg for number of tests to run
    initial begin
        if (! $value$plusargs("num_tests=%d", num_tests)) begin
            $display("ERROR: please specify +num_tests=<value> to start.");
            $finish;
        end
        $display("Starting %4d test(s)...", num_tests);
    end

    //--------------------------------------------------------------------------------
    // Print Settings
    //--------------------------------------------------------------------------------
    
    initial
    begin
        `ifdef DATA_BUS_WIDTH_8
            $display("DATA BUS IS 8");
        `else
            $display("DATA BUS IS 32");
        `endif
        $display("Address Width: %2d; Data Width: %2d", `UART_ADDR_WIDTH, `UART_DATA_WIDTH);
    end

    //--------------------------------------------------------------------------------
    // VCD File Initializations
    //-------------------------------------------------------------------------------- 
    
    initial begin

        // Set VCD filename
        $dumpfile(`VCD_FILENAME);

        // Dump all variables
        $dumpvars(0, uart1);

        // Dump all arrayed variables
        for(i = 0; i < 16; i++)
            $dumpvars(0, uart1.regs.receiver.fifo_rx.rfifo.ram[i]);
    end

    //--------------------------------------------------------------------------------
    // Clock/Reset Initializations
    //--------------------------------------------------------------------------------

    // set clock rate
    always #(`CLOCK_PERIOD / 2) clk  <= ~clk;

    // initialize clock to 0
    initial begin
        clk = 0;
    end

    // initialize reset
    initial begin
        #1               rst = 1'b1;
        #(`CLOCK_PERIOD) rst = 1'b0;
    end

    // define LFSR reset
    always @(posedge rst) begin
        #1
        lfsr_enable_i         <= 1'b1;
        comp_lfsr_enable_i    <= 1'b1;
        lfsr_load_seed_i      <= 1'b1;
        comp_lfsr_load_seed_i <= 1'b1;
        #(`CLOCK_PERIOD)
        lfsr_enable_i         <= 1'b0;
        comp_lfsr_enable_i    <= 1'b0;
        lfsr_load_seed_i      <= 1'b0;
        comp_lfsr_load_seed_i <= 1'b0;
    end

    // initialize status flags
    initial begin
        #1 uart2_initialized = 1'b0;
    end

    //--------------------------------------------------------------------------------
    // Subroutines
    //--------------------------------------------------------------------------------
    
    // Transmit Byte with UART-1
    task sendbyte1;
        
        // Inputs
        input [7:0] byte;
        
        begin
            #(`CLOCK_PERIOD)
            lfsr_enable_i = 1'b0;
            $display("Time: %10t (ns): UART-1 sending : %h", $time, byte);
            wbm1.wb_wr1(0, 4'b1, {24'b0, byte});
            @(posedge clk);
            @(posedge clk);
            #(1)
            lfsr_enable_i = 1'b1;
        end
    endtask

    // Transmit Byte with UART-2
    task sendbyte2;

        // Inputs
        input [7:0] byte;

        begin
            // $display("Time: %10t (ns): UART-2 sending : %h", $time, byte);
            wbm2.wb_wr1(0, 4'b1, {24'b0, byte});
            @(posedge clk);
            @(posedge clk);
        end
    endtask

    // Receive Byte with UART-1
    task receivebyte1;
        begin
            wbm1.wb_rd1(0, 4'b1, data1_o);
            $display("Time: %10t (ns): UART-1 receiving: %h", $time, data1_o[7:0]);
            @(posedge clk);
        end
    endtask

    // Receive Byte with UART-2
    task receivebyte2;
        begin
            wbm2.wb_rd1(0, 4'b1, data2_o);
            // $display("Time: %10t (ns): UART-2 receiving: %h", $time, data2_o[7:0]);
            @(posedge clk);
        end
    endtask

    task wait2clocks;
        begin
            @(posedge clk);
            @(posedge clk);
        end
    endtask

    //--------------------------------------------------------------------------------
    // Testbench (Main DUT)
    //--------------------------------------------------------------------------------

    initial begin
        
        #(`CLOCK_PERIOD + 1);

        //----------------------------------------------------------------------------
        // Initialize UART-1 (Main DUT)
        //----------------------------------------------------------------------------

        // Initalize lcr 
        wbm1.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011011, 24'b0});
        wait2clocks();

        //write to lcr. set bit 7 to 1 (DLL and DLM accessible)
        wbm1.wb_wr1(`UART_REG_LC, 4'b1000, {8'b10011011, 24'b0});
        // set dl to divide by 2
        wbm1.wb_wr1(`UART_REG_DL1,4'b0001, 32'd2);
        wait2clocks();

        // set dl to divide by 3
        wbm1.wb_wr1(`UART_REG_DL1,4'b0001, 32'd3);
        wait2clocks();

        // set dl to divide by 2
        wbm1.wb_wr1(`UART_REG_DL1,4'b0001, 32'd2);
        wait2clocks();

        // restore normal regiters by resetting bit 7 in lcr
        wbm1.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011011, 24'b0}); //00011011
        wait2clocks();

        // Set bits 0-3 of the modem control resigster
        wbm1.wb_wr1(`UART_REG_MC, 4'b0001, {24'b0, 8'hFF});
        wait2clocks();
        // Reset bits 0-3 of the modem control resigster
        wbm1.wb_wr1(`UART_REG_MC, 4'b0001, {24'b0, 8'h00});
        wait2clocks();

        // Clear Rx and Tx FIFO
        wbm1.wb_wr1(`UART_REG_FC, 4'b0100, {8'b0, 8'h06, 16'h0000});
        wait2clocks();
        // Change interrupt level
        wbm1.wb_wr1(`UART_REG_FC, 4'b0100, {8'b0, 8'h80, 16'h0000});
        wait2clocks();
        // Change interrupt level back
        wbm1.wb_wr1(`UART_REG_FC, 4'b0100, {8'b0, 8'h00, 16'h0000});
        wait2clocks();

        // Enable all interrupts
        wbm1.wb_wr1(`UART_REG_IE, 4'b0010, {16'd0, 8'hFF, 8'd0}); 
        wait2clocks();
        // Disable all interrupts
        wbm1.wb_wr1(`UART_REG_IE, 4'b0010, {16'd0, 8'h00, 8'd0}); 
        wait2clocks();

        // Set scratch register
        wbm1.wb_wr1(`UART_REG_SR, 4'b1000, {8'hFF, 24'd0}); 
        wait2clocks();
        // Reset scratch register
        wbm1.wb_wr1(`UART_REG_SR, 4'b1000, {8'h00, 24'd0}); 
        wait2clocks();

        // Read the LSR
        wbm1.wb_rd1(`UART_REG_LS, 4'b0010, data1_o);
        @(posedge clk);
        // Read the IIR
        wbm1.wb_rd1(`UART_REG_II, 4'b0100, data1_o);
        @(posedge clk);
        // Read the LSR
        wbm1.wb_rd1(`UART_REG_LS, 4'b0010, data1_o);
        @(posedge clk);

        // Change word size to 7-bits
        wbm1.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011010, 24'b0});
        wait2clocks();

        // Change word size to 8-bits
        wbm1.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011011, 24'b0});

        //----------------------------------------------------------------------------
        // Run <num_tests> tests
        //----------------------------------------------------------------------------

        repeat(num_tests) begin

            //------------------------------------------------------------------------
            // Transmit Bytes from UART-1 (Main DUT) to UART-2 (Helper DUT)
            //------------------------------------------------------------------------

            // Enable LFSR
            lfsr_enable_i = 1'b1;
            repeat(`TRANSMIT_FIFO_SIZE) begin
                sendbyte1(random_byte_o);
            end
            lfsr_enable_i = 1'b0;

            // Wait for all bytes to be transmitted out of UART-1
            wait (uart1.regs.tstate==0 && uart1.regs.transmitter.tf_count==0);

            //------------------------------------------------------------------------
            // Receive Bytes from UART-2 (Helper DUT)
            //------------------------------------------------------------------------

            // Now receiving
            // enable interrupts
            wbm1.wb_wr1(`UART_REG_IE, 4'b0010, {16'b0, 8'b00001111, 8'b0});
            @(posedge clk);

            // disable interupts
            wbm1.wb_wr1(`UART_REG_IE, 4'b0010, {16'b0, 8'b00000000, 8'b0});
            @(posedge clk);

            // enable interrupts
            wbm1.wb_wr1(`UART_REG_IE, 4'b0010, {16'b0, 8'b00001111, 8'b0});

            // wait until the reciever FIFO is full
            wait(uart1.regs.receiver.rf_count == `RECEIVE_FIFO_SIZE);
         
            e = 6400;
            while (e > 0)
            begin
                @(posedge clk)
                if (uart1.regs.enable) e = e - 1;
            end

            // Retrieve all bytes from WB bus
            repeat(`RECEIVE_FIFO_SIZE) begin
                receivebyte1();

                // Check that recieved bytes is correct
                if ( data1_o[7:0] !== comp_random_byte_o)
                  begin $display("ERROR: send/receive of byte 0x%2h failed.", data1_o[7:0]); $finish; end
                
                // Update compare LFSR
                #(1);
                comp_lfsr_enable_i = 1'b1;
                #(`CLOCK_PERIOD);
                comp_lfsr_enable_i = 1'b0;
            end
        
            $display("%m : UART-1 receive finished.");
        end

        $finish;
    end

    //--------------------------------------------------------------------------------
    // Initialize UART-2 (Helper DUT)
    //--------------------------------------------------------------------------------

    initial begin
        
        #(`CLOCK_PERIOD + 1);

        //Init LCR
        wbm2.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011011, 24'b0});
        wait2clocks();

        //write to lcr. set bit 7
        wbm2.wb_wr1(`UART_REG_LC, 4'b1000, {8'b10011011, 24'b0});
        // set dl to divide by 2
        wbm2.wb_wr1(`UART_REG_DL1, 4'b1, 32'd2);
        wait2clocks();

        // set dl to divide by 3
        wbm2.wb_wr1(`UART_REG_DL1,4'b0001, 32'd3);
        wait2clocks();

        // set dl to divide by 2
        wbm2.wb_wr1(`UART_REG_DL1,4'b0001, 32'd2);
        wait2clocks();

        // restore normal registers
        wbm2.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011011, 24'b0});
        wait2clocks();

        // Set bits 0-3 of the modem control resigster
        wbm2.wb_wr1(`UART_REG_MC, 4'b0001, {24'b0, 8'hFF});
        wait2clocks();
        // Reset bits 0-3 of the modem control resigster
        wbm2.wb_wr1(`UART_REG_MC, 4'b0001, {24'b0, 8'h00});
        wait2clocks();

        // Clear Rx and Tx FIFO
        wbm2.wb_wr1(`UART_REG_FC, 4'b0100, {8'b0, 8'h06, 16'h0000});
        wait2clocks();
        // Change interrupt level
        wbm2.wb_wr1(`UART_REG_FC, 4'b0100, {8'b0, 8'h80, 16'h0000});
        wait2clocks();
        // Change interrupt level back
        wbm2.wb_wr1(`UART_REG_FC, 4'b0100, {8'b0, 8'h00, 16'h0000});
        wait2clocks();

        // Enable all interrupts
        wbm2.wb_wr1(`UART_REG_IE, 4'b0010, {16'd0, 8'hFF, 8'd0}); 
        wait2clocks();
        // Disable all interrupts
        wbm2.wb_wr1(`UART_REG_IE, 4'b0010, {16'd0, 8'h00, 8'd0}); 
        wait2clocks();

        // Set scratch register
        wbm2.wb_wr1(`UART_REG_SR, 4'b1000, {8'hFF, 24'd0}); 
        wait2clocks();
        // Reset scratch register
        wbm2.wb_wr1(`UART_REG_SR, 4'b1000, {8'h00, 24'd0}); 
        wait2clocks();

        // Read the LSR
        wbm2.wb_rd1(`UART_REG_LS, 4'b0010, data2_o);
        @(posedge clk);
        // Read the IIR
        wbm2.wb_rd1(`UART_REG_II, 4'b0100, data2_o);
        @(posedge clk);
        // Read the LSR
        wbm2.wb_rd1(`UART_REG_LS, 4'b0010, data2_o);
        @(posedge clk);

        // Change word size to 7-bits
        wbm2.wb_wr1(`UART_REG_LC, 4'b1000, {8'b00011010, 24'b0});
        wait2clocks();

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

        // Set initialized flag
        uart2_initialized = 1'b1;
    end

    //----------------------------------------------------------------------------
    // Receive Bytes from UART-1 (Main DUT) and always Transmit back
    //----------------------------------------------------------------------------

    always @(uart2.regs.receiver.rf_count == `RECEIVE_FIFO_SIZE) begin
        
        // Check that UART-2 (Helper DUT) has been initialized
        if (uart2_initialized == 1'b1) begin
            
            // Wait
            e = 6400;
            while (e > 0)
            begin
                @(posedge clk)
                if (uart2.regs.enable) e = e - 1;
            end

            // Retrieve all bytes from WB bus
            repeat(`RECEIVE_FIFO_SIZE) begin
                receivebyte2();
                sendbyte2(data2_o[7:0]);
            end
            
            // Wait for all bytes to be transmitted out of UART-2
            wait (uart2.regs.tstate==0 && uart2.regs.transmitter.tf_count==0);
            $display("UART-2 transmit/receive finished.");
        end
    end

endmodule
