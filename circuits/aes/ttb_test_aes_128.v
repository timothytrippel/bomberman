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
`define AES_CYCLE_LATENCY 21
`define AES_KEY_SIZE      128
`define STATE_LFSR_SEED   128'hDEAD_BEEF_DEAD_BEEF_DEAD_BEEF_DEAD_BEEF
`define KEY_LFSR_SEED     128'hCAFE_FEED_CAFE_FEED_CAFE_FEED_CAFE_FEED
`define IMG_FILENAME      "mona_lisa.jpg"
`define IMG_SIZE          100
`define TEST_SET_REPEATS  2

module ttb_test_aes_128;
    
    // Parameters
    parameter NUM_BITS = `AES_KEY_SIZE;

    // Command Line Args
    reg [31:0] num_tests;

    // Test type (random = 0; file = 1)
    reg test_type;

    // Clock
    reg lfsr_clk;
    reg aes_clk;

    // AES Inputs
    reg aes_enable;
    wire [`AES_KEY_SIZE - 1:0] aes_state;
    wire [`AES_KEY_SIZE - 1:0] aes_key;

    // AES Outputs
    wire [`AES_KEY_SIZE - 1:0] out;

    // LSFR Outputs/AES Inputs
    reg  [`AES_KEY_SIZE - 1:0] seed_state;
    reg  [`AES_KEY_SIZE - 1:0] seed_key; 
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

    // IMG encryption test variables
    reg [7:0] img_bytes [`IMG_SIZE - 1: 0];
    reg [`AES_KEY_SIZE - 1:0] img_state;
    integer file_id;
    integer file_read_status;
    integer i;

    // Assign Key and State
    assign aes_key   = test_type ? `KEY_LFSR_SEED : random_key;
    assign aes_state = test_type ? img_state : random_state; 

    // Instantiate the Unit Under Test (DUT)
    aes_128 dut (
        .clk(aes_clk), 
        .state(aes_state), 
        .key(aes_key), 
        .out(out)
    );

    // Instantiate 128-bit LFSR for generating random state (plain text)
    lfsr #(.NUM_BITS(`AES_KEY_SIZE)) state_lfsr (
        .i_Clk(lfsr_clk),
        .i_Enable(lfsr_state_enable),
        .i_Seed_DV(load_state_seed),
        .i_Seed_Data(seed_state),
        .o_LFSR_Data(random_state),
        .o_LFSR_Done(lfsr_state_done)
    );

    // Instantiate 128-bit LFSR for generating random AES key 
    lfsr #(.NUM_BITS(`AES_KEY_SIZE)) key_lfsr (
        .i_Clk(lfsr_clk),
        .i_Enable(lfsr_key_enable),
        .i_Seed_DV(load_key_seed),
        .i_Seed_Data(seed_key),
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

        // Get arg for key LFSR seed
        if (! $value$plusargs("seed_key=%h", seed_key)) begin
            $display("INFO: +seed_key=<value> not specified... using default.");
            seed_key = `KEY_LFSR_SEED;
        end

        // Get arg for state LFSR seed
        if (! $value$plusargs("seed_state=%h", seed_state)) begin
            $display("INFO: +seed_state=<value> not specified... using default.");
            seed_state = `STATE_LFSR_SEED;
        end

        // Open VCD file
        $dumpfile(`VCD_FILENAME);
        $dumpvars(0, dut);

        for (i = 0; i < `TEST_SET_REPEATS; i++) begin
            
            // Initialize values
            test_type         = 1'b0;
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

            // Start random encryptions
            $display("Starting %4d random encryptions (time: %t)...", num_tests, $time);

            // Seed LFSRs
            $display("Seeding state LFSR with value: %32h", seed_state);
            $display("Seeding key   LFSR with value: %32h", seed_key);

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

            // Wait for all encryptions to be done
            #(`CLOCK_PERIOD * (num_tests - 1));
            aes_enable = 1'b0;
            #(`CLOCK_PERIOD / 2);
            lfsr_state_enable = 1'b0;
            lfsr_key_enable   = 1'b0;

            // Random tests complete
            $display("Completed %4d random encryptions (time: %t).", num_tests, $time);
        end

        // // Start image encryption test
        // $display("Starting image encryption (time: %t)...", $time);

        // // Open image file
        // file_id = $fopen(`IMG_FILENAME, "rb");
        // if (file_id == 0) $error ("Error Open File: %s", `IMG_FILENAME);

        // #(`CLOCK_PERIOD / 2);
        // test_type = 1'b1;

        // // Load and encrypt image
        // i = 0;
        // @(posedge lfsr_clk);

        // while (!$feof(file_id) && i < `IMG_SIZE) begin
        //     file_read_status = $fread(img_state, file_id);
        //     $display ("Encrypting img_byte (%d) = %h", i, img_state);
        //     if (i == 0) aes_enable = 1'b1;
        //     @(posedge aes_clk)
        //     i++;
        // end

        // // Wait for all encryptions to be done
        // #(`CLOCK_PERIOD * `AES_CYCLE_LATENCY);
        // aes_enable = 1'b0;

        // // Close image file
        // $display("Completed image encryption.");
        // $fclose(file_id);

        // End testbench
        $finish;
    end
endmodule
