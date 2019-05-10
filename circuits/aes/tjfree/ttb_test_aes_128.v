/*
 * Copyright 2012, Homer Hsing <homer.hsing@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Simulation timescale
`timescale 1ns / 100ps

// Defines
`define CLOCK_PERIOD      10
`define VCD_FILENAME      "aes-tjfree"
`define AES_CYCLE_LATENCY 21
`define AES_KEY_SIZE      128
`define STATE_LFSR_SEED   128'hDEAD_BEEF_DEAD_BEEF_DEAD_BEEF_DEAD_BEEF
`define KEY_LFSR_SEED     128'hCAFE_FEED_CAFE_FEED_CAFE_FEED_CAFE_FEED

module ttb_test_aes_128;
    
    // Parameters
    parameter NUM_BITS = `AES_KEY_SIZE;

    // Command Line Args
    reg [31:0] num_tests;
    reg load_status;

    // Clock
    reg lfsr_clk;
    reg aes_clk;

    // AES Inputs
    reg aes_enable;

    // AES Outputs
    wire [`AES_KEY_SIZE - 1:0] out;

    // LSFR Outputs/AES Inputs
    wire [`AES_KEY_SIZE - 1:0] random_state;
    wire [`AES_KEY_SIZE - 1:0] random_key; 

    // LSFRs Inputs
    reg load_state_seed;
    reg lfsr_state_enable;
    reg load_key_seed;
    reg lfsr_key_enable;

    // LSFR Outputs
    wire lfsr_state_done;
    wire lfsr_key_done;

    // Instantiate the Unit Under Test (DUT)
    aes_128 dut (
        .clk(aes_clk), 
        .state(random_state), 
        .key(random_key), 
        .out(out)
    );

    // Instantiate 128-bit LFSR for generating random state (plain text)
    lfsr #(.NUM_BITS(`AES_KEY_SIZE)) state_lfsr (
        .i_Clk(lfsr_clk),
        .i_Enable(lfsr_state_enable),
        .i_Seed_DV(load_state_seed),
        .i_Seed_Data(`STATE_LFSR_SEED),
        .o_LFSR_Data(random_state),
        .o_LFSR_Done(lfsr_state_done)
    );

    // Instantiate 128-bit LFSR for generating random AES key 
    lfsr #(.NUM_BITS(`AES_KEY_SIZE)) key_lfsr (
        .i_Clk(lfsr_clk),
        .i_Enable(lfsr_key_enable),
        .i_Seed_DV(load_key_seed),
        .i_Seed_Data(`KEY_LFSR_SEED),
        .o_LFSR_Data(random_key),
        .o_LFSR_Done(lfsr_key_done)
    );

    // Set clock rates
    always #(`CLOCK_PERIOD / 2) aes_clk  <= ~aes_clk & aes_enable;
    always #(`CLOCK_PERIOD / 2) lfsr_clk <= ~lfsr_clk;

    // Test bench
    initial begin

        // Get arg for number of tests to run
        if (! $value$plusargs("num_tests=%d", num_tests)) begin
            $display("ERROR: please specify +num_tests=<value> to start.");
            $finish;
        end
        $display("Starting %4d tests...", num_tests);

        // Open VCD file
        $dumpfile({`VCD_FILENAME,".vcd"});
        $dumpvars(0, dut);

        // Initialize values
        aes_clk           = 1'b0;
        aes_enable        = 1'b0;
        lfsr_clk          = 1'b1;
        lfsr_state_enable = 1'b0;
        lfsr_key_enable   = 1'b0;
        load_state_seed   = 1'b1;
        load_key_seed     = 1'b1;

        // Wait 1.5 (LFSR) clock periods
        #(`CLOCK_PERIOD);
        #(`CLOCK_PERIOD / 2);

        // Enable LFSRs and Load Seeds
        lfsr_state_enable = 1'b1;
        lfsr_key_enable   = 1'b1;
        
        // Disable Seeding
        #(`CLOCK_PERIOD);
        load_state_seed = 1'b0;
        load_key_seed   = 1'b0;

        // Wait a clock period and start AES encryptions
        #(`CLOCK_PERIOD);
        aes_enable = 1'b1;

        // Wait for first encryption to be done
        #(`CLOCK_PERIOD * `AES_CYCLE_LATENCY);
        #(`CLOCK_PERIOD * (num_tests - 1));
        aes_enable = 1'b0;

        // // Test complete
        $display("Completed %4d tests.", num_tests);
        $finish;
    end     
endmodule
